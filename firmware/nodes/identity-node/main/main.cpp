/*
 * Identity Console V1
 *
 * Ambient Physical AI
 *
 * FreeRTOS Runtime Stabilized Baseline V6
 *
 * Architecture:
 * - UI Task owns M5.update(), Display, Touch, Encoder, Buzzer
 * - NFC Task owns WS1850S, NFC polling, UID reading, recovery
 * - Identity Event Queue connects NFC -> UI
 *
 * Stabilization:
 * - Shared I2C mutex protects only short I2C transactions:
 *   M5.update()/Touch read and WS1850S operations.
 * - No global mutex around drawing/display logic.
 * - ESP_ERR_NOT_FOUND during UID read is treated as card removal/absence,
 *   not as a critical NFC failure.
 *
 * Scope:
 * - No ToF
 * - No MQTT
 * - No NDEF
 * - No StackFlow
 * - No new UI
 */

#include "M5Unified.h"
#include "esp_log.h"
#include "esp_err.h"
#include <errno.h>

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

static const char *TAG = "identity-node";

#define WIFI_SSID "OKFIBRA-Claudio_2GHz"
#define WIFI_PASS "15120813"

#define UDP_LISTEN_PORT 3333
#define UDP_RX_BUFFER_SIZE 256
#define AX630C_IP "192.168.77.15"
#define AX630C_UDP_PORT 4444

// -----------------------------------------------------------------------------
// Profiles / Contexts
// -----------------------------------------------------------------------------

struct Profile {
    const char *id;
    const char *name;
    const char *role;
};

static const Profile profiles[] = {
    {"unknown", "Unknown", "visitor"},
    {"claudio", "Claudio", "owner"},
    {"student", "Student", "learner"},
};

enum ContextId {
    CONTEXT_LAB = 0,
    CONTEXT_RESEARCH,
    CONTEXT_CLASSROOM,
    CONTEXT_DEMO,
    CONTEXT_MEETING,
};

struct ContextDefinition {
    ContextId id;
    const char *protocol_name;
    const char *display_name;
};

static const ContextDefinition contexts[] = {
    {CONTEXT_LAB,       "Lab",       "Lab"},
    {CONTEXT_RESEARCH,  "Research",  "Research"},
    {CONTEXT_CLASSROOM, "Classroom", "Classroom"},
    {CONTEXT_DEMO,      "Demo",      "Demo"},
    {CONTEXT_MEETING,   "Meeting",   "Meeting"},
};

static const int PROFILE_COUNT = sizeof(profiles) / sizeof(profiles[0]);
static const int CONTEXT_COUNT = sizeof(contexts) / sizeof(contexts[0]);

// -----------------------------------------------------------------------------
// Identity Events
// -----------------------------------------------------------------------------

enum IdentityEventType {
    EVENT_NFC_CARD_PRESENT,
    EVENT_NFC_CARD_REMOVED,
    EVENT_NFC_UID_READ,
    EVENT_NFC_ERROR,
    EVENT_NFC_RECOVERED,
    EVENT_PRESENCE_RECEIVED,
};

struct IdentityEvent {
    IdentityEventType type;
    char uid[32];
    int profile_index;
    char message[64];
};

static QueueHandle_t identity_event_queue = NULL;
static SemaphoreHandle_t i2c_bus_mutex = NULL;

// UI activity guard: when the user is actively rotating/touching,
// NFC polling backs off briefly. This avoids hammering the shared M5Dial
// I2C lines while M5.update()/touch processing is active.
static volatile TickType_t ui_quiet_until = 0;

static void request_nfc_quiet_window(TickType_t duration)
{
    ui_quiet_until = xTaskGetTickCount() + duration;
}

static bool is_ui_quiet_window_active()
{
    TickType_t now = xTaskGetTickCount();
    return ((int32_t)(ui_quiet_until - now)) > 0;
}


// -----------------------------------------------------------------------------
// UI Runtime State
// -----------------------------------------------------------------------------

static int current_profile = 0;
static int current_context = 0;

static bool nfc_detected = false;
static bool nfc_card_present = false;

static char last_nfc_uid[32] = "";
static char last_nfc_status[64] = "NFC: starting";

