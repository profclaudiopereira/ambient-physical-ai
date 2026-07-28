/**
 * @file main.c
 * @brief Echo Pyramid + AtomS3R Expression Node application.
 *
 * The validated RGB, UDP and personalized welcome contracts are preserved.
 * Local Status Display V1 adds BOOT, READY and personalized WELCOME states
 * without changing Cognitive Runtime or StackFlow contracts.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "display_manager.h"
#include "network_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_log.h"

#define UDP_PORT        5005
#define UDP_BUFFER_SIZE 128
#define WELCOME_PREFIX  "WELCOME|"
#define USER_NAME_SIZE  64

#define I2C_PORT        I2C_NUM_0
#define I2C_SDA_GPIO    38
#define I2C_SCL_GPIO    39
#define I2C_FREQ_HZ     100000

#define STM32_I2C_ADDR  0x1A

#define RGB1_BRIGHTNESS_REG_ADDR 0x10
#define RGB2_BRIGHTNESS_REG_ADDR 0x11
#define RGB1_STATUS_REG_ADDR     0x20
#define RGB2_STATUS_REG_ADDR     0x60

#define READY_WAIT_INTERVAL_MS   100
#define READY_WAIT_TIMEOUT_MS    15000
#define WELCOME_DISPLAY_MS       3000

static const char *TAG = "echo_pyramid_voice";
static i2c_master_bus_handle_t i2c_bus = NULL;

typedef struct {
    const char *type;
    const char *event;
    const char *target;
    const char *message;
    const char *parameter;
} semantic_event_t;

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
    esp_err_t ret =
        i2c_master_bus_add_device(i2c_bus, &dev_cfg, &dev_handle);

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

static esp_err_t write_register(uint8_t dev_addr,
                                uint8_t reg_addr,
                                uint8_t value)
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
    uint8_t data[4] = {blue, green, red, 0x00};

    return write_registers(
        STM32_I2C_ADDR, reg_addr, data, sizeof(data));
}

static void set_all_demo_leds(uint8_t red,
                              uint8_t green,
                              uint8_t blue)
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

static void show_ready_screen(void)
{
    esp_err_t err = display_show_ready(network_get_ip());
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Unable to show READY screen: %s",
                 esp_err_to_name(err));
    }
}

static void handle_semantic_event(const semantic_event_t *event)
{
    ESP_LOGI(TAG, "Semantic Event received");
    ESP_LOGI(TAG, "type: %s", event->type);
    ESP_LOGI(TAG, "event: %s", event->event);
    ESP_LOGI(TAG, "target: %s", event->target);
    ESP_LOGI(TAG, "message: %s", event->message);

    if (event->parameter != NULL && event->parameter[0] != '\0') {
        ESP_LOGI(TAG, "parameter: %s", event->parameter);
    }

    if (strcmp(event->event, "welcome_researcher") == 0) {
        const char *user =
            (event->parameter != NULL && event->parameter[0] != '\0')
                ? event->parameter : "USER";

        expression_rgb_welcome();

        esp_err_t err = display_show_welcome(user);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Unable to show WELCOME screen: %s",
                     esp_err_to_name(err));
        }

        /*
         * V1 deliberately keeps this short synchronous pause. UDP reception
         * resumes after three seconds and the node returns to READY. A future
         * voice-state manager can replace this with an asynchronous UI task.
         */
        vTaskDelay(pdMS_TO_TICKS(WELCOME_DISPLAY_MS));
        show_ready_screen();

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

    if (strcmp(msg, "WELCOME") == 0 ||
        strncmp(msg, WELCOME_PREFIX, strlen(WELCOME_PREFIX)) == 0) {

        char user_name[USER_NAME_SIZE] = {0};
        const char *parameter = NULL;

        if (strncmp(msg, WELCOME_PREFIX,
                    strlen(WELCOME_PREFIX)) == 0) {
            const char *received_name =
                msg + strlen(WELCOME_PREFIX);
            size_t received_length =
                strnlen(received_name, USER_NAME_SIZE);

            if (received_length > 0 &&
                received_length < USER_NAME_SIZE) {
                memcpy(user_name, received_name, received_length);
                user_name[received_length] = '\0';
                parameter = user_name;
            } else if (received_length >= USER_NAME_SIZE) {
                ESP_LOGW(TAG,
                         "Welcome parameter exceeds supported size; "
                         "using legacy behavior");
            }
        }

        semantic_event_t event = {
            .type = "semantic_event",
            .event = "welcome_researcher",
            .target = "expression_layer",
            .message = (parameter != NULL)
                           ? "Personalized welcome event received via UDP"
                           : "Welcome event received via UDP",
            .parameter = parameter
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "RED") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_red",
            .target = "expression_layer",
            .message = "Red event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "GREEN") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_green",
            .target = "expression_layer",
            .message = "Green event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "BLUE") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_blue",
            .target = "expression_layer",
            .message = "Blue event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "OFF") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_off",
            .target = "expression_layer",
            .message = "Off event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else {
        ESP_LOGW(TAG, "Unknown UDP command: %s", msg);
    }
}

