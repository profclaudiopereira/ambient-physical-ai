#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"

#include "ld2410.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static const char *TAG = "presence-radar";

#define WIFI_SSID "OKFIBRA-Claudio_2GHz"
#define WIFI_PASS "15120813"

#define UDP_BROADCAST_IP "192.168.77.255"
#define UDP_PORT 3333

static int udp_socket_fd = -1;
static struct sockaddr_in udp_dest_addr = {};
static esp_netif_t *wifi_sta_netif = nullptr;

static bool presence_state = false;
static volatile bool wifi_connected = false;
static bool pending_presence_udp = false;
static uint16_t pending_presence_distance_mm = 0;

// -----------------------------------------------------------------------------
// Wi-Fi STA + UDP Broadcast
// -----------------------------------------------------------------------------

static void restart_dhcp_client()
{
    if (wifi_sta_netif == nullptr) {
        ESP_LOGE(TAG, "Wi-Fi netif is not available");
        return;
    }

    esp_netif_dhcp_status_t status = ESP_NETIF_DHCP_INIT;
    const esp_err_t status_err =
        esp_netif_dhcpc_get_status(wifi_sta_netif, &status);

    if (status_err == ESP_OK) {
        ESP_LOGI(TAG, "DHCP client status: %d", static_cast<int>(status));
    }

    if (status == ESP_NETIF_DHCP_STARTED) {
        const esp_err_t stop_err =
            esp_netif_dhcpc_stop(wifi_sta_netif);

        if (stop_err != ESP_OK &&
            stop_err != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
            ESP_LOGW(
                TAG,
                "DHCP client stop returned: %s",
                esp_err_to_name(stop_err)
            );
        }
    }

    const esp_err_t start_err =
        esp_netif_dhcpc_start(wifi_sta_netif);

    if (start_err == ESP_OK ||
        start_err == ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED) {
        ESP_LOGI(TAG, "DHCP client active");
    } else {
        ESP_LOGE(
            TAG,
            "Failed to start DHCP client: %s",
            esp_err_to_name(start_err)
        );
    }
}

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi STA started; connecting...");
        ESP_ERROR_CHECK(esp_wifi_connect());

    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi associated with access point");
        restart_dhcp_client();

    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        const wifi_event_sta_disconnected_t *event =
            static_cast<const wifi_event_sta_disconnected_t *>(event_data);

        wifi_connected = false;

        ESP_LOGW(
            TAG,
            "Wi-Fi disconnected, reason=%d, reconnecting...",
            event != nullptr ? event->reason : -1
        );

        esp_wifi_connect();

    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *event =
            static_cast<const ip_event_got_ip_t *>(event_data);

        wifi_connected = true;

        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG,
                 "Wi-Fi connected. IP      : " IPSTR,
                 IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG,
                 "Netmask                 : " IPSTR,
                 IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG,
                 "Gateway                 : " IPSTR,
                 IP2STR(&event->ip_info.gw));
        ESP_LOGI(TAG, "Network ready          : YES");
        ESP_LOGI(TAG, "========================================");
    }
}

static void wifi_init_sta()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
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

    wifi_sta_netif = esp_netif_create_default_wifi_sta();

    if (wifi_sta_netif == nullptr) {
        ESP_LOGE(TAG, "Failed to create default Wi-Fi STA netif");
        return;
    }

    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        nullptr,
        nullptr
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        nullptr,
        nullptr
    ));

    wifi_config_t wifi_config = {};
    strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid),
            WIFI_SSID,
            sizeof(wifi_config.sta.ssid));
    strncpy(reinterpret_cast<char *>(wifi_config.sta.password),
            WIFI_PASS,
            sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config)
    );
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA started. SSID: %s", WIFI_SSID);
}

static void udp_broadcast_init()
{
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (udp_socket_fd < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket");
        return;
    }

    const int broadcast_enable = 1;
    const int ret = setsockopt(
        udp_socket_fd,
        SOL_SOCKET,
        SO_BROADCAST,
        &broadcast_enable,
        sizeof(broadcast_enable)
    );

    if (ret < 0) {
        ESP_LOGW(TAG, "Failed to enable UDP broadcast");
    }

    udp_dest_addr = {};
    udp_dest_addr.sin_family = AF_INET;
    udp_dest_addr.sin_port = htons(UDP_PORT);
    udp_dest_addr.sin_addr.s_addr = inet_addr(UDP_BROADCAST_IP);

    ESP_LOGI(TAG,
             "UDP broadcast configured: %s:%d",
             UDP_BROADCAST_IP,
             UDP_PORT);
}

