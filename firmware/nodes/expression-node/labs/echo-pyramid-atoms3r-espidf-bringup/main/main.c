#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_log.h"

#define I2C_PORT        I2C_NUM_0
#define I2C_SDA_GPIO    38
#define I2C_SCL_GPIO    39
#define I2C_FREQ_HZ     100000

#define STM32_I2C_ADDR  0x1A

#define RGB1_BRIGHTNESS_REG_ADDR  0x10
#define RGB2_BRIGHTNESS_REG_ADDR  0x11
#define RGB1_STATUS_REG_ADDR      0x20
#define RGB2_STATUS_REG_ADDR      0x60

static const char *TAG = "expression_e04";

static i2c_master_bus_handle_t i2c_bus = NULL;

typedef struct {
    const char *type;
    const char *event;
    const char *target;
    const char *message;
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

static void expression_rgb_welcome(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_WELCOME");

    esp_err_t ret;

    ret = set_brightness(1, 100);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RGB channel 1 brightness: %s", esp_err_to_name(ret));
        return;
    }

    ret = set_brightness(2, 100);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RGB channel 2 brightness: %s", esp_err_to_name(ret));
        return;
    }

    ret = set_rgb(1, 0, 0, 255, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RGB CH1 LED0: %s", esp_err_to_name(ret));
        return;
    }

    ret = set_rgb(2, 0, 0, 255, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RGB CH2 LED0: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "RGB welcome reaction sent to Echo Pyramid.");
}

static void handle_semantic_event(const semantic_event_t *event)
{
    ESP_LOGI(TAG, "Semantic Event received");
    ESP_LOGI(TAG, "type: %s", event->type);
    ESP_LOGI(TAG, "event: %s", event->event);
    ESP_LOGI(TAG, "target: %s", event->target);
    ESP_LOGI(TAG, "message: %s", event->message);

    if (strcmp(event->event, "welcome_researcher") == 0) {
        expression_rgb_welcome();
    } else {
        ESP_LOGW(TAG, "Unknown semantic event");
    }
}

void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Ambient Physical AI");
    ESP_LOGI(TAG, "Expression Layer - E04 Semantic Event RGB");
    ESP_LOGI(TAG, "Hardware: Echo Pyramid + AtomS3R");
    ESP_LOGI(TAG, "Target: ESP32-S3");
    ESP_LOGI(TAG, "Purpose: Semantic Event to RGB reaction");
    ESP_LOGI(TAG, "========================================");

    ESP_LOGI(TAG, "Chip cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);

    ESP_ERROR_CHECK(init_i2c_bus());

    semantic_event_t welcome_event = {
        .type = "semantic_event",
        .event = "welcome_researcher",
        .target = "expression_layer",
        .message = "Welcome to the Ambient Physical AI Research Laboratory"
    };

    int counter = 0;

    while (true) {
        ESP_LOGI(TAG, "Expression node heartbeat: %d", counter++);

        if (counter == 3) {
            handle_semantic_event(&welcome_event);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}