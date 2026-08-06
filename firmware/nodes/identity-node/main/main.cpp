/**
 * @file main.cpp
 * @brief Identity Node runtime for the Ambient Physical AI system.
 *
 * This firmware implements the M5Dial-based identity interface responsible for:
 *
 * - receiving presence notifications through UDP;
 * - prompting the user to present an NFC card;
 * - detecting and reading NFC card identifiers through the WS1850S;
 * - mapping known card UIDs to local identity profiles;
 * - allowing local context selection through the rotary encoder;
 * - rendering profile and runtime state information on the display;
 * - generating the canonical Identity Package;
 * - transmitting identity information to the Cognitive Runtime through UDP.
 *
 * FreeRTOS architecture:
 *
 * - The UI task owns M5.update(), display rendering, touch input, encoder
 *   processing, buzzer feedback, and application-level identity state.
 * - The NFC task owns WS1850S initialization, polling, UID acquisition, and
 *   controlled recovery.
 * - The UDP listener task receives presence events from the Presence Node.
 * - The Identity Event Queue transfers NFC and presence events to the UI task.
 *
 * Shared I2C access:
 *
 * - A mutex protects only short transactions involving M5Dial internal
 *   peripherals and the WS1850S.
 * - Display rendering and other non-I2C UI work are intentionally performed
 *   outside the mutex.
 * - Temporary NFC quiet windows reduce contention while the user is actively
 *   interacting with the touch interface or rotary encoder.
 *
 * NFC behavior:
 *
 * - ESP_ERR_NOT_FOUND during UID acquisition is treated as normal card absence
 *   or removal rather than as a critical driver failure.
 * - Repeated driver or bus failures are handled by a staged recovery state
 *   machine.
 *
 * Current implementation boundaries:
 *
 * - NFC identity resolution currently uses local UID mapping.
 * - NDEF profile decoding is not implemented in this runtime version.
 * - Identity Packages are transported directly to the AX630C over UDP.
 */

#include "M5Unified.h"
#include "esp_log.h"
#include "esp_err.h"
#include <errno.h>
#include "profile_image_manager.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include <stdio.h>
#include <string.h>

#include "bsp/m5dial.h"
#include "iot_knob.h"

#include "driver/i2c_master.h"
#include "ws1850s.h"

// Logging tag used by all Identity Node runtime components in this file.
static const char *TAG = "identity-node";

// -----------------------------------------------------------------------------
// Network configuration
//
// UDP_LISTEN_PORT receives presence events from the Presence Node.
// AX630C_IP and AX630C_UDP_PORT identify the Cognitive Runtime endpoint that
// receives generated Identity Packages.
// -----------------------------------------------------------------------------
/*
 * Update the Wi-Fi credentials below before building.
 * If static IP addressing is enabled, also adjust the network
 * configuration (IP, gateway and subnet mask) for your environment.
 */

#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASS "<YOUR_WIFI_PASSWORD>"

#define UDP_LISTEN_PORT 3333
#define UDP_RX_BUFFER_SIZE 256
#define AX630C_IP "192.168.1.202"
#define AX630C_UDP_PORT 4444

// -----------------------------------------------------------------------------
// Profiles / Contexts
// -----------------------------------------------------------------------------

/**
 * @brief Local identity profile resolved from an NFC card UID.
 *
 * The profile identifier is the canonical machine-readable value included in
 * the Identity Package. The name and role fields are used by both the local UI
 * and the outgoing identity contract.
 */
struct Profile {
    const char *id;
    const char *name;
    const char *role;
};

// Index zero is the safe fallback used for unknown, absent, or unmapped cards.
static const Profile profiles[] = {
    {"unknown", "Unknown", "visitor"},
    {"claudio", "Cláudio", "owner"},
    {"student", "Student", "learner"},
    {"mariana", "Mariana", "researcher"},
    {"herminio", "Hermínio", "researcher"},
};

/**
 * @brief Identifiers for the operational contexts selectable on the M5Dial.
 *
 * The enum values are used as stable indexes into the contexts table.
 */
enum ContextId {
    CONTEXT_LAB = 0,
    CONTEXT_RESEARCH,
    CONTEXT_CLASSROOM,
    CONTEXT_DEMO,
    CONTEXT_MEETING,
};

/**
 * @brief Defines the protocol and display representations of a context.
 *
 * protocol_name is serialized into the Identity Package.
 * display_name is rendered on the local Identity Node interface.
 */
struct ContextDefinition {
    ContextId id;
    const char *protocol_name;
    const char *display_name;
};

// Table order must remain aligned with ContextId because current_context is
// used directly as an array index.
static const ContextDefinition contexts[] = {
    {CONTEXT_LAB,       "Lab",       "Lab"},
    {CONTEXT_RESEARCH,  "Research",  "Research"},
    {CONTEXT_CLASSROOM, "Classroom", "Classroom"},
    {CONTEXT_DEMO,      "Demo",      "Demo"},
    {CONTEXT_MEETING,   "Meeting",   "Meeting"},
};

// Compile-time table sizes used for index validation and circular navigation.
static const int PROFILE_COUNT = sizeof(profiles) / sizeof(profiles[0]);
static const int CONTEXT_COUNT = sizeof(contexts) / sizeof(contexts[0]);

// -----------------------------------------------------------------------------
// Identity Events
// -----------------------------------------------------------------------------

/**
 * @brief Application-level events delivered to the UI task.
 *
 * Hardware-facing tasks publish these events instead of directly modifying the
 * display or application state. This preserves UI ownership within ui_task().
 */
enum IdentityEventType {
    EVENT_NFC_CARD_PRESENT,
    EVENT_NFC_CARD_REMOVED,
    EVENT_NFC_UID_READ,
    EVENT_NFC_ERROR,
    EVENT_NFC_RECOVERED,
    EVENT_PRESENCE_RECEIVED,
};

/**
 * @brief Message transferred through the Identity Event Queue.
 *
 * uid and profile_index are populated for successful NFC identity events.
 * message carries a short diagnostic or state description when applicable.
 */
struct IdentityEvent {
    IdentityEventType type;
    char uid[32];
    int profile_index;
    char message[64];
};

// Inter-task queue for NFC and presence events consumed by the UI task.
static QueueHandle_t identity_event_queue = NULL;
// Serializes short transactions on the shared M5Dial I2C bus.
static SemaphoreHandle_t i2c_bus_mutex = NULL;

// NFC polling is temporarily deferred while touch or encoder interaction is
// active. This reduces contention between M5Dial internal peripherals and the
// WS1850S on the shared I2C bus.
static volatile TickType_t ui_quiet_until = 0;

/**
 * @brief Defers NFC polling for a bounded period.
 *
 * @param duration Quiet-window duration expressed in FreeRTOS ticks.
 */
static void request_nfc_quiet_window(TickType_t duration)
{
    ui_quiet_until = xTaskGetTickCount() + duration;
}

/**
 * @brief Reports whether NFC polling must remain temporarily suspended.
 *
 * The signed tick difference keeps the comparison valid across normal
 * FreeRTOS tick-counter wraparound.
 *
 * @return true while the current tick is before ui_quiet_until.
 */
static bool is_ui_quiet_window_active()
{
    TickType_t now = xTaskGetTickCount();
    return ((int32_t)(ui_quiet_until - now)) > 0;
}


// -----------------------------------------------------------------------------
// UI Runtime State
// -----------------------------------------------------------------------------

// UI-owned indexes into the profile and context definition tables.
static int current_profile = 0;
static int current_context = 0;

