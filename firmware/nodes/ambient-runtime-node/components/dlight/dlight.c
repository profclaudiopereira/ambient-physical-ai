#include "dlight.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DLIGHT_ADDR 0x23

esp_err_t dlight_read_lux(
    i2c_master_bus_handle_t bus,
    float *lux
)
{
    if (lux == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_dev_handle_t dev_handle;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DLIGHT_ADDR,
        .scl_speed_hz = 100000,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t cmd = 0x10; // Continuously H-Resolution Mode

    ret = i2c_master_transmit(dev_handle, &cmd, 1, 100);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(180));

    uint8_t data[2] = {0};

    ret = i2c_master_receive(dev_handle, data, 2, 100);

    i2c_master_bus_rm_device(dev_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t raw = ((uint16_t)data[0] << 8) | data[1];

    *lux = raw / 1.2f;

    return ESP_OK;
}