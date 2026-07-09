#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "env_iv.h"
#include "oled_sh1107.h"
#include "dlight.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "tab5_platform.h"
#include "pahub.h"

extern "C" void app_main(void)
{
    ESP_LOGI("ambient-runtime", "Ambient Runtime - Tab5 Platform + PaHub Validation");

    ESP_ERROR_CHECK(tab5_platform_init());

    i2c_master_bus_handle_t bus = tab5_platform_get_port_a_i2c_bus();

    env_iv_sht40_data_t sht40 = {};
    env_iv_bmp280_data_t bmp280 = {};

    ESP_ERROR_CHECK(pahub_select_channel(bus, 0));

    esp_err_t sht_ret = env_iv_sht40_read(bus, &sht40);
    if (sht_ret == ESP_OK) {
        ESP_LOGI("ambient-runtime", "SHT40 Temp: %.2f C  Hum: %.2f %%", sht40.temperature_c, sht40.humidity_percent);
    } else {
        ESP_LOGW("ambient-runtime", "SHT40 read failed: %s", esp_err_to_name(sht_ret));
    }

    esp_err_t bmp_ret = env_iv_bmp280_read(bus, &bmp280);
    if (bmp_ret == ESP_OK) {
        ESP_LOGI("ambient-runtime", "BMP280 Temp: %.2f C  Press: %.2f hPa", bmp280.temperature_c, bmp280.pressure_hpa);
    } else {
        ESP_LOGW("ambient-runtime", "BMP280 read failed: %s", esp_err_to_name(bmp_ret));
    }

    ESP_ERROR_CHECK(pahub_select_channel(bus, 1));
    ESP_ERROR_CHECK(oled_sh1107_init(bus));
    ESP_ERROR_CHECK(oled_sh1107_print_ambient_test());

    ESP_ERROR_CHECK(tab5_platform_fill(0xF800));
    ESP_ERROR_CHECK(tab5_platform_backlight_set(100));

ESP_ERROR_CHECK(pahub_select_channel(bus, 2));

float lux = 0.0f;
esp_err_t lux_ret = dlight_read_lux(bus, &lux);

if (lux_ret == ESP_OK) {
    ESP_LOGI("ambient-runtime", "DLight Lux: %.2f lx", lux);
} else {
    ESP_LOGW("ambient-runtime", "DLight read failed: %s", esp_err_to_name(lux_ret));
}

    while (true) {
        ESP_LOGI("ambient-runtime", "Tab5 Platform + PaHub + ENV-IV + OLED Alive");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}