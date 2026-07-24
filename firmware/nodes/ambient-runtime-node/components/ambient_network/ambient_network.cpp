#include "ambient_network.h"

#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "ambient-network";

/*
 * Credenciais temporárias do laboratório.
 *
 * Depois do milestone, poderemos migrar isso para Kconfig,
 * NVS ou provisionamento. Nesta missão, não ampliaremos o escopo.
 */
#define AMBIENT_WIFI_SSID "OKFIBRA-Claudio_2GHz"
#define AMBIENT_WIFI_PASS "15120813"

static esp_netif_t *s_sta_netif = nullptr;

static ambient_network_status_t s_network_status = {
    .initialized = false,
    .connected = false,
    .network_ready = false,
    .ipv4 = "0.0.0.0",
    .netmask = "0.0.0.0",
    .gateway = "0.0.0.0",
    .rssi_dbm = 0
};

static esp_err_t initialize_nvs(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        ESP_LOGW(TAG, "NVS erase required");

        ret = nvs_flash_erase();

        if (ret != ESP_OK) {
            ESP_LOGE(
                TAG,
                "NVS erase failed: %s",
                esp_err_to_name(ret)
            );
            return ret;
        }

        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "NVS initialization failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    ESP_LOGI(TAG, "NVS initialized");
    return ESP_OK;
}

static void clear_network_address(void)
{
    snprintf(
        s_network_status.ipv4,
        sizeof(s_network_status.ipv4),
        "0.0.0.0"
    );

    snprintf(
        s_network_status.netmask,
        sizeof(s_network_status.netmask),
        "0.0.0.0"
    );

    snprintf(
        s_network_status.gateway,
        sizeof(s_network_status.gateway),
        "0.0.0.0"
    );

    s_network_status.rssi_dbm = 0;
}

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
)
{
    (void)arg;
    (void)event_data;

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "Wi-Fi Station started");
            ESP_LOGI(TAG, "Connecting to SSID: %s", AMBIENT_WIFI_SSID);

            if (esp_wifi_connect() != ESP_OK) {
                ESP_LOGE(TAG, "esp_wifi_connect failed");
            }
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Wi-Fi associated with access point");

            s_network_status.connected = true;
            s_network_status.network_ready = false;
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGW(TAG, "Wi-Fi disconnected");

            s_network_status.connected = false;
            s_network_status.network_ready = false;
            clear_network_address();

            /*
             * Reconexão mínima, necessária para estabilidade da demo.
             */
            ESP_LOGI(TAG, "Attempting Wi-Fi reconnection");

            if (esp_wifi_connect() != ESP_OK) {
                ESP_LOGE(TAG, "Wi-Fi reconnection request failed");
            }
            break;

        default:
            break;
        }
    }

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP) {

        const ip_event_got_ip_t *event =
            static_cast<const ip_event_got_ip_t *>(event_data);

        snprintf(
            s_network_status.ipv4,
            sizeof(s_network_status.ipv4),
            IPSTR,
            IP2STR(&event->ip_info.ip)
        );

        snprintf(
            s_network_status.netmask,
            sizeof(s_network_status.netmask),
            IPSTR,
            IP2STR(&event->ip_info.netmask)
        );

        snprintf(
            s_network_status.gateway,
            sizeof(s_network_status.gateway),
            IPSTR,
            IP2STR(&event->ip_info.gw)
        );

        s_network_status.connected = true;
        s_network_status.network_ready = true;

        ESP_LOGI(TAG, "DHCP IPv4 assigned");
        ESP_LOGI(TAG, "IP:      %s", s_network_status.ipv4);
        ESP_LOGI(TAG, "Netmask: %s", s_network_status.netmask);
        ESP_LOGI(TAG, "Gateway: %s", s_network_status.gateway);
        ESP_LOGI(TAG, "STATUS:  NETWORK READY");
    }
}