// Application-level NFC status reflected in the UI and Identity Package.
static bool nfc_detected = false;
static bool nfc_card_present = false;

// Most recently accepted UID and human-readable NFC runtime status.
static char last_nfc_uid[32] = "";
static char last_nfc_status[64] = "NFC: starting";

// Temporary identity-confirmation screen state and expiration deadline.
static bool identity_visual_active = false;
static TickType_t identity_visual_until = 0;
static const TickType_t IDENTITY_VISUAL_DURATION = pdMS_TO_TICKS(3000);

// Temporary presence prompt state and expiration deadline.
static bool presence_prompt_active = false;
static TickType_t presence_prompt_until = 0;
static const TickType_t PRESENCE_PROMPT_DURATION = pdMS_TO_TICKS(5000);

// -----------------------------------------------------------------------------
// NFC Runtime State
// -----------------------------------------------------------------------------

// ESP-IDF I2C handles owned by the NFC runtime and rebuilt during recovery.
static i2c_master_bus_handle_t nfc_bus_handle = NULL;
static i2c_master_dev_handle_t nfc_dev_handle = NULL;

/**
 * @brief States of the NFC polling and recovery state machine.
 *
 * The machine separates normal polling, UID acquisition, card-removal
 * confirmation, error handling, and controlled cooldown periods.
 */
enum NfcState {
    NFC_INIT,
    NFC_IDLE,
    NFC_POLL,
    NFC_READ_UID,
    NFC_CARD_PRESENT,
    NFC_CARD_REMOVED,
    NFC_ERROR,
    NFC_COOLDOWN,
};

// Timing parameters for NFC startup, polling, retry, and recovery behavior.
static const TickType_t NFC_BOOT_DELAY       = pdMS_TO_TICKS(1200);
static const TickType_t NFC_IDLE_DELAY       = pdMS_TO_TICKS(120);
static const TickType_t NFC_POLL_DELAY       = pdMS_TO_TICKS(450);
static const TickType_t NFC_READ_DELAY       = pdMS_TO_TICKS(80);
static const TickType_t NFC_SHORT_COOLDOWN   = pdMS_TO_TICKS(300);
static const TickType_t NFC_MEDIUM_COOLDOWN  = pdMS_TO_TICKS(900);
static const TickType_t NFC_LONG_COOLDOWN    = pdMS_TO_TICKS(3000);

// Debounce and retry limits used to distinguish transient RF misses from a
// stable card removal or a persistent UID acquisition failure.
static const int CARD_REMOVED_CONFIRM_COUNT = 5;
static const int NFC_UID_READ_ATTEMPTS = 8;
static const TickType_t NFC_UID_RETRY_DELAY = pdMS_TO_TICKS(60);

// -----------------------------------------------------------------------------
// Network Runtime State
// -----------------------------------------------------------------------------

// Updated by the ESP-IDF event handler and observed by UDP runtime tasks.
static volatile bool wifi_connected = false;

// -----------------------------------------------------------------------------
// I2C lock helpers
// -----------------------------------------------------------------------------

/**
 * @brief Acquires exclusive access to the shared I2C bus.
 *
 * @param timeout Maximum time to wait, expressed in FreeRTOS ticks.
 *
 * @return true when the mutex was acquired; false if the mutex is unavailable
 *         or the timeout expires.
 */
static bool lock_i2c(TickType_t timeout)
{
    if (i2c_bus_mutex == NULL) {
        return false;
    }

    return xSemaphoreTake(i2c_bus_mutex, timeout) == pdTRUE;
}

/**
 * @brief Releases the shared I2C bus mutex when it exists.
 */
static void unlock_i2c()
{
    if (i2c_bus_mutex != NULL) {
        xSemaphoreGive(i2c_bus_mutex);
    }
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

/**
 * @brief Maps a hexadecimal NFC UID string to a local profile index.
 *
 * The returned value is used directly as an index into the profiles table.
 * Unrecognized UIDs fall back to index zero, which represents the unknown
 * visitor profile.
 *
 * @param uid Null-terminated uppercase hexadecimal UID string.
 *
 * @return Index of the matching profile, or zero when no mapping exists.
 */
static int find_profile_by_uid(const char *uid)
{
    if (strcmp(uid, "8804DC32") == 0) {
        return 1;
    }

    if (strcmp(uid, "88048667") == 0) {
        return 2;
    }

    if (strcmp(uid, "8804D225") == 0) {
        return 3;
   }

    if (strcmp(uid, "8804EB36") == 0) {
       return 4;
   }
    // Preserve the unknown-profile fallback for every unmapped card.
    return 0;
}

/**
 * @brief Publishes an event to the UI-owned Identity Event Queue.
 *
 * A bounded wait prevents producer tasks from blocking indefinitely when the
 * queue is temporarily full.
 *
 * @param event Event payload copied into the FreeRTOS queue.
 */
static void send_identity_event(const IdentityEvent &event)
{
    if (identity_event_queue != NULL) {
        // Queue delivery is best-effort and intentionally uses a short timeout.
        xQueueSend(identity_event_queue, &event, pdMS_TO_TICKS(20));
    }
}

/**
 * @brief Creates and publishes an event that does not require UID data.
 *
 * The helper zero-initializes the complete event structure and safely copies an
 * optional diagnostic message before forwarding it to the common queue sender.
 *
 * @param type Application event type.
 * @param message Optional null-terminated status or diagnostic message.
 */
static void send_simple_event(IdentityEventType type, const char *message = "")
{
    IdentityEvent event = {};
    event.type = type;

    if (message != NULL) {
        strncpy(event.message, message, sizeof(event.message) - 1);
        event.message[sizeof(event.message) - 1] = '\0';
    }

    send_identity_event(event);
}

// -----------------------------------------------------------------------------
// Wi-Fi STA + UDP Listener
// -----------------------------------------------------------------------------

/**
 * @brief Handles Wi-Fi station and IPv4 acquisition events.
 *
 * The handler initiates the first connection when the station starts,
 * requests reconnection after a disconnect event, and updates the shared
 * connectivity flag after DHCP assigns an IPv4 address.
 *
 * @param arg Unused user argument.
 * @param event_base ESP-IDF event base identifying the event subsystem.
 * @param event_id Event identifier within the selected event base.
 * @param event_data Event-specific payload supplied by ESP-IDF.
 */
static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        
        // Start the initial station connection after the Wi-Fi driver is ready.
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        ESP_LOGW(TAG, "Wi-Fi disconnected, reconnecting...");
        // Reconnection is intentionally requested without rebuilding the
        // complete network stack.
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        
        // UDP tasks may start network operations only after IPv4 acquisition.
        wifi_connected = true;
        ESP_LOGI(TAG, "Wi-Fi connected. IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

/**
 * @brief Initializes the ESP-IDF Wi-Fi stack in station mode.
 *
 * The function initializes NVS, creates the default network interface and
 * event loop, registers Wi-Fi and IP handlers, applies the configured station
 * credentials, and starts the Wi-Fi driver.
 *
 * Initialization failures are treated as fatal through ESP_ERROR_CHECK(),
 * except when the default event loop already exists.
 */
static void wifi_init_sta()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Recreate NVS when the stored partition is full or incompatible with
        // the current ESP-IDF version.
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(ret);
    }

    ESP_ERROR_CHECK(esp_netif_init());

    // ESP_ERR_INVALID_STATE is accepted because another subsystem may have
    // already created the process-wide default event loop.
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(ret);
    }

    // Create the default station interface, including its DHCP client.
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // Register a shared handler for station lifecycle and IPv4 events.
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    // Populate a zero-initialized station configuration using the deployment
    // credentials defined at compile time.
    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA started. SSID: %s", WIFI_SSID);
}

