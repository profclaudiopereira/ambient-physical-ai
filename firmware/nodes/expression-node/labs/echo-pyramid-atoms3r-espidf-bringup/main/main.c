#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"


/*
 * Update the Wi-Fi credentials below before building.
 * If static IP addressing is enabled, also adjust the network
 * configuration (IP, gateway and subnet mask) for your environment.
 */
#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASS "<YOUR_WIFI_PASSWORD>"

#define UDP_PORT 5005
#define UDP_BUFFER_SIZE 128

#define I2C_PORT     I2C_NUM_0
#define I2C_SDA_GPIO 38
#define I2C_SCL_GPIO 39
#define I2C_FREQ_HZ  100000

#define STM32_I2C_ADDR 0x1A

#define RGB1_BRIGHTNESS_REG_ADDR 0x10
#define RGB2_BRIGHTNESS_REG_ADDR 0x11
#define RGB1_STATUS_REG_ADDR     0x20
#define RGB2_STATUS_REG_ADDR     0x60

static const char *TAG = "expression_e05_2";

static i2c_master_bus_handle_t i2c_bus = NULL;
static volatile bool wifi_connected = false;

typedef struct {
    const char *type;
    const char *event;
    const char *target;
    const char *message;
} semantic_event_t;

/* ================= Wi-Fi ================= */

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

static void wifi_init_sta(void)
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
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {0};

    strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA started. SSID: %s", WIFI_SSID);
}

/* ================= I2C / RGB ================= */

static esp_err_t init_i2c_bus(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&bus_config, &i2c_bus);
}

static esp_err_t write_registers(uint8_t dev_addr,
                                 uint8_t reg_addr,
                                 const uint8_t *data,
                                 size_t len)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    i2c_master_dev_handle_t dev_handle = NULL;

    esp_err_t ret = i2c_master_bus_add_device(i2c_bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t buffer[16] = {0};

    if (len + 1 > sizeof(buffer)) {
        i2c_master_bus_rm_device(dev_handle);
        return ESP_ERR_INVALID_SIZE;
    }

    buffer[0] = reg_addr;
    memcpy(&buffer[1], data, len);

    ret = i2c_master_transmit(dev_handle, buffer, len + 1, 100);

    i2c_master_bus_rm_device(dev_handle);

    return ret;
}

static esp_err_t write_register(uint8_t dev_addr, uint8_t reg_addr, uint8_t value)
{
    return write_registers(dev_addr, reg_addr, &value, 1);
}

static esp_err_t set_brightness(uint8_t channel, uint8_t value)
{
    if (value > 100) {
        value = 100;
    }

    uint8_t reg_addr = (channel == 1)
                           ? RGB1_BRIGHTNESS_REG_ADDR
                           : RGB2_BRIGHTNESS_REG_ADDR;

    return write_register(STM32_I2C_ADDR, reg_addr, value);
}

static esp_err_t set_rgb(uint8_t channel,
                         uint8_t led_index,
                         uint8_t red,
                         uint8_t green,
                         uint8_t blue)
{
    if (led_index > 13) {
        led_index = 13;
    }

    uint8_t base_reg = (channel == 1)
                           ? RGB1_STATUS_REG_ADDR
                           : RGB2_STATUS_REG_ADDR;

    uint8_t reg_addr = base_reg + (led_index * 4);

    uint8_t data[4] = {
        blue,
        green,
        red,
        0x00
    };

    return write_registers(STM32_I2C_ADDR, reg_addr, data, sizeof(data));
}

static void set_all_demo_leds(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(set_brightness(1, 100));
    ESP_ERROR_CHECK(set_brightness(2, 100));

    ESP_ERROR_CHECK(set_rgb(1, 0, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 3, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 7, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 10, red, green, blue));

    ESP_ERROR_CHECK(set_rgb(2, 0, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 3, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 7, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 10, red, green, blue));
}

static void expression_rgb_welcome(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_WELCOME");
    set_all_demo_leds(0, 255, 0);
    ESP_LOGI(TAG, "RGB welcome reaction sent to Echo Pyramid.");
}

static void expression_rgb_red(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_RED");
    set_all_demo_leds(255, 0, 0);
}

static void expression_rgb_green(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_GREEN");
    set_all_demo_leds(0, 255, 0);
}