static bool send_presence_event_udp(uint16_t distance_mm)
{
    if (!wifi_connected) {
        ESP_LOGW(TAG,
                 "UDP send postponed: Wi-Fi not connected yet");
        return false;
    }

    if (udp_socket_fd < 0) {
        ESP_LOGW(TAG, "UDP socket not ready");
        return false;
    }

    char payload[160] = {0};

    snprintf(
        payload,
        sizeof(payload),
        "{\"type\":\"presence_event\","
        "\"state\":\"PRESENT\","
        "\"distance_mm\":%u,"
        "\"source\":\"presence_node_v1\"}",
        static_cast<unsigned>(distance_mm)
    );

    const int sent = sendto(
        udp_socket_fd,
        payload,
        strlen(payload),
        0,
        reinterpret_cast<struct sockaddr *>(&udp_dest_addr),
        sizeof(udp_dest_addr)
    );

    if (sent < 0) {
        ESP_LOGW(TAG, "UDP send failed, errno=%d", errno);
        return false;
    }

    ESP_LOGI(TAG, "UDP presence_event sent: %s", payload);
    return true;
}

// -----------------------------------------------------------------------------
// LD2410C Presence Adapter
// -----------------------------------------------------------------------------

static bool target_state_is_present(ld2410_target_state_t state)
{
    return state == LD2410_TARGET_MOVING ||
           state == LD2410_TARGET_STATIONARY ||
           state == LD2410_TARGET_MOVING_AND_STATIONARY;
}

static uint16_t detection_distance_to_mm(uint16_t distance_cm)
{
    const uint32_t distance_mm =
        static_cast<uint32_t>(distance_cm) * 10U;

    return distance_mm > UINT16_MAX
        ? UINT16_MAX
        : static_cast<uint16_t>(distance_mm);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "PRESENCE_NODE_V2_MILESTONE_001");
    ESP_LOGI(TAG, "AtomS3 Lite + HLK-LD2410C");
    ESP_LOGI(TAG,
             "Goal: native human presence detection + existing UDP contract");
    ESP_LOGI(TAG,
             "UART1: TX=GPIO1 RX=GPIO2 baud=256000");
    ESP_LOGI(TAG, "Wi-Fi SSID: %s", WIFI_SSID);
    ESP_LOGI(TAG,
             "UDP broadcast: %s:%d",
             UDP_BROADCAST_IP,
             UDP_PORT);

    ld2410_config_t radar_config = {};
    radar_config.uart_port = UART_NUM_1;
    radar_config.tx_gpio = GPIO_NUM_1;
    radar_config.rx_gpio = GPIO_NUM_2;
    radar_config.baud_rate = 256000;
    radar_config.rx_buffer_size = 2048;

    ld2410_handle_t radar = nullptr;

    esp_err_t ret = ld2410_new(&radar_config, &radar);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG,
                 "LD2410C initialization failed: %s",
                 esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG,
             "LD2410C initialized. Starting existing Wi-Fi/UDP stack.");

    wifi_init_sta();
    udp_broadcast_init();

    while (true) {
        ld2410_target_data_t current = {};

        ret = ld2410_read(
            radar,
            &current,
            pdMS_TO_TICKS(500)
        );

        if (ret == ESP_ERR_TIMEOUT) {
            continue;
        }

        if (ret != ESP_OK) {
            ESP_LOGW(TAG,
                     "LD2410C read failed: %s",
                     esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (current.state == LD2410_TARGET_UNKNOWN) {
            ESP_LOGW(TAG,
                     "LD2410C returned UNKNOWN state; preserving current presence state");
            continue;
        }

        const bool radar_present =
            target_state_is_present(current.state);

        const uint16_t distance_mm =
            detection_distance_to_mm(
                current.detection_distance_cm
            );

        ESP_LOGI(
            TAG,
            "Radar: state=%s detection=%u cm moving=%u cm/%u stationary=%u cm/%u",
            ld2410_target_state_to_string(current.state),
            static_cast<unsigned>(current.detection_distance_cm),
            static_cast<unsigned>(current.moving_distance_cm),
            static_cast<unsigned>(current.moving_energy),
            static_cast<unsigned>(current.stationary_distance_cm),
            static_cast<unsigned>(current.stationary_energy)
        );

        if (!presence_state && radar_present) {
            presence_state = true;
            pending_presence_distance_mm = distance_mm;

            ESP_LOGI(TAG, "PRESENT");

            if (!send_presence_event_udp(distance_mm)) {
                pending_presence_udp = true;
            }
        } else if (presence_state && !radar_present) {
            presence_state = false;
            pending_presence_udp = false;

            ESP_LOGI(TAG, "NOT_PRESENT");
        }

        if (presence_state &&
            pending_presence_udp &&
            wifi_connected) {
            if (send_presence_event_udp(
                    pending_presence_distance_mm)) {
                pending_presence_udp = false;
            }
        }
    }
}