/**
 * @brief Performs lightweight validation of an incoming presence-event payload.
 *
 * The current implementation does not parse JSON structurally. Instead, it
 * accepts only payloads containing the expected event type, PRESENT state, and
 * Presence Node source identifier.
 *
 * @param payload Null-terminated UDP payload.
 *
 * @return true when all required contract fragments are present.
 */
static bool is_presence_event_payload(const char *payload)
{
    if (payload == NULL) {
        return false;
    }

    // This substring filter is intentionally narrow for the validated V1
    // contract, but it is not a general-purpose JSON parser.
    return strstr(payload, "\"type\":\"presence_event\"") != NULL &&
           strstr(payload, "\"state\":\"PRESENT\"") != NULL &&
           strstr(payload, "\"source\":\"presence_node_v1\"") != NULL;
}

/**
 * @brief Receives Presence Node events over UDP and forwards them to the UI.
 *
 * The task waits for IPv4 connectivity, binds a datagram socket to
 * UDP_LISTEN_PORT on all local interfaces, and blocks while receiving packets.
 *
 * Valid PRESENT events are translated into EVENT_PRESENCE_RECEIVED messages
 * and published through the Identity Event Queue. The task does not access the
 * display directly.
 *
 * @param param Unused FreeRTOS task argument.
 */