static void expression_rgb_blue(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_BLUE");
    set_all_demo_leds(0, 0, 255);
}

static void expression_rgb_off(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_OFF");
    set_all_demo_leds(0, 0, 0);
}

/* ================= Semantic Event ================= */

static void handle_semantic_event(const semantic_event_t *event)
{
    ESP_LOGI(TAG, "Semantic Event received");
    ESP_LOGI(TAG, "type: %s", event->type);
    ESP_LOGI(TAG, "event: %s", event->event);
    ESP_LOGI(TAG, "target: %s", event->target);
    ESP_LOGI(TAG, "message: %s", event->message);

    if (strcmp(event->event, "welcome_researcher") == 0) {
        expression_rgb_welcome();
    } else if (strcmp(event->event, "rgb_red") == 0) {
        expression_rgb_red();
    } else if (strcmp(event->event, "rgb_green") == 0) {
        expression_rgb_green();
    } else if (strcmp(event->event, "rgb_blue") == 0) {
        expression_rgb_blue();
    } else if (strcmp(event->event, "rgb_off") == 0) {
        expression_rgb_off();
    } else {
        ESP_LOGW(TAG, "Unknown semantic event: %s", event->event);
    }
}

static void handle_udp_message(const char *msg)
{
    ESP_LOGI(TAG, "UDP message received: %s", msg);

    if (strcmp(msg, "WELCOME") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "welcome_researcher",
            .target = "expression_layer",
            .message = "Welcome event received via UDP"
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "RED") == 0) {
        semantic_event_t event = {"semantic_event", "rgb_red", "expression_layer", "Red event received via UDP"};
        handle_semantic_event(&event);

    } else if (strcmp(msg, "GREEN") == 0) {
        semantic_event_t event = {"semantic_event", "rgb_green", "expression_layer", "Green event received via UDP"};
        handle_semantic_event(&event);

    } else if (strcmp(msg, "BLUE") == 0) {
        semantic_event_t event = {"semantic_event", "rgb_blue", "expression_layer", "Blue event received via UDP"};
        handle_semantic_event(&event);

    } else if (strcmp(msg, "OFF") == 0) {
        semantic_event_t event = {"semantic_event", "rgb_off", "expression_layer", "Off event received via UDP"};
        handle_semantic_event(&event);

    } else {
        ESP_LOGW(TAG, "Unknown UDP command: %s", msg);
    }
}

/* ================= UDP Task ================= */

static void udp_server_task(void *pvParameters)
{
    (void)pvParameters;

    char rx_buffer[UDP_BUFFER_SIZE];

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket");
        vTaskDelete(NULL);
        return;
    }

    int err = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (err < 0) {
        ESP_LOGE(TAG, "UDP socket bind failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "UDP server listening on port %d", UDP_PORT);

    while (true) {
        struct sockaddr_in source_addr;
        socklen_t socklen = sizeof(source_addr);

        int len = recvfrom(sock,
                           rx_buffer,
                           sizeof(rx_buffer) - 1,
                           0,
                           (struct sockaddr *)&source_addr,
                           &socklen);

        if (len < 0) {
            ESP_LOGE(TAG, "UDP recvfrom failed");
            continue;
        }

        rx_buffer[len] = 0;
        handle_udp_message(rx_buffer);
    }
}

/* ================= Main ================= */

void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Ambient Physical AI");
    ESP_LOGI(TAG, "Expression Layer - E05.2 UDP Semantic Event Reception");
    ESP_LOGI(TAG, "Hardware: Echo Pyramid + AtomS3R");
    ESP_LOGI(TAG, "Target: ESP32-S3");
    ESP_LOGI(TAG, "Purpose: UDP message to Semantic Event to RGB");
    ESP_LOGI(TAG, "========================================");

    ESP_LOGI(TAG, "Chip cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);

    ESP_ERROR_CHECK(init_i2c_bus());

    wifi_init_sta();

    xTaskCreate(
        udp_server_task,
        "udp_server_task",
        4096,
        NULL,
        5,
        NULL
    );

    int counter = 0;

    while (true) {
        ESP_LOGI(TAG,
                 "Expression node heartbeat: %d | Wi-Fi: %s | UDP port: %d",
                 counter++,
                 wifi_connected ? "connected" : "not connected",
                 UDP_PORT);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}