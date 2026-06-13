#include "env_iv.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SHT40_ADDR 0x44

esp_err_t env_iv_sht40_read(
    i2c_master_bus_handle_t bus,
    env_iv_sht40_data_t *data
)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_dev_handle_t dev_handle;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT40_ADDR,
        .scl_speed_hz = 100000,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t cmd = 0xFD; // SHT40 high precision measurement

    ret = i2c_master_transmit(dev_handle, &cmd, 1, 100);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t raw[6] = {0};

    ret = i2c_master_receive(dev_handle, raw, sizeof(raw), 100);
    i2c_master_bus_rm_device(dev_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t raw_temp = ((uint16_t)raw[0] << 8) | raw[1];
    uint16_t raw_hum  = ((uint16_t)raw[3] << 8) | raw[4];

    data->temperature_c =
        -45.0f + 175.0f * ((float)raw_temp / 65535.0f);

    data->humidity_percent =
        -6.0f + 125.0f * ((float)raw_hum / 65535.0f);

    if (data->humidity_percent < 0.0f) {
        data->humidity_percent = 0.0f;
    }

    if (data->humidity_percent > 100.0f) {
        data->humidity_percent = 100.0f;
    }

    return ESP_OK;
}