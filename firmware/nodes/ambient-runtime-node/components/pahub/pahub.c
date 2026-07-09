#include "pahub.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#define PAHUB_ADDR 0x70

static const char *TAG = "pahub";

esp_err_t pahub_select_channel(
    i2c_master_bus_handle_t bus,
    uint8_t channel
)
{
    if (channel > 7) {
        ESP_LOGE(TAG, "Invalid PaHub channel: %d", channel);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data = 1 << channel;

    i2c_master_dev_handle_t dev_handle;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PAHUB_ADDR,
        .scl_speed_hz = 100000,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = i2c_master_transmit(dev_handle, &data, 1, 100);

    i2c_master_bus_rm_device(dev_handle);

    if (ret == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(20));
        ESP_LOGI(TAG, "PaHub channel %d selected", channel);
    }

    return ret;
}