static void udp_listener_task(void *param)
{
    (void)param;

    ESP_LOGI(TAG, "UDP listener task started");

    // Delay socket creation until the station has obtained an IPv4 address.
    while (!wifi_connected) {
        ESP_LOGI(TAG, "UDP listener waiting for Wi-Fi...");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Wi-Fi ready; creating UDP listener");

    // One byte is reserved for the null terminator added after recvfrom().
    char rx_buffer[UDP_RX_BUFFER_SIZE] = {0};

    // Create a blocking IPv4 datagram socket for presence-event reception.
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0) {
        ESP_LOGE(
            TAG,
            "Unable to create UDP listener socket, errno=%d",
            errno
        );
        vTaskDelete(NULL);
        return;
    }

    // Allow the listener port to be rebound promptly after a task or firmware
    // restart.
    int reuse_addr = 1;

    if (setsockopt(
            sock,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_addr,
            sizeof(reuse_addr)) < 0) {
        ESP_LOGW(
            TAG,
            "SO_REUSEADDR failed, errno=%d",
            errno
        );
    }

    // Listen on every local IPv4 interface using the Presence Node contract
    // port.
    struct sockaddr_in listen_addr = {};
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(UDP_LISTEN_PORT);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(
        sock,
        reinterpret_cast<struct sockaddr *>(&listen_addr),
        sizeof(listen_addr)
    );

    if (ret < 0) {
        ESP_LOGE(
            TAG,
            "UDP bind failed on 0.0.0.0:%d, errno=%d",
            UDP_LISTEN_PORT,
            errno
        );

        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(
        TAG,
        "UDP listener ready on 0.0.0.0:%d",
        UDP_LISTEN_PORT
    );

    while (true) {
        struct sockaddr_in source_addr = {};
        socklen_t source_addr_len = sizeof(source_addr);

        // Clear stale data so logging and string filtering see only the newly
        // received datagram.
        memset(rx_buffer, 0, sizeof(rx_buffer));

        ESP_LOGI(TAG, "UDP listener waiting for packet...");

        // Block until a datagram arrives; this task does not require periodic
        // wake-ups while the socket is healthy.
        int len = recvfrom(
            sock,
            rx_buffer,
            sizeof(rx_buffer) - 1,
            0,
            reinterpret_cast<struct sockaddr *>(&source_addr),
            &source_addr_len
        );

        if (len < 0) {
            ESP_LOGW(
                TAG,
                "UDP recvfrom failed, errno=%d",
                errno
            );

            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // recvfrom() returns raw bytes, so terminate the bounded payload before
        // using string functions or log formatting.
        rx_buffer[len] = '\0';

        char source_ip[INET_ADDRSTRLEN] = {0};

        inet_ntop(
            AF_INET,
            &source_addr.sin_addr,
            source_ip,
            sizeof(source_ip)
        );

        ESP_LOGI(
            TAG,
            "UDP RX: %d bytes from %s:%u",
            len,
            source_ip,
            static_cast<unsigned>(ntohs(source_addr.sin_port))
        );

        ESP_LOGI(TAG, "UDP payload: %s", rx_buffer);

        if (is_presence_event_payload(rx_buffer)) {
            ESP_LOGI(TAG, "Valid presence_event received");

            // Convert the network contract into an internal event consumed by
            // the UI task.
            send_simple_event(
                EVENT_PRESENCE_RECEIVED,
                "Presence detected"
            );
        } else {
            ESP_LOGW(
                TAG,
                "UDP payload received but rejected by presence filter"
            );
        }
    }
}
// -----------------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------------

/**
 * @brief Renders the default Identity Node console.
 *
 * The console presents the currently selected profile, active operational
 * context, and NFC readiness state.
 *
 * This function performs a full-screen redraw and must therefore be called only
 * by the UI task, which owns all display operations.
 */
static void draw_console()
{
    // Rebuild the complete console from the current UI-owned runtime state.
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextDatum(middle_center);

    // current_profile is maintained as a validated index into the profile table.
    const Profile &profile = profiles[current_profile];

    const bool has_active_profile =
        strcmp(profile.id, "unknown") != 0;

    // -------------------------------------------------------------------------
    // Header
    // -------------------------------------------------------------------------

    M5.Display.setTextSize(2);
    M5.Display.drawString("AMBIENT AI", 120, 24);

    M5.Display.drawLine(52, 43, 188, 43, DARKGREY);

    // -------------------------------------------------------------------------
    // Active profile
    // -------------------------------------------------------------------------

    // The image manager provides its own unknown-profile fallback.
    ProfileImageManager::drawProfile(
        profile.id,
        120,
        85
    );

    if (has_active_profile) {
        M5.Display.setTextSize(2);
        M5.Display.drawString(profile.name, 120, 126);

        M5.Display.setTextSize(1);
        M5.Display.drawString(profile.role, 120, 148);
    } else {
        M5.Display.setTextSize(1);
        M5.Display.drawString("Waiting for identity", 120, 130);
    }

    // -------------------------------------------------------------------------
    // Active context
    // -------------------------------------------------------------------------

    M5.Display.setTextSize(1);
    M5.Display.drawString("ACTIVE CONTEXT", 120, 174);

    M5.Display.setTextSize(2);
    M5.Display.drawString(
        contexts[current_context].display_name,
        120,
        198
    );

    // -------------------------------------------------------------------------
    // Node state
    // -------------------------------------------------------------------------

    M5.Display.setTextSize(1);

    // nfc_detected reflects controller availability, not card presence.
    if (nfc_detected) {
        M5.Display.drawString(
            "IDENTITY NODE READY",
            120,
            224
        );
    } else {
        M5.Display.drawString(
            "NFC INITIALIZING",
            120,
            224
        );
    }

    // Restore the default datum expected by other display routines.
    M5.Display.setTextDatum(top_left);
}

/**
 * @brief Renders the temporary prompt requesting NFC authentication.
 *
 * This screen is shown after a validated presence event is received and remains
 * active until its timeout expires or another identity flow replaces it.
 *
 * Display access remains owned by the UI task.
 */
static void draw_presence_prompt()
{
    // Replace the normal console with a focused authentication instruction.
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextDatum(middle_center);

    M5.Display.setTextSize(2);
    M5.Display.drawString("Presence detected", 120, 105);

    M5.Display.setTextSize(2);
    M5.Display.drawString("Tap NFC card", 120, 145);

    // Restore the default datum after centered prompt rendering.
    M5.Display.setTextDatum(top_left);
}

/**
 * @brief Activates and renders the timed presence prompt.
 *
 * Starting the presence prompt cancels any active identity visualization so
 * that the authentication request becomes the current UI state.
 */
static void show_presence_prompt()
{
    presence_prompt_active = true;
    
    // Store an absolute FreeRTOS tick deadline for non-blocking expiration.
    presence_prompt_until = xTaskGetTickCount() + PRESENCE_PROMPT_DURATION;
    
    // Presence prompting takes precedence over a previous identity result.
    identity_visual_active = false;

    // Provide brief local feedback without blocking the UI task.
    draw_presence_prompt();
    M5.Speaker.tone(2200, 80);
}

/**
 * @brief Expires the presence prompt and restores the default console.
 *
 * The function is called periodically by the UI task and performs no blocking
 * delay.
 */
static void update_presence_prompt_timeout()
{
    // Avoid tick calculations when no presence prompt is active.
    if (!presence_prompt_active) {
        return;
    }

    TickType_t now = xTaskGetTickCount();

    // Signed tick subtraction preserves normal behavior across counter wrap.
    if (((int32_t)(presence_prompt_until - now)) <= 0) {
        presence_prompt_active = false;
        
        // Return to the console using the latest profile and context state.
        draw_console();
    }
}


/**
 * @brief Renders a temporary identity-confirmation screen.
 *
 * The screen presents the resolved profile image, name, role, active context,
 * and the NFC UID associated with the authentication event.
 *
 * Invalid profile indexes are normalized to the unknown-profile fallback before
 * accessing the profile table.
 *
 * @param profile_index Index into the local profile table.
 * @param uid Null-terminated hexadecimal UID string, or null when unavailable.
 */
static void draw_identity_visualization(int profile_index, const char *uid)
{
    // Enforce the unknown-profile fallback before indexing the table.
    if (profile_index < 0 || profile_index >= PROFILE_COUNT) {
        profile_index = 0;
    }

    const Profile &profile = profiles[profile_index];

    // Build a standalone confirmation view rather than overlaying the console.
    M5.Display.fillScreen(BLACK);

    M5.Display.setTextDatum(middle_center);

    M5.Display.setTextSize(2);
    M5.Display.drawString("IDENTITY", 120, 22);

    M5.Display.drawRoundRect(15, 45, 210, 185, 14, WHITE);

    ProfileImageManager::drawProfile(
        profile.id,
        120,
        82
    );

    M5.Display.setTextSize(2);
    M5.Display.drawString(profile.name, 120, 125);

    M5.Display.setTextSize(1);
    M5.Display.drawString(profile.role, 120, 150);

    // Format the current UI-selected context into a bounded display buffer.
    char context_line[48] = {0};
        snprintf(
        context_line,
        sizeof(context_line),
        "Context: %s",
        contexts[current_context].display_name
    );
    M5.Display.drawString(context_line, 120, 178);

    // Render a deterministic fallback when no usable UID was supplied.
    char uid_line[48] = {0};
    if (uid != NULL && uid[0] != '\0') {
        
        // The current WS1850S implementation returns four-byte UIDs, represented
        // as eight hexadecimal characters.
        snprintf(uid_line, sizeof(uid_line), "UID: %.8s", uid);
    } else {
        snprintf(uid_line, sizeof(uid_line), "UID: none");
    }
    M5.Display.drawString(uid_line, 120, 202);

    // Restore the default alignment expected by subsequent UI rendering.
    M5.Display.setTextDatum(top_left);
}

/**
 * @brief Activates the timed identity-confirmation screen.
 *
 * @param profile_index Resolved profile index to display.
 * @param uid Null-terminated UID associated with the authentication event.
 */
static void show_identity_visualization(int profile_index, const char *uid)
{
    identity_visual_active = true;
    
    // Use an absolute tick deadline so the UI task remains non-blocking.
    identity_visual_until = xTaskGetTickCount() + IDENTITY_VISUAL_DURATION;
    draw_identity_visualization(profile_index, uid);
}

/**
 * @brief Expires the identity-confirmation view and restores the console.
 *
 * The function is evaluated periodically by the UI task and does not block.
 */
static void update_identity_visualization_timeout()
{
    // Skip deadline evaluation when the confirmation view is inactive.
    if (!identity_visual_active) {
        return;
    }

    TickType_t now = xTaskGetTickCount();

    // Signed tick subtraction preserves normal behavior across counter wrap.
    if (((int32_t)(identity_visual_until - now)) <= 0) {
        identity_visual_active = false;
        // Rebuild the console from the latest application state.
        draw_console();
    }
}

/**
 * @brief Sends a serialized Identity Package to the Cognitive Runtime.
 *
 * A short-lived UDP socket is created for each transmission and closed after
 * sendto() completes. Transmission is skipped when the payload is empty or the
 * station has not obtained network connectivity.
 *
 * @param json_payload Null-terminated serialized Identity Package.
 */
static void send_identity_package_udp(const char *json_payload)
{
    // Reject invalid application payloads before allocating a socket.
    if (json_payload == NULL || json_payload[0] == '\0') {
        ESP_LOGW(TAG, "Identity UDP TX skipped: empty payload");
        return;
    }

    // The current transport is best-effort and does not queue packages while
    // the network is unavailable.
    if (!wifi_connected) {
        ESP_LOGW(TAG, "Identity UDP TX skipped: Wi-Fi not connected");
        return;
    }

    // Use a transient socket because identity events are infrequent and no
    // persistent transport session is required.
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Identity UDP TX socket creation failed");
        return;
    }

    // Address the fixed Cognitive Runtime endpoint defined by the deployment
    // network configuration.
    struct sockaddr_in dest_addr = {};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(AX630C_UDP_PORT);
    inet_pton(AF_INET, AX630C_IP, &dest_addr.sin_addr);

    // UDP delivery is best-effort; success here confirms only that the local
    // stack accepted the datagram for transmission.
    int sent = sendto(
        sock,
        json_payload,
        strlen(json_payload),
        0,
        (struct sockaddr *)&dest_addr,
        sizeof(dest_addr)
    );

    if (sent < 0) {
        ESP_LOGE(TAG, "Identity UDP TX failed");
    } else {
        ESP_LOGI(TAG, "Identity Package sent to AX630C %s:%d",
                 AX630C_IP,
                 AX630C_UDP_PORT);
    }

    close(sock);
}

/**
 * @brief Builds and transmits the current Identity Package.
 *
 * The package combines the UI-owned profile and context selections with the
 * latest NFC runtime state. When no card is currently considered present, the
 * serialized UID field is intentionally emitted as an empty string.
 *
 * The resulting JSON contract is logged locally and then sent to the AX630C
 * Cognitive Runtime over UDP.
 */
static void generate_identity_package()
{
    // Do not propagate a stale UID after the NFC state marks the card absent.
    const char *package_uid = nfc_card_present ? last_nfc_uid : "";

    // The fixed buffer bounds serialization and keeps package generation
    // deterministic without dynamic allocation.
    char identity_json[512] = {0};

    // context and current_context intentionally carry the same value to remain
    // compatible with the validated Identity Package contract.
    snprintf(identity_json,
             sizeof(identity_json),
             "{\"type\":\"identity_package\","
             "\"contract_version\":\"1.1\","
             "\"profile\":{\"id\":\"%s\",\"name\":\"%s\",\"role\":\"%s\"},"
             "\"context\":\"%s\","
             "\"current_context\":\"%s\","
             "\"nfc\":{\"detected\":%s,\"card_present\":%s,\"uid\":\"%s\"},"
             "\"source\":\"m5dial_identity_console_v1\"}",
             profiles[current_profile].id,
             profiles[current_profile].name,
             profiles[current_profile].role,
             contexts[current_context].protocol_name,
             contexts[current_context].protocol_name,
             nfc_detected ? "true" : "false",
             nfc_card_present ? "true" : "false",
             package_uid);

    // Keep the exact outbound contract visible in serial logs for integration
    // validation and reproducibility.         
    ESP_LOGI(TAG, "%s", identity_json);

    send_identity_package_udp(identity_json);
}
// -----------------------------------------------------------------------------
// NFC Setup / Reset
// -----------------------------------------------------------------------------

/**
 * @brief Releases NFC device and bus resources without acquiring the I2C mutex.
 *
 * This function is intended for callers that already hold exclusive access to
 * the shared I2C bus or are executing within another unlocked NFC setup path.
 *
 * Device removal is performed before deleting the master bus because the
 * device handle depends on the bus lifetime.
 */
static void reset_nfc_runtime_unlocked()
{
    // Remove the logical WS1850S device before deleting its owning bus.
    if (nfc_dev_handle != NULL) {
        i2c_master_bus_rm_device(nfc_dev_handle);
        
        // Prevent later code from using a handle whose resource was released.
        nfc_dev_handle = NULL;
    }

    if (nfc_bus_handle != NULL) {
        i2c_del_master_bus(nfc_bus_handle);
        
        // Clear the runtime reference after the bus has been deleted.
        nfc_bus_handle = NULL;
    }
    // Reflect that no initialized NFC controller is currently available.
    nfc_detected = false;
}

/**
 * @brief Rebuilds and initializes the WS1850S NFC runtime without locking.
 *
 * The function clears any previous NFC resources, creates a new ESP-IDF I2C
 * master bus, attaches the WS1850S device, probes the controller, reads its
 * version register, and applies the driver initialization sequence.
 *
 * Callers must ensure exclusive access to the shared I2C bus.
 *
 * @return true when the bus, device, probe, and initialization stages succeed.
 */
static bool setup_nfc_unlocked()
{
    // Begin from a known state so partial resources from an earlier attempt do
    // not survive into the new initialization sequence.
    reset_nfc_runtime_unlocked();

    // Reuse the M5Dial board-level I2C pins so the WS1850S shares the same
    // physical bus as the internal touch-related peripherals.
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = BSP_I2C_SDA;
    bus_cfg.scl_io_num = BSP_I2C_SCL;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    
    // Apply the ESP-IDF glitch filter used by the validated M5Dial baseline.
    bus_cfg.glitch_ignore_cnt = 7;
    
    // Enable internal pull-ups as a supplemental safeguard for the shared bus.
    bus_cfg.flags.enable_internal_pullup = true;

    // Create a fresh ESP-IDF master-bus instance owned by the NFC runtime.
    esp_err_t ret = i2c_new_master_bus(&bus_cfg, &nfc_bus_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC I2C bus init failed: %s", esp_err_to_name(ret));
        return false;
    }

    // Configure the WS1850S as a 7-bit I2C device at the validated 100 kHz bus
    // speed.
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = WS1850S_I2C_ADDRESS;
    dev_cfg.scl_speed_hz = 100000;

    // Attach the controller to the newly created bus and retain its device
    // handle for all subsequent WS1850S operations.
    ret = i2c_master_bus_add_device(nfc_bus_handle, &dev_cfg, &nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC add device failed: %s", esp_err_to_name(ret));
        
        // Release the bus created earlier because device attachment did not
        // complete successfully.
        reset_nfc_runtime_unlocked();
        return false;
    }

    // Verify that the expected controller responds before issuing register-level
    // initialization commands.
    ret = ws1850s_probe(nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S probe failed: %s", esp_err_to_name(ret));
        reset_nfc_runtime_unlocked();
        return false;
    }

    // Read the controller version for diagnostics; failure here is logged but
    // does not independently abort initialization.
    uint8_t version = 0;
    ret = ws1850s_read_version(nfc_dev_handle, &version);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NFC/WS1850S VersionReg: 0x%02X", version);
    } else {
        ESP_LOGW(TAG, "NFC/WS1850S VersionReg read failed: %s", esp_err_to_name(ret));
    }

    // Apply the validated WS1850S register configuration required for polling
    // and UID acquisition.
    ret = ws1850s_init(nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S init failed: %s", esp_err_to_name(ret));
        reset_nfc_runtime_unlocked();
        return false;
    }

    ESP_LOGI(TAG, "NFC/WS1850S initialized at 0x28");

    // Publish controller availability only after every required setup stage
    // has completed successfully.
    nfc_detected = true;
    return true;
}

/**
 * @brief Initializes the NFC runtime while holding the shared I2C mutex.
 *
 * @return true when the mutex is acquired and setup_nfc_unlocked() succeeds.
 */
static bool setup_nfc_locked()
{
    bool ok = false;

    // Initialization receives a longer lock timeout than normal polling because
    // it may recreate the complete I2C bus and device runtime.
    if (lock_i2c(pdMS_TO_TICKS(800))) {
        
        // Keep the mutex across the complete rebuild so no other task accesses
        // partially initialized I2C resources.
        ok = setup_nfc_unlocked();
        unlock_i2c();
    } else {
        ESP_LOGW(TAG, "NFC setup skipped: I2C lock timeout");
    }

    return ok;
}

/**
 * @brief Releases NFC resources while holding the shared I2C mutex.
 *
 * The wrapper is used by recovery paths that do not already own the bus lock.
 */
static void reset_nfc_runtime_locked()
{
    if (lock_i2c(pdMS_TO_TICKS(800))) {
        
        // Prevent concurrent M5Dial or NFC transactions while handles and bus
        // resources are being destroyed.
        reset_nfc_runtime_unlocked();
        unlock_i2c();
    } else {
        ESP_LOGW(TAG, "NFC reset skipped: I2C lock timeout");
    }
}

/**
 * @brief Acquires an NFC UID and converts it to uppercase hexadecimal text.
 *
 * Each attempt independently acquires the shared I2C mutex, performs a fresh
 * card-presence request, optionally reinitializes the controller during the
 * fourth attempt, and then executes UID anti-collision through the WS1850S
 * driver.
 *
 * Transient RF misses and expected acquisition errors are retried locally.
 * Persistent bus or driver errors are returned to the NFC state machine for
 * staged recovery.
 *
 * @param uid_text Destination buffer for the null-terminated hexadecimal UID.
 * @param uid_text_size Capacity of uid_text in bytes.
 *
 * @return ESP_OK on successful UID conversion; ESP_ERR_NOT_FOUND when the card
 *         cannot be acquired after all attempts; another ESP-IDF error for
 *         non-transient failures.
 */
