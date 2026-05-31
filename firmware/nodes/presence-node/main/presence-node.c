#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO           2
#define I2C_MASTER_SDA_IO           1
#define I2C_MASTER_PORT             I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          50000
#define I2C_SCAN_TIMEOUT_MS         50

static const char *TAG = "presence-node";

void app_main(void)
{
    ESP_LOGI(TAG, "Presence Node v0.2 - I2C Scanner");
    ESP_LOGI(TAG, "Initializing I2C bus...");

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    i2c_master_bus_handle_t bus_handle;

    esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(err));
        return;
    }

    while (1) {
    ESP_LOGI(TAG, "Probing VL53L0X at address 0x29...");

    err = i2c_master_probe(bus_handle, 0x29, 1000);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "VL53L0X detected at 0x29");
    } else {
        ESP_LOGW(TAG, "VL53L0X not detected: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(3000));
}
}