#include "wifi_station.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

/*
 * Update the Wi-Fi credentials below before building.
 * If static IP addressing is enabled, also adjust the network
 * configuration (IP, gateway and subnet mask) for your environment.
 */
#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASSWORD "<YOUR_WIFI_PASSWORD>"

#define WIFI_CONNECTED_BIT BIT0

static const char *TAG = "wifi_station";

static EventGroupHandle_t wifi_event_group;
static bool connected = false;

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START) {

        ESP_LOGI(TAG, "Wi-Fi station started");
        esp_wifi_connect();
        return;
    }

    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED) {

        connected = false;

        xEventGroupClearBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGW(TAG, "Wi-Fi disconnected; reconnecting");

        esp_wifi_connect();
        return;
    }

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP) {

        const ip_event_got_ip_t *event =
            (const ip_event_got_ip_t *)event_data;

        connected = true;

        ESP_LOGI(
            TAG,
            "IPv4 acquired: " IPSTR,
            IP2STR(&event->ip_info.ip)
        );

        ESP_LOGI(
            TAG,
            "Gateway: " IPSTR,
            IP2STR(&event->ip_info.gw)
        );

        xEventGroupSetBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );
    }
}

int wifi_station_init(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing Wi-Fi");

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "NVS initialization failed: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    err = esp_netif_init();

    if (err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE) {

        ESP_LOGE(
            TAG,
            "Network interface initialization failed: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    err = esp_event_loop_create_default();

    if (err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE) {

        ESP_LOGE(
            TAG,
            "Event loop initialization failed: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    wifi_event_group = xEventGroupCreate();

    if (wifi_event_group == NULL) {
        ESP_LOGE(TAG, "Could not create Wi-Fi event group");
        return -1;
    }

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_config =
        WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(&wifi_init_config);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Wi-Fi driver initialization failed: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );






    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );

    wifi_config_t wifi_config = {0};

    strncpy(
        (char *)wifi_config.sta.ssid,
        WIFI_SSID,
        sizeof(wifi_config.sta.ssid) - 1
    );

    strncpy(
        (char *)wifi_config.sta.password,
        WIFI_PASSWORD,
        sizeof(wifi_config.sta.password) - 1
    );

    wifi_config.sta.threshold.authmode =
        WIFI_AUTH_WPA2_PSK;

    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_STA)
    );

ESP_ERROR_CHECK(esp_netif_dhcpc_stop(sta_netif));

esp_netif_ip_info_t ip_info = {0};

ip_info.ip.addr =
    ESP_IP4TOADDR(192, 168, 1, 205);

ip_info.gw.addr =
    ESP_IP4TOADDR(192, 168, 1, 1);

ip_info.netmask.addr =
    ESP_IP4TOADDR(255, 255, 255, 0);
ESP_ERROR_CHECK(
    esp_netif_set_ip_info(sta_netif, &ip_info)
);




    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(
        TAG,
        "Connecting to SSID: %s",
        WIFI_SSID
    );

    return 0;
}

bool wifi_station_is_connected(void)
{
    return connected;
}