esp_err_t ambient_network_init(void)
{
    if (s_network_status.initialized) {
        ESP_LOGW(TAG, "Ambient Network already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Ambient Network Wi-Fi Station initialization");

    esp_err_t ret = initialize_nvs();

    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_netif_init();

    if (ret != ESP_OK &&
        ret != ESP_ERR_INVALID_STATE) {

        ESP_LOGE(
            TAG,
            "esp_netif_init failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    ret = esp_event_loop_create_default();

    if (ret != ESP_OK &&
        ret != ESP_ERR_INVALID_STATE) {

        ESP_LOGE(
            TAG,
            "Event loop creation failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    s_sta_netif = esp_netif_create_default_wifi_sta();

    if (s_sta_netif == nullptr) {
        ESP_LOGE(TAG, "Unable to create default Wi-Fi STA netif");
        return ESP_FAIL;
    }

    ret = esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        wifi_event_handler,
        nullptr
    );

    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "WIFI_EVENT handler registration failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    ret = esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        wifi_event_handler,
        nullptr
    );

    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "IP_EVENT handler registration failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    wifi_init_config_t wifi_init_config =
        WIFI_INIT_CONFIG_DEFAULT();

    ret = esp_wifi_init(&wifi_init_config);

    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "esp_wifi_init failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    /*
     * Desabilita power-save para reduzir latência e tornar
     * a demonstração mais previsível.
     */
    ret = esp_wifi_set_ps(WIFI_PS_NONE);

    if (ret != ESP_OK) {
        ESP_LOGW(
            TAG,
            "Unable to disable Wi-Fi power-save: %s",
            esp_err_to_name(ret)
        );
    }

    wifi_config_t wifi_config = {};

    strncpy(
        reinterpret_cast<char *>(wifi_config.sta.ssid),
        AMBIENT_WIFI_SSID,
        sizeof(wifi_config.sta.ssid) - 1
    );

    strncpy(
        reinterpret_cast<char *>(wifi_config.sta.password),
        AMBIENT_WIFI_PASS,
        sizeof(wifi_config.sta.password) - 1
    );

    wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ret = esp_wifi_set_mode(WIFI_MODE_STA);






    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "esp_wifi_set_mode failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

ESP_ERROR_CHECK(
    esp_netif_dhcpc_stop(s_sta_netif)
);

esp_netif_ip_info_t ip_info = {};

ip_info.ip.addr =
    ESP_IP4TOADDR(192,168,77,25);

ip_info.gw.addr =
    ESP_IP4TOADDR(192,168,77,1);

ip_info.netmask.addr =
    ESP_IP4TOADDR(255,255,255,0);

ESP_ERROR_CHECK(
    esp_netif_set_ip_info(
        s_sta_netif,
        &ip_info
    )
);




    ret = esp_wifi_set_config(
        WIFI_IF_STA,
        &wifi_config
    );

    if (ret != ESP_OK) {
        ESP_LOGE(
            TAG,
            "esp_wifi_set_config failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    s_network_status.initialized = true;
    s_network_status.connected = false;
    s_network_status.network_ready = false;
    clear_network_address();

    ret = esp_wifi_start();

    if (ret != ESP_OK) {
        s_network_status.initialized = false;

        ESP_LOGE(
            TAG,
            "esp_wifi_start failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    ESP_LOGI(TAG, "Wi-Fi Station initialization complete");
    ESP_LOGI(TAG, "Waiting for association and DHCP");

    return ESP_OK;
}

ambient_network_status_t ambient_network_get_status(void)
{
    if (s_network_status.connected) {
        wifi_ap_record_t ap_info = {};

        esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);

        if (ret == ESP_OK) {
            s_network_status.rssi_dbm = ap_info.rssi;
        } else {
            ESP_LOGW(
                TAG,
                "Unable to obtain RSSI: %s",
                esp_err_to_name(ret)
            );
        }
    }

    return s_network_status;
}