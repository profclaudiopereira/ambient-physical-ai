#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

#include "env_iv.h"
#include "oled_sh1107.h"

#define I2C_SCL_GPIO 54
#define I2C_SDA_GPIO 53
#define I2C_PORT     0

#define PAHUB_ADDR   0x70

static const char *TAG = "ambient-runtime";

static i2c_master_bus_handle_t i2c_bus = NULL;

static env_iv_sht40_data_t env_data;
static env_iv_bmp280_data_t bmp_data;

static void i2c_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    ESP_LOGI(
        TAG,
        "I2C initialized: SDA=GPIO%d, SCL=GPIO%d",
        I2C_SDA_GPIO,
        I2C_SCL_GPIO
    );
}

static void pahub_select_channel(uint8_t channel)
{
    if (channel > 7) {
        ESP_LOGE(TAG, "Invalid PaHub channel: %d", channel);
        return;
    }

    uint8_t data = 1 << channel;

    i2c_master_dev_handle_t dev_handle;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PAHUB_ADDR,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &dev_cfg, &dev_handle));
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &data, 1, 100));
    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));

    vTaskDelay(pdMS_TO_TICKS(20));

    ESP_LOGI(TAG, "PaHub channel %d selected", channel);
}
static void i2c_scan(void)
{
    ESP_LOGI(TAG, "Scanning I2C bus...");

    for (uint8_t addr = 1; addr < 127; addr++) {
        esp_err_t ret =
            i2c_master_probe(
                i2c_bus,
                addr,
                100);

        if (ret == ESP_OK) {
            ESP_LOGI(
                TAG,
                "Found I2C device at address: 0x%02X",
                addr);
        }
    }

    ESP_LOGI(TAG, "I2C scan complete.");
}
void app_main(void)
{
    printf("\n");
    printf("=====================================\n");
    printf(" Ambient Runtime Node\n");
    printf(" M004.6 - ENV-IV Full Readings\n");
    printf("=====================================\n");

    i2c_init();

    // Initial OLED validation
    pahub_select_channel(1);
    oled_sh1107_init(i2c_bus);
    oled_sh1107_print_test();


while (1) {

    pahub_select_channel(1);

    pahub_select_channel(1);
    //oled_sh1107_print_test();
    //oled_sh1107_print_ok();
oled_sh1107_print_ambient_test();

    vTaskDelay(pdMS_TO_TICKS(5000));
}
}