static void udp_server_task(void *pv_parameters)
{
    (void)pv_parameters;

    char rx_buffer[UDP_BUFFER_SIZE];

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket");
        display_show_error("UDP SOCKET");
        vTaskDelete(NULL);
        return;
    }

    int err = bind(
        sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (err < 0) {
        ESP_LOGE(TAG, "UDP socket bind failed");
        display_show_error("UDP BIND");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "UDP server listening on port %d", UDP_PORT);
    show_ready_screen();

    while (true) {
        struct sockaddr_in source_addr;
        socklen_t source_addr_len = sizeof(source_addr);

        int len = recvfrom(
            sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
            (struct sockaddr *)&source_addr, &source_addr_len);

        if (len < 0) {
            ESP_LOGE(TAG, "UDP recvfrom failed");
            continue;
        }

        rx_buffer[len] = '\0';
        handle_udp_message(rx_buffer);
    }
}

void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Ambient Physical AI");
    ESP_LOGI(TAG, "Expression Layer - Echo Pyramid Voice Node");
    ESP_LOGI(TAG, "Hardware: Echo Pyramid + AtomS3R");
    ESP_LOGI(TAG, "Firmware stage: Local Status Display V1");
    ESP_LOGI(TAG, "========================================");

    ESP_LOGI(TAG, "Chip cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);

    ESP_ERROR_CHECK(init_i2c_bus());
    ESP_ERROR_CHECK(display_manager_init());
    ESP_ERROR_CHECK(display_show_boot());
    ESP_ERROR_CHECK(network_init());

    int waited_ms = 0;
    while (!network_is_connected() &&
           waited_ms < READY_WAIT_TIMEOUT_MS) {
        vTaskDelay(pdMS_TO_TICKS(READY_WAIT_INTERVAL_MS));
        waited_ms += READY_WAIT_INTERVAL_MS;
    }

    if (!network_is_connected()) {
        ESP_LOGW(TAG,
                 "Wi-Fi not connected after %d ms; UDP will still start",
                 READY_WAIT_TIMEOUT_MS);
        ESP_ERROR_CHECK(display_show_error("WI-FI TIMEOUT"));
    }

    BaseType_t task_created = xTaskCreate(
        udp_server_task,
        "udp_server_task",
        4096,
        NULL,
        5,
        NULL);

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UDP server task");
        ESP_ERROR_CHECK(display_show_error("UDP TASK"));
        return;
    }

    int counter = 0;

    while (true) {
        ESP_LOGI(TAG,
                 "Expression node heartbeat: %d | Wi-Fi: %s | "
                 "IP: %s | UDP port: %d",
                 counter++,
                 network_is_connected() ? "connected" : "not connected",
                 network_get_ip(),
                 UDP_PORT);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