static bool identity_visual_active = false;
static TickType_t identity_visual_until = 0;
static const TickType_t IDENTITY_VISUAL_DURATION = pdMS_TO_TICKS(3000);

static bool presence_prompt_active = false;
static TickType_t presence_prompt_until = 0;
static const TickType_t PRESENCE_PROMPT_DURATION = pdMS_TO_TICKS(5000);

// -----------------------------------------------------------------------------
// NFC Runtime State
// -----------------------------------------------------------------------------

static i2c_master_bus_handle_t nfc_bus_handle = NULL;
static i2c_master_dev_handle_t nfc_dev_handle = NULL;

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

static const TickType_t NFC_BOOT_DELAY       = pdMS_TO_TICKS(1200);
static const TickType_t NFC_IDLE_DELAY       = pdMS_TO_TICKS(120);
static const TickType_t NFC_POLL_DELAY       = pdMS_TO_TICKS(450);
static const TickType_t NFC_READ_DELAY       = pdMS_TO_TICKS(80);
static const TickType_t NFC_SHORT_COOLDOWN   = pdMS_TO_TICKS(300);
static const TickType_t NFC_MEDIUM_COOLDOWN  = pdMS_TO_TICKS(900);
static const TickType_t NFC_LONG_COOLDOWN    = pdMS_TO_TICKS(3000);

static const int CARD_REMOVED_CONFIRM_COUNT = 5;
static const int NFC_UID_READ_ATTEMPTS = 8;
static const TickType_t NFC_UID_RETRY_DELAY = pdMS_TO_TICKS(60);

// -----------------------------------------------------------------------------
// Network Runtime State
// -----------------------------------------------------------------------------

static volatile bool wifi_connected = false;

// -----------------------------------------------------------------------------
// I2C lock helpers
// -----------------------------------------------------------------------------

static bool lock_i2c(TickType_t timeout)
{
    if (i2c_bus_mutex == NULL) {
        return false;
    }

    return xSemaphoreTake(i2c_bus_mutex, timeout) == pdTRUE;
}

