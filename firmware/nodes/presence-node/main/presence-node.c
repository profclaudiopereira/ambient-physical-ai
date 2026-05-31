#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_log.h"
#include "esp_err.h"

#include "vl53l0x.h"

#define I2C_MASTER_SCL_IO           15
#define I2C_MASTER_SDA_IO           13
#define I2C_MASTER_PORT             I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000

static const char *TAG = "presence-node";

void app_main(void)
{
    ESP_LOGI(TAG, "Presence Node v0.3 - VL53L0X Test");
    ESP_LOGI(TAG, "Initializing I2C bus...");

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;

    esp_err_t err = i2c_new_master_bus(
        &bus_config,
        &bus_handle
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to initialize I2C bus: %s",
            esp_err_to_name(err)
        );
        return;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = VL53L0X_I2C_ADDRESS,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    i2c_master_dev_handle_t tof_dev = NULL;

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_cfg,
            &tof_dev
        )
    );

    while (1)
    {
        err = vl53l0x_is_alive(tof_dev);

        if (err == ESP_OK)
        {
            ESP_LOGI(
                TAG,
                "VL53L0X detected"
            );
        }
        else
        {
            ESP_LOGW(
                TAG,
                "VL53L0X not detected: %s",
                esp_err_to_name(err)
            );
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}