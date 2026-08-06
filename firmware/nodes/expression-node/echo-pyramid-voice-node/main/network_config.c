/**
 * @file network_config.c
 * @brief Static network configuration for the Echo Pyramid Voice Node.
 */

#include "network_config.h"

#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "lwip/inet.h"
#include "lwip/ip4_addr.h"

#define DEVICE_NAME       "echo-pyramid-voice-node"
#define DEVICE_HOSTNAME   "echo-pyramid-voice"

#define STATIC_IP_OCTET_1 192
#define STATIC_IP_OCTET_2 168
#define STATIC_IP_OCTET_3 1
#define STATIC_IP_OCTET_4 204

#define GATEWAY_OCTET_1   192
#define GATEWAY_OCTET_2   168
#define GATEWAY_OCTET_3   1
#define GATEWAY_OCTET_4   1

#define NETMASK_OCTET_1   255
#define NETMASK_OCTET_2   255
#define NETMASK_OCTET_3   255
#define NETMASK_OCTET_4   0

#define STATIC_IP_TEXT    "192.168.1.202"
#define STATIC_GATEWAY    "192.168.1.1"
#define STATIC_NETMASK    "255.255.255.0"
#define STATIC_DNS_MAIN   "192.168.1.1"
#define STATIC_DNS_BACKUP "8.8.8.8"


#define WIFI_SSID "APT_201_2G"
#define WIFI_PASS "Severina73"

static const char *TAG = "echo_pyramid_net";
static volatile bool s_wifi_connected = false;
static char s_current_ip[16] = STATIC_IP_TEXT;

static esp_err_t initialize_nvs(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            return ret;
        }
        ret = nvs_flash_init();
    }

    return ret;
}

static esp_err_t configure_static_ipv4(esp_netif_t *netif)
{
    esp_err_t ret = esp_netif_dhcpc_stop(netif);

    if (ret != ESP_OK && ret != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
        return ret;
    }

    esp_netif_ip_info_t ip_info = {0};

    IP4_ADDR(&ip_info.ip,
             STATIC_IP_OCTET_1, STATIC_IP_OCTET_2,
             STATIC_IP_OCTET_3, STATIC_IP_OCTET_4);
    IP4_ADDR(&ip_info.gw,
             GATEWAY_OCTET_1, GATEWAY_OCTET_2,
             GATEWAY_OCTET_3, GATEWAY_OCTET_4);
    IP4_ADDR(&ip_info.netmask,
             NETMASK_OCTET_1, NETMASK_OCTET_2,
             NETMASK_OCTET_3, NETMASK_OCTET_4);

    ret = esp_netif_set_ip_info(netif, &ip_info);
    if (ret != ESP_OK) {
        return ret;
    }

    esp_netif_dns_info_t dns_info = {0};
    dns_info.ip.type = IPADDR_TYPE_V4;

    dns_info.ip.u_addr.ip4.addr = ipaddr_addr(STATIC_DNS_MAIN);
    ret = esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
    if (ret != ESP_OK) {
        return ret;
    }

    dns_info.ip.u_addr.ip4.addr = ipaddr_addr(STATIC_DNS_BACKUP);
    ret = esp_netif_set_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG,
             "Static network configured: device=%s hostname=%s IP=%s GW=%s MASK=%s",
             DEVICE_NAME, DEVICE_HOSTNAME, STATIC_IP_TEXT,
             STATIC_GATEWAY, STATIC_NETMASK);

    return ESP_OK;
}

static void network_event_handler(void *arg,
                                  esp_event_base_t event_base,
                                  int32_t event_id,
                                  void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_err_t ret = esp_wifi_connect();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Initial Wi-Fi connection failed: %s",
                     esp_err_to_name(ret));
        }
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_wifi_connected = false;
        ESP_LOGW(TAG, "Wi-Fi disconnected, reconnecting...");

        esp_err_t ret = esp_wifi_connect();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Wi-Fi reconnection failed: %s",
                     esp_err_to_name(ret));
        }
    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *event =
            (const ip_event_got_ip_t *)event_data;

        s_wifi_connected = true;
        snprintf(s_current_ip, sizeof(s_current_ip),
                 IPSTR, IP2STR(&event->ip_info.ip));

        ESP_LOGI(TAG, "Wi-Fi connected. IP: %s", s_current_ip);
    }
}

esp_err_t network_init(void)
{
    esp_err_t ret = initialize_nvs();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_netif_init();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        return ret;
    }

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    if (sta_netif == NULL) {
        return ESP_FAIL;
    }

    ret = esp_netif_set_hostname(sta_netif, DEVICE_HOSTNAME);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = configure_static_ipv4(sta_netif);
    if (ret != ESP_OK) {
        return ret;
    }

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&wifi_init_config);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        &network_event_handler, NULL, NULL);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        &network_event_handler, NULL, NULL);
    if (ret != ESP_OK) {
        return ret;
    }

    wifi_config_t wifi_config = {0};

    strlcpy((char *)wifi_config.sta.ssid,
            WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password,
            WIFI_PASS, sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG,
             "Wi-Fi STA started: SSID=%s hostname=%s static_ip=%s",
             WIFI_SSID, DEVICE_HOSTNAME, STATIC_IP_TEXT);

    return ESP_OK;
}

bool network_is_connected(void)
{
    return s_wifi_connected;
}

const char *network_get_ip(void)
{
    return s_current_ip;
}