static esp_err_t read_uid_text_locked(char *uid_text, size_t uid_text_size)
{
    // Reject unusable output buffers and calls made before NFC initialization.
    if (uid_text == NULL || uid_text_size == 0 || nfc_dev_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Preserve the most recent acquisition result across bounded retries.
    esp_err_t last_ret = ESP_FAIL;

    for (int attempt = 1; attempt <= NFC_UID_READ_ATTEMPTS; attempt++) {
        
        // Acquire the bus independently for each attempt so retry delays do not
        // block touch and other M5Dial I2C activity.
        if (!lock_i2c(pdMS_TO_TICKS(500))) {
            last_ret = ESP_ERR_TIMEOUT;
        } else {
            // Keep binary UID data local to the current acquisition attempt.
            uint8_t uid[10] = {0};
            uint8_t uid_len = 0;
            bool present = false;

            // Each acquisition attempt starts with a fresh REQA.
            // This is more reliable than doing one REQA in NFC_POLL and then
            // retrying anticollision several times without re-arming the card.
            last_ret = ws1850s_card_present(nfc_dev_handle, &present);

            if (last_ret == ESP_OK && present) {
                // Allow the card and RF frontend to settle briefly before
                // anti-collision and UID retrieval.
                vTaskDelay(pdMS_TO_TICKS(8));

                // Controlled re-arm during acquisition only.
                // Avoid doing this on every normal poll.
                
                // Use one bounded mid-sequence reinitialization to recover from
                // a transient controller state without rebuilding the bus.
                if (attempt == 4 && nfc_dev_handle != NULL) {
                    ws1850s_init(nfc_dev_handle);
                    vTaskDelay(pdMS_TO_TICKS(25));
                    
                    // Reissue REQA after initialization before attempting UID
                    // acquisition again.
                    ws1850s_card_present(nfc_dev_handle, &present);
                    vTaskDelay(pdMS_TO_TICKS(8));
                }

                last_ret = ws1850s_read_uid(nfc_dev_handle, uid, &uid_len);
            } else if (last_ret == ESP_OK && !present) {
                
                // Normalize a successful REQA transaction with no detected card
                // into the expected absence result.
                last_ret = ESP_ERR_NOT_FOUND;
            }

            // Release the shared bus before string formatting and retry logging.
            unlock_i2c();

            if (last_ret == ESP_OK) {
                // Clear the destination so partial UID text cannot survive from
                // an earlier failed attempt.
                memset(uid_text, 0, uid_text_size);

                // Convert each UID byte into two uppercase hexadecimal
                // characters while preserving space for the null terminator.
                for (int i = 0; i < uid_len && (i * 2 + 1) < (int)uid_text_size; i++) {
                    snprintf(uid_text + (i * 2),
                             uid_text_size - (i * 2),
                             "%02X",
                             uid[i]);
                }

                if (attempt > 1) {
                    ESP_LOGI(TAG, "NFC UID acquired after %d attempts", attempt);
                }

                // Stop retrying immediately after a complete UID conversion.
                return ESP_OK;
            }
        }

        // Acquisition misses: retry quietly. These are common while the card is
        // entering/leaving the RF field or near the edge of the antenna.
        
        // Treat RF absence, malformed transient responses, CRC misses, generic
        // acquisition failures, and lock timeouts as bounded retry conditions.
        if (last_ret == ESP_ERR_NOT_FOUND ||
            last_ret == ESP_ERR_INVALID_SIZE ||
            last_ret == ESP_ERR_INVALID_CRC ||
            last_ret == ESP_FAIL ||
            last_ret == ESP_ERR_TIMEOUT) {
            vTaskDelay(NFC_UID_RETRY_DELAY);
            continue;
        }

        // Real I2C/driver error: let the state machine recovery handle it.
        ESP_LOGW(TAG, "NFC UID read failed: %s", esp_err_to_name(last_ret));
        return last_ret;
    }

    // Exhausting every bounded attempt is reported as card-not-acquired rather
    // than as a critical controller failure.
    return ESP_ERR_NOT_FOUND;
}

// -----------------------------------------------------------------------------
// NFC Task
// -----------------------------------------------------------------------------

/**
 * @brief Runs the NFC polling and recovery state machine.
 *
 * This task owns the operational lifecycle of the WS1850S controller:
 *
 * - delayed startup and controller initialization;
 * - card-presence polling;
 * - bounded UID acquisition;
 * - stable card-presence tracking;
 * - card-removal confirmation;
 * - staged recovery after controller, driver, or bus failures;
 * - controlled cooldown periods between recovery attempts.
 *
 * The task never updates the display directly. NFC results and runtime status
 * changes are published to the UI task through the Identity Event Queue.
 *
 * Shared I2C access is acquired only around bounded WS1850S transactions or
 * complete setup/reset operations.
 *
 * @param param Unused FreeRTOS task argument.
 */
static void nfc_task(void *param)
{
    (void)param;

    // Allow the board, UI runtime, and shared I2C peripherals to stabilize
    // before creating the NFC controller runtime.
    vTaskDelay(NFC_BOOT_DELAY);

    // Every task start enters the same controlled initialization path.
    NfcState state = NFC_INIT;

    // raw_present stores the most recent hardware poll result.
    // stable_present represents the debounced application-level card state.
    bool raw_present = false;
    bool stable_present = false;

    // Track removal confirmation, recovery escalation, and the most recent
    // non-transient NFC error.
    int removed_count = 0;
    int consecutive_errors = 0;
    esp_err_t last_nfc_error = ESP_OK;

    // The recovery path selects the next delay according to error severity.
    TickType_t cooldown_time = NFC_SHORT_COOLDOWN;

    // The task is permanent; all operational behavior is represented by state
    // transitions rather than by task termination.
    while (true) {
        switch (state) {
            case NFC_INIT: {
                // Build the complete NFC bus and controller runtime from a
                // known state.
                ESP_LOGI(TAG, "NFC state: INIT");

                if (setup_nfc_locked()) {
                    
                    // A complete initialization clears the previous recovery
                    // history and exposes controller readiness to the UI.
                    consecutive_errors = 0;
                    send_simple_event(EVENT_NFC_RECOVERED, "NFC ready");
                    state = NFC_IDLE;
                } else {

                    // Initialization failure uses the longest cooldown before
                    // another complete setup attempt.
                    consecutive_errors++;
                    cooldown_time = NFC_LONG_COOLDOWN;
                    send_simple_event(EVENT_NFC_ERROR, "NFC init failed");
                    state = NFC_COOLDOWN;
                }

                break;
            }

            case NFC_IDLE: {

                // Yield between polls and respect temporary UI ownership of the
                // shared I2C bus.
                if (is_ui_quiet_window_active()) {
                    // Remain in NFC_IDLE while touch or encoder activity is
                    // expected to use M5Dial internal I2C peripherals.
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;
                }

                // Apply the normal inter-poll interval before requesting card
                // presence again.
                vTaskDelay(NFC_IDLE_DELAY);
                state = NFC_POLL;
                break;
            }

            case NFC_POLL: {
                // Perform one bounded card-presence transaction and translate
                // the raw result into the next state-machine transition.
                if (is_ui_quiet_window_active()) {
                    
                    // UI activity may have started after NFC_IDLE completed, so
                    // recheck immediately before accessing the controller.
                    state = NFC_IDLE;
                    break;
                }

                if (nfc_dev_handle == NULL) {
                    
                    // A missing device handle requires a complete runtime
                    // reconstruction rather than a normal polling retry.
                    state = NFC_INIT;
                    break;
                }

                // Clear the previous hardware result before starting a new
                // presence transaction.
                raw_present = false;
                esp_err_t ret = ESP_FAIL;

                // Hold the shared bus only for the single WS1850S presence
                // transaction.
                if (lock_i2c(pdMS_TO_TICKS(200))) {
                    ret = ws1850s_card_present(nfc_dev_handle, &raw_present);
                    unlock_i2c();
                } else {

                    // Represent mutex acquisition failure through the same
                    // error channel used by controller operations.
                    ret = ESP_ERR_TIMEOUT;
                }

                if (ret != ESP_OK) {
                    // Preserve the failure classification for staged recovery.
                    last_nfc_error = ret;
                    ESP_LOGW(TAG, "NFC polling failed: %s", esp_err_to_name(ret));
                    state = NFC_ERROR;
                    break;
                }

                // Any successful presence transaction breaks the sequence of
                // consecutive controller or bus errors.
                consecutive_errors = 0;

                if (raw_present) {
                    
                    // A positive poll cancels any pending removal confirmation.
                    removed_count = 0;

                    // A newly detected card requires UID acquisition; an already
                    // accepted card only requires continued monitoring.
                    if (!stable_present) {
                        state = NFC_READ_UID;
                    } else {
                        state = NFC_CARD_PRESENT;
                    }
                } else {

                    // A negative raw poll starts removal confirmation only when
                    // the application previously considered a card stable.
                    if (stable_present) {
                        state = NFC_CARD_REMOVED;
                    } else {
                        state = NFC_IDLE;
                    }
                }

                break;
            }

            case NFC_READ_UID: {
                // Acquire and map the UID of a card that was newly detected by
                // the polling state.
                // Allow the RF interaction to settle before the bounded
                // anti-collision sequence begins.
                vTaskDelay(NFC_READ_DELAY);

                // Store the hexadecimal UID locally before publishing it across
                // the task boundary.
                char uid_text[32] = {0};
                esp_err_t ret = read_uid_text_locked(uid_text, sizeof(uid_text));

                if (ret == ESP_OK) {
                    
                    // A complete UID read promotes the card to the stable
                    // application-level presence state.
                    stable_present = true;
                    removed_count = 0;
                    consecutive_errors = 0;

                    // Publish the resolved UID and local profile mapping as one
                    // atomic queue message to the UI task.
                    IdentityEvent event = {};
                    event.type = EVENT_NFC_UID_READ;
                    strncpy(event.uid, uid_text, sizeof(event.uid) - 1);
                    event.uid[sizeof(event.uid) - 1] = '\0';
                    event.profile_index = find_profile_by_uid(event.uid);

                    send_identity_event(event);

                    // Avoid rereading the same UID while the card remains in the
                    // RF field.
                    state = NFC_CARD_PRESENT;
                } else if (ret == ESP_ERR_NOT_FOUND) {
                    // Card was present during poll but disappeared before UID read.
                    // This is a normal removal/absence condition, not a critical NFC error.
                    stable_present = false;
                    removed_count = 0;
                    state = NFC_CARD_REMOVED;
                } else {

                    // Non-absence failures are delegated to the staged recovery
                    // state.
                    last_nfc_error = ret;
                    state = NFC_ERROR;
                }

                break;
            }

            case NFC_CARD_PRESENT: {
                
                // Throttle repeated presence checks while the accepted card
                // remains within the RF field.
                vTaskDelay(NFC_POLL_DELAY);
                
                // Continue monitoring for removal without reacquiring the UID.
                state = NFC_POLL;
                break;
            }

            case NFC_CARD_REMOVED: {
                // Confirm removal across multiple negative polls before
                // notifying the application.
                
                // Count consecutive observations in which a previously stable
                // card is no longer detected.
                removed_count++;

                if (removed_count >= CARD_REMOVED_CONFIRM_COUNT) {
                    
                    // Commit the removal only after the configured debounce
                    // threshold has been reached.
                    stable_present = false;
                    removed_count = 0;

                    send_simple_event(EVENT_NFC_CARD_REMOVED, "Card removed");
                }

                // Return through NFC_IDLE so the normal polling interval and UI
                // quiet-window rules remain in effect.
                state = NFC_IDLE;
                break;
            }

            case NFC_ERROR: {
                // Escalate recovery according to the number and classification
                // of consecutive NFC failures.
                
                // A successful polling transaction resets this counter before
                // the state machine can return here.
                consecutive_errors++;

                ESP_LOGW(TAG, "NFC error count: %d", consecutive_errors);

                // Level 1 keeps the existing runtime and applies either a bus
                // backoff or a bounded controller reinitialization.
                if (consecutive_errors == 1) {
                    if (last_nfc_error == ESP_ERR_INVALID_STATE || last_nfc_error == ESP_ERR_TIMEOUT) {
                        ESP_LOGW(TAG, "NFC recovery level 1: bus backoff only");
                        
                        // Avoid immediate controller commands when the failure
                        // indicates bus contention or invalid runtime state.
                        cooldown_time = NFC_MEDIUM_COOLDOWN;
                    } else {
                        ESP_LOGW(TAG, "NFC recovery level 1: soft reinit");

                    // Reapply the controller register configuration without
                    // destroying the I2C bus or device handle.
                        if (nfc_dev_handle != NULL && lock_i2c(pdMS_TO_TICKS(300))) {
                            ws1850s_init(nfc_dev_handle);
                            unlock_i2c();
                        }


                    // A local controller reinitialization requires only the
                    // shortest recovery delay.
                        cooldown_time = NFC_SHORT_COOLDOWN;
                    }
                } else if (consecutive_errors == 2) {

                    // Level 2 preserves the runtime but exposes degraded NFC status
                    // and pauses polling for a longer interval.
                    ESP_LOGW(TAG, "NFC recovery level 2: pause polling");

                    // Inform the UI only after recovery has escalated beyond the
                    // first transient failure.
                    send_simple_event(EVENT_NFC_ERROR, "NFC polling paused");

                    cooldown_time = NFC_MEDIUM_COOLDOWN;
                } else {

                    // Level 3 discards the existing runtime and forces a complete
                    // bus and controller reconstruction.
                    ESP_LOGW(TAG, "NFC recovery level 3: controlled rebuild");

                    send_simple_event(EVENT_NFC_ERROR, "NFC rebuild");

                    reset_nfc_runtime_locked();


                    // Clear every card-related observation because the
                    // underlying controller runtime no longer exists.
                    stable_present = false;
                    raw_present = false;
                    removed_count = 0;

                    // The rebuilt runtime starts a new recovery history.
                    consecutive_errors = 0;

                    // Delay the next initialization attempt after destroying
                    // and recreating shared bus resources.
                    cooldown_time = NFC_LONG_COOLDOWN;
                    
                    // Recovery levels 1 and 2 preserve the current handles and
                    // resume through the common cooldown state.
                    state = NFC_COOLDOWN;
                    break;
                }

                state = NFC_COOLDOWN;
                break;
            }

            // Suspend NFC activity for the delay selected by the preceding
            // initialization or recovery path.
            case NFC_COOLDOWN: {
                vTaskDelay(cooldown_time);

                // Rebuild missing or unavailable runtimes; otherwise resume
                // normal polling with the existing controller instance.
                if (nfc_dev_handle == NULL || !nfc_detected) {
                    state = NFC_INIT;
                } else {
                    state = NFC_IDLE;
                }

                break;
            }

            // Recover defensively from an invalid or corrupted state value.
            default: {
                state = NFC_INIT;
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// UI Task
// -----------------------------------------------------------------------------

/**
 * @brief Owns the M5Dial user interface and application-facing identity state.
 *
 * This task is the only runtime component that calls M5.update(), renders the
 * display, reads touch input, handles encoder-based context selection, controls
 * the speaker, and consumes IdentityEvent messages.
 *
 * NFC and network tasks communicate with the UI exclusively through the
 * Identity Event Queue. Short M5Dial update and touch transactions are
 * protected by the shared I2C mutex because the WS1850S controller uses the
 * same physical bus.
 *
 * Temporary screens follow this precedence:
 *
 * presence prompt > identity confirmation > default console
 *
 * @param param Unused FreeRTOS task argument.
 */
static void ui_task(void *param)
{
    (void)param;

    // Initialize all M5Dial services before any display, touch, or speaker use.
    auto cfg = M5.config();
    M5.begin(cfg);

    // Prepare the embedded profile-image provider used by every UI state.
    ProfileImageManager::init();

    ESP_LOGI(TAG, "Identity Console V1");
    ESP_LOGI(TAG, "FreeRTOS runtime stabilized baseline V6 + identity visualization");

    // Bind the context selector to the M5Dial encoder pins defined by the BSP.
    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = BSP_ENCODER_A,
        .gpio_encoder_b = BSP_ENCODER_B,
        .enable_power_save = false,
    };

    knob_handle_t knob = iot_knob_create(&knob_cfg);


    // Encoder initialization failure does not prevent the remaining UI, NFC,
    // touch, and network functions from continuing.
    if (knob == NULL) {
        ESP_LOGE(TAG, "Failed to create knob encoder");
    } else {
        ESP_LOGI(TAG, "Knob encoder initialized");
    }

    // Present the neutral console before asynchronous NFC initialization
    // completes.
    draw_console();
    M5.Speaker.tone(2000, 150);

    // Retain previous input samples so the loop reacts to changes and touch
    // edges rather than continuously repeating the same action.
    int last_encoder_count = 0;
    int last_touch_state = 0;

    while (true) {
        int touch_state = 0;

        // M5.update may touch internal I2C devices on M5Dial.
        // Protect only this short transaction window, not the full UI loop.
        if (lock_i2c(pdMS_TO_TICKS(50))) {
            M5.update();
            touch_state = M5.Touch.getCount();
            unlock_i2c();
        }

        // Interpret any count increase or decrease as one circular context
        // navigation step.
        if (knob != NULL) {
            int encoder_count = iot_knob_get_count_value(knob);

            if (encoder_count != last_encoder_count) {
                if (encoder_count > last_encoder_count) {
                    current_context++;
                } else {
                    current_context--;
                }

                // Interpret any count increase or decrease as one circular context
                // navigation step.
                if (current_context >= CONTEXT_COUNT) {
                    current_context = 0;
                }

                if (current_context < 0) {
                    current_context = CONTEXT_COUNT - 1;
                }

                ESP_LOGI(
    TAG,
    "Context selected: %s",
    contexts[current_context].display_name
);

                request_nfc_quiet_window(pdMS_TO_TICKS(350));

                last_encoder_count = encoder_count;
                if (!identity_visual_active && !presence_prompt_active) {
                    draw_console();
                }
            }
        }

        if (touch_state > 0 && last_touch_state == 0 && !identity_visual_active && !presence_prompt_active) {
            current_profile++;

            if (current_profile >= PROFILE_COUNT) {
                current_profile = 0;
            }

            ESP_LOGI(TAG, "Profile selected: %s / %s",
                     profiles[current_profile].name,
                     profiles[current_profile].role);

            request_nfc_quiet_window(pdMS_TO_TICKS(350));

            M5.Speaker.tone(2500, 80);
            draw_console();
            generate_identity_package();
        }

        last_touch_state = touch_state;

        IdentityEvent event = {};

        while (xQueueReceive(identity_event_queue, &event, 0) == pdTRUE) {
            switch (event.type) {
                case EVENT_NFC_CARD_PRESENT:
                    nfc_card_present = true;
                    strncpy(last_nfc_status, "Card present", sizeof(last_nfc_status) - 1);
                    last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';
                    draw_console();
                    break;

                case EVENT_NFC_CARD_REMOVED:
                    nfc_card_present = false;
                    identity_visual_active = false;
                    last_nfc_uid[0] = '\0';

                    strncpy(last_nfc_status, "Card removed", sizeof(last_nfc_status) - 1);
                    last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';

                    ESP_LOGI(TAG, "NFC card stable: NO");

                    M5.Speaker.tone(1800, 80);
                    draw_console();
                    break;

                case EVENT_NFC_UID_READ:
                    nfc_detected = true;
                    nfc_card_present = true;

                    strncpy(last_nfc_uid, event.uid, sizeof(last_nfc_uid) - 1);
                    last_nfc_uid[sizeof(last_nfc_uid) - 1] = '\0';

                    current_profile = event.profile_index;

                    snprintf(last_nfc_status,
                             sizeof(last_nfc_status),
                             "UID mapped");

                    ESP_LOGI(TAG, "NFC UID: %s", last_nfc_uid);
                    ESP_LOGI(TAG, "NFC mapped profile: %s / %s",
                             profiles[current_profile].name,
                             profiles[current_profile].role);

                    show_identity_visualization(current_profile, last_nfc_uid);
                    M5.Speaker.tone(2800, 80);
                    generate_identity_package();
                    break;

                case EVENT_NFC_ERROR:
                    nfc_detected = false;
                    nfc_card_present = false;
                    identity_visual_active = false;

                    if (event.message[0] != '\0') {
                        strncpy(last_nfc_status, event.message, sizeof(last_nfc_status) - 1);
                        last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';
                    } else {
                        strncpy(last_nfc_status, "NFC error", sizeof(last_nfc_status) - 1);
                        last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';
                    }

                    ESP_LOGW(TAG, "NFC error event: %s", last_nfc_status);

                    draw_console();
                    break;

                case EVENT_NFC_RECOVERED:
                    nfc_detected = true;

                    if (event.message[0] != '\0') {
                        strncpy(last_nfc_status, event.message, sizeof(last_nfc_status) - 1);
                        last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';
                    } else {
                        strncpy(last_nfc_status, "NFC ready", sizeof(last_nfc_status) - 1);
                        last_nfc_status[sizeof(last_nfc_status) - 1] = '\0';
                    }

                    ESP_LOGI(TAG, "NFC recovered: %s", last_nfc_status);

                    if (!presence_prompt_active) {
                        draw_console();
                    }
                    break;

                case EVENT_PRESENCE_RECEIVED:
                    ESP_LOGI(TAG, "Presence event received: show NFC prompt");
                    show_presence_prompt();
                    break;

                default:
                    break;
            }
        }

        update_presence_prompt_timeout();

        if (!presence_prompt_active) {
            update_identity_visualization_timeout();
        }

        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

// -----------------------------------------------------------------------------
// app_main
// -----------------------------------------------------------------------------

extern "C" void app_main(void)
{
    identity_event_queue = xQueueCreate(10, sizeof(IdentityEvent));
    i2c_bus_mutex = xSemaphoreCreateMutex();

    if (identity_event_queue == NULL || i2c_bus_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create runtime primitives");
        return;
    }

    wifi_init_sta();

    xTaskCreatePinnedToCore(ui_task,  "ui_task",  8192, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(nfc_task, "nfc_task", 8192, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(udp_listener_task, "udp_listener_task", 4096, NULL, 3, NULL, 0);
}