static void unlock_i2c()
{
    if (i2c_bus_mutex != NULL) {
        xSemaphoreGive(i2c_bus_mutex);
    }
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static int find_profile_by_uid(const char *uid)
{
    if (strcmp(uid, "8804DC32") == 0) {
        return 1;
    }

    if (strcmp(uid, "88048667") == 0) {
        return 2;
    }

    return 0;
}

static void send_identity_event(const IdentityEvent &event)
{
    if (identity_event_queue != NULL) {
        xQueueSend(identity_event_queue, &event, pdMS_TO_TICKS(20));
    }
}

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

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        ESP_LOGW(TAG, "Wi-Fi disconnected, reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        wifi_connected = true;
        ESP_LOGI(TAG, "Wi-Fi connected. IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

static void wifi_init_sta()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(ret);
    }

    ESP_ERROR_CHECK(esp_netif_init());

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(ret);
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA started. SSID: %s", WIFI_SSID);
}

static bool is_presence_event_payload(const char *payload)
{
    if (payload == NULL) {
        return false;
    }

    return strstr(payload, "\"type\":\"presence_event\"") != NULL &&
           strstr(payload, "\"state\":\"PRESENT\"") != NULL &&
           strstr(payload, "\"source\":\"presence_node_v1\"") != NULL;
}

static void udp_listener_task(void *param)
{
    (void)param;

    ESP_LOGI(TAG, "UDP listener task started");

    while (!wifi_connected) {
        ESP_LOGI(TAG, "UDP listener waiting for Wi-Fi...");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Wi-Fi ready; creating UDP listener");

    char rx_buffer[UDP_RX_BUFFER_SIZE] = {0};

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

        memset(rx_buffer, 0, sizeof(rx_buffer));

        ESP_LOGI(TAG, "UDP listener waiting for packet...");

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

static void draw_console()
{
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextDatum(middle_center);

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

    const char *initial = "?";

    if (strcmp(profile.id, "claudio") == 0) {
        initial = "C";
    } else if (strcmp(profile.id, "student") == 0) {
        initial = "S";
    }

    M5.Display.fillCircle(120, 85, 29, DARKGREY);
    M5.Display.drawCircle(120, 85, 30, WHITE);

    M5.Display.setTextSize(3);
    M5.Display.drawString(initial, 120, 85);

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

    M5.Display.setTextDatum(top_left);
}
static void draw_presence_prompt()
{
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextDatum(middle_center);

    M5.Display.setTextSize(2);
    M5.Display.drawString("Presence detected", 120, 105);

    M5.Display.setTextSize(2);
    M5.Display.drawString("Tap NFC card", 120, 145);

    M5.Display.setTextDatum(top_left);
}

static void show_presence_prompt()
{
    presence_prompt_active = true;
    presence_prompt_until = xTaskGetTickCount() + PRESENCE_PROMPT_DURATION;

    identity_visual_active = false;

    draw_presence_prompt();
    M5.Speaker.tone(2200, 80);
}

static void update_presence_prompt_timeout()
{
    if (!presence_prompt_active) {
        return;
    }

    TickType_t now = xTaskGetTickCount();

    if (((int32_t)(presence_prompt_until - now)) <= 0) {
        presence_prompt_active = false;
        draw_console();
    }
}


static void draw_identity_visualization(int profile_index, const char *uid)
{
    if (profile_index < 0 || profile_index >= PROFILE_COUNT) {
        profile_index = 0;
    }

    const Profile &profile = profiles[profile_index];
    const char *initial = "?";

    if (strcmp(profile.id, "claudio") == 0) {
        initial = "C";
    } else if (strcmp(profile.id, "student") == 0) {
        initial = "S";
    }

    M5.Display.fillScreen(BLACK);

    M5.Display.setTextDatum(middle_center);

    M5.Display.setTextSize(2);
    M5.Display.drawString("IDENTITY", 120, 22);

    M5.Display.drawRoundRect(15, 45, 210, 185, 14, WHITE);

    M5.Display.fillCircle(120, 82, 28, DARKGREY);
    M5.Display.drawCircle(120, 82, 29, WHITE);

    M5.Display.setTextSize(3);
    M5.Display.drawString(initial, 120, 82);

    M5.Display.setTextSize(2);
    M5.Display.drawString(profile.name, 120, 125);

    M5.Display.setTextSize(1);
    M5.Display.drawString(profile.role, 120, 150);

    char context_line[48] = {0};
    snprintf(
    context_line,
    sizeof(context_line),
    "Context: %s",
    contexts[current_context].display_name
);
    M5.Display.drawString(context_line, 120, 178);

    char uid_line[48] = {0};
    if (uid != NULL && uid[0] != '\0') {
        snprintf(uid_line, sizeof(uid_line), "UID: %.8s", uid);
    } else {
        snprintf(uid_line, sizeof(uid_line), "UID: none");
    }
    M5.Display.drawString(uid_line, 120, 202);

    M5.Display.setTextDatum(top_left);
}

static void show_identity_visualization(int profile_index, const char *uid)
{
    identity_visual_active = true;
    identity_visual_until = xTaskGetTickCount() + IDENTITY_VISUAL_DURATION;
    draw_identity_visualization(profile_index, uid);
}

static void update_identity_visualization_timeout()
{
    if (!identity_visual_active) {
        return;
    }

    TickType_t now = xTaskGetTickCount();

    if (((int32_t)(identity_visual_until - now)) <= 0) {
        identity_visual_active = false;
        draw_console();
    }
}


static void send_identity_package_udp(const char *json_payload)
{
    if (json_payload == NULL || json_payload[0] == '\0') {
        ESP_LOGW(TAG, "Identity UDP TX skipped: empty payload");
        return;
    }

    if (!wifi_connected) {
        ESP_LOGW(TAG, "Identity UDP TX skipped: Wi-Fi not connected");
        return;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Identity UDP TX socket creation failed");
        return;
    }

    struct sockaddr_in dest_addr = {};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(AX630C_UDP_PORT);
    inet_pton(AF_INET, AX630C_IP, &dest_addr.sin_addr);

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


static void generate_identity_package()
{
    const char *package_uid = nfc_card_present ? last_nfc_uid : "";

    char identity_json[512] = {0};

    snprintf(identity_json,
             sizeof(identity_json),
             "{\"type\":\"identity_package\",\"profile\":{\"id\":\"%s\",\"name\":\"%s\",\"role\":\"%s\"},\"context\":\"%s\",\"nfc\":{\"detected\":%s,\"card_present\":%s,\"uid\":\"%s\"},\"source\":\"m5dial_identity_console_v1\"}",
             profiles[current_profile].id,
             profiles[current_profile].name,
             profiles[current_profile].role,
             contexts[current_context].protocol_name,
             nfc_detected ? "true" : "false",
             nfc_card_present ? "true" : "false",
             package_uid);

    ESP_LOGI(TAG, "%s", identity_json);

    send_identity_package_udp(identity_json);
}
// -----------------------------------------------------------------------------
// NFC Setup / Reset
// -----------------------------------------------------------------------------

static void reset_nfc_runtime_unlocked()
{
    if (nfc_dev_handle != NULL) {
        i2c_master_bus_rm_device(nfc_dev_handle);
        nfc_dev_handle = NULL;
    }

    if (nfc_bus_handle != NULL) {
        i2c_del_master_bus(nfc_bus_handle);
        nfc_bus_handle = NULL;
    }

    nfc_detected = false;
}

static bool setup_nfc_unlocked()
{
    reset_nfc_runtime_unlocked();

    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = BSP_I2C_SDA;
    bus_cfg.scl_io_num = BSP_I2C_SCL;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;

    esp_err_t ret = i2c_new_master_bus(&bus_cfg, &nfc_bus_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC I2C bus init failed: %s", esp_err_to_name(ret));
        return false;
    }

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = WS1850S_I2C_ADDRESS;
    dev_cfg.scl_speed_hz = 100000;

    ret = i2c_master_bus_add_device(nfc_bus_handle, &dev_cfg, &nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC add device failed: %s", esp_err_to_name(ret));
        reset_nfc_runtime_unlocked();
        return false;
    }

    ret = ws1850s_probe(nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S probe failed: %s", esp_err_to_name(ret));
        reset_nfc_runtime_unlocked();
        return false;
    }

    uint8_t version = 0;
    ret = ws1850s_read_version(nfc_dev_handle, &version);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NFC/WS1850S VersionReg: 0x%02X", version);
    } else {
        ESP_LOGW(TAG, "NFC/WS1850S VersionReg read failed: %s", esp_err_to_name(ret));
    }

    ret = ws1850s_init(nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S init failed: %s", esp_err_to_name(ret));
        reset_nfc_runtime_unlocked();
        return false;
    }

    ESP_LOGI(TAG, "NFC/WS1850S initialized at 0x28");

    nfc_detected = true;
    return true;
}

static bool setup_nfc_locked()
{
    bool ok = false;

    if (lock_i2c(pdMS_TO_TICKS(800))) {
        ok = setup_nfc_unlocked();
        unlock_i2c();
    } else {
        ESP_LOGW(TAG, "NFC setup skipped: I2C lock timeout");
    }

    return ok;
}

static void reset_nfc_runtime_locked()
{
    if (lock_i2c(pdMS_TO_TICKS(800))) {
        reset_nfc_runtime_unlocked();
        unlock_i2c();
    } else {
        ESP_LOGW(TAG, "NFC reset skipped: I2C lock timeout");
    }
}

static esp_err_t read_uid_text_locked(char *uid_text, size_t uid_text_size)
{
    if (uid_text == NULL || uid_text_size == 0 || nfc_dev_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t last_ret = ESP_FAIL;

    for (int attempt = 1; attempt <= NFC_UID_READ_ATTEMPTS; attempt++) {
        if (!lock_i2c(pdMS_TO_TICKS(500))) {
            last_ret = ESP_ERR_TIMEOUT;
        } else {
            uint8_t uid[10] = {0};
            uint8_t uid_len = 0;
            bool present = false;

            // Each acquisition attempt starts with a fresh REQA.
            // This is more reliable than doing one REQA in NFC_POLL and then
            // retrying anticollision several times without re-arming the card.
            last_ret = ws1850s_card_present(nfc_dev_handle, &present);

            if (last_ret == ESP_OK && present) {
                vTaskDelay(pdMS_TO_TICKS(8));

                // Controlled re-arm during acquisition only.
                // Avoid doing this on every normal poll.
                if (attempt == 4 && nfc_dev_handle != NULL) {
                    ws1850s_init(nfc_dev_handle);
                    vTaskDelay(pdMS_TO_TICKS(25));
                    ws1850s_card_present(nfc_dev_handle, &present);
                    vTaskDelay(pdMS_TO_TICKS(8));
                }

                last_ret = ws1850s_read_uid(nfc_dev_handle, uid, &uid_len);
            } else if (last_ret == ESP_OK && !present) {
                last_ret = ESP_ERR_NOT_FOUND;
            }

            unlock_i2c();

            if (last_ret == ESP_OK) {
                memset(uid_text, 0, uid_text_size);

                for (int i = 0; i < uid_len && (i * 2 + 1) < (int)uid_text_size; i++) {
                    snprintf(uid_text + (i * 2),
                             uid_text_size - (i * 2),
                             "%02X",
                             uid[i]);
                }

                if (attempt > 1) {
                    ESP_LOGI(TAG, "NFC UID acquired after %d attempts", attempt);
                }

                return ESP_OK;
            }
        }

        // Acquisition misses: retry quietly. These are common while the card is
        // entering/leaving the RF field or near the edge of the antenna.
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

    return ESP_ERR_NOT_FOUND;
}

// -----------------------------------------------------------------------------
// NFC Task
// -----------------------------------------------------------------------------

static void nfc_task(void *param)
{
    (void)param;

    vTaskDelay(NFC_BOOT_DELAY);

    NfcState state = NFC_INIT;

    bool raw_present = false;
    bool stable_present = false;

    int removed_count = 0;
    int consecutive_errors = 0;
    esp_err_t last_nfc_error = ESP_OK;

    TickType_t cooldown_time = NFC_SHORT_COOLDOWN;

    while (true) {
        switch (state) {
            case NFC_INIT: {
                ESP_LOGI(TAG, "NFC state: INIT");

                if (setup_nfc_locked()) {
                    consecutive_errors = 0;
                    send_simple_event(EVENT_NFC_RECOVERED, "NFC ready");
                    state = NFC_IDLE;
                } else {
                    consecutive_errors++;
                    cooldown_time = NFC_LONG_COOLDOWN;
                    send_simple_event(EVENT_NFC_ERROR, "NFC init failed");
                    state = NFC_COOLDOWN;
                }

                break;
            }

            case NFC_IDLE: {
                if (is_ui_quiet_window_active()) {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;
                }

                vTaskDelay(NFC_IDLE_DELAY);
                state = NFC_POLL;
                break;
            }

            case NFC_POLL: {
                if (is_ui_quiet_window_active()) {
                    state = NFC_IDLE;
                    break;
                }

                if (nfc_dev_handle == NULL) {
                    state = NFC_INIT;
                    break;
                }

                raw_present = false;
                esp_err_t ret = ESP_FAIL;

                if (lock_i2c(pdMS_TO_TICKS(200))) {
                    ret = ws1850s_card_present(nfc_dev_handle, &raw_present);
                    unlock_i2c();
                } else {
                    ret = ESP_ERR_TIMEOUT;
                }

                if (ret != ESP_OK) {
                    last_nfc_error = ret;
                    ESP_LOGW(TAG, "NFC polling failed: %s", esp_err_to_name(ret));
                    state = NFC_ERROR;
                    break;
                }

                consecutive_errors = 0;

                if (raw_present) {
                    removed_count = 0;

                    if (!stable_present) {
                        state = NFC_READ_UID;
                    } else {
                        state = NFC_CARD_PRESENT;
                    }
                } else {
                    if (stable_present) {
                        state = NFC_CARD_REMOVED;
                    } else {
                        state = NFC_IDLE;
                    }
                }

                break;
            }

            case NFC_READ_UID: {
                vTaskDelay(NFC_READ_DELAY);

                char uid_text[32] = {0};
                esp_err_t ret = read_uid_text_locked(uid_text, sizeof(uid_text));

                if (ret == ESP_OK) {
                    stable_present = true;
                    removed_count = 0;
                    consecutive_errors = 0;

                    IdentityEvent event = {};
                    event.type = EVENT_NFC_UID_READ;
                    strncpy(event.uid, uid_text, sizeof(event.uid) - 1);
                    event.uid[sizeof(event.uid) - 1] = '\0';
                    event.profile_index = find_profile_by_uid(event.uid);

                    send_identity_event(event);

                    state = NFC_CARD_PRESENT;
                } else if (ret == ESP_ERR_NOT_FOUND) {
                    // Card was present during poll but disappeared before UID read.
                    // This is a normal removal/absence condition, not a critical NFC error.
                    stable_present = false;
                    removed_count = 0;
                    state = NFC_CARD_REMOVED;
                } else {
                    last_nfc_error = ret;
                    state = NFC_ERROR;
                }

                break;
            }

            case NFC_CARD_PRESENT: {
                vTaskDelay(NFC_POLL_DELAY);
                state = NFC_POLL;
                break;
            }

            case NFC_CARD_REMOVED: {
                removed_count++;

                if (removed_count >= CARD_REMOVED_CONFIRM_COUNT) {
                    stable_present = false;
                    removed_count = 0;

                    send_simple_event(EVENT_NFC_CARD_REMOVED, "Card removed");
                }

                state = NFC_IDLE;
                break;
            }

            case NFC_ERROR: {
                consecutive_errors++;

                ESP_LOGW(TAG, "NFC error count: %d", consecutive_errors);

                if (consecutive_errors == 1) {
                    if (last_nfc_error == ESP_ERR_INVALID_STATE || last_nfc_error == ESP_ERR_TIMEOUT) {
                        ESP_LOGW(TAG, "NFC recovery level 1: bus backoff only");
                        cooldown_time = NFC_MEDIUM_COOLDOWN;
                    } else {
                        ESP_LOGW(TAG, "NFC recovery level 1: soft reinit");

                        if (nfc_dev_handle != NULL && lock_i2c(pdMS_TO_TICKS(300))) {
                            ws1850s_init(nfc_dev_handle);
                            unlock_i2c();
                        }

                        cooldown_time = NFC_SHORT_COOLDOWN;
                    }
                } else if (consecutive_errors == 2) {
                    ESP_LOGW(TAG, "NFC recovery level 2: pause polling");

                    send_simple_event(EVENT_NFC_ERROR, "NFC polling paused");

                    cooldown_time = NFC_MEDIUM_COOLDOWN;
                } else {
                    ESP_LOGW(TAG, "NFC recovery level 3: controlled rebuild");

                    send_simple_event(EVENT_NFC_ERROR, "NFC rebuild");

                    reset_nfc_runtime_locked();

                    stable_present = false;
                    raw_present = false;
                    removed_count = 0;
                    consecutive_errors = 0;

                    cooldown_time = NFC_LONG_COOLDOWN;
                    state = NFC_COOLDOWN;
                    break;
                }

                state = NFC_COOLDOWN;
                break;
            }

            case NFC_COOLDOWN: {
                vTaskDelay(cooldown_time);

                if (nfc_dev_handle == NULL || !nfc_detected) {
                    state = NFC_INIT;
                } else {
                    state = NFC_IDLE;
                }

                break;
            }

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

static void ui_task(void *param)
{
    (void)param;

    auto cfg = M5.config();
    M5.begin(cfg);

    ESP_LOGI(TAG, "Identity Console V1");
    ESP_LOGI(TAG, "FreeRTOS runtime stabilized baseline V6 + identity visualization");

    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = BSP_ENCODER_A,
        .gpio_encoder_b = BSP_ENCODER_B,
        .enable_power_save = false,
    };

    knob_handle_t knob = iot_knob_create(&knob_cfg);

    if (knob == NULL) {
        ESP_LOGE(TAG, "Failed to create knob encoder");
    } else {
        ESP_LOGI(TAG, "Knob encoder initialized");
    }

    draw_console();
    M5.Speaker.tone(2000, 150);

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

        if (knob != NULL) {
            int encoder_count = iot_knob_get_count_value(knob);

            if (encoder_count != last_encoder_count) {
                if (encoder_count > last_encoder_count) {
                    current_context++;
                } else {
                    current_context--;
                }

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
