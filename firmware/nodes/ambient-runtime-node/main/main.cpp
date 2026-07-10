#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_err.h"
#include "esp_log.h"

#include "ambient_console.h"
#include "dlight.h"
#include "env_iv.h"
#include "oled_sh1107.h"
#include "pahub.h"
#include "tab5_platform.h"

static const char *TAG = "ambient-runtime";

static constexpr uint8_t PAHUB_CHANNEL_ENV_IV = 0;
static constexpr uint8_t PAHUB_CHANNEL_OLED   = 1;
static constexpr uint8_t PAHUB_CHANNEL_DLIGHT = 2;

static constexpr TickType_t RUNTIME_UPDATE_INTERVAL =
    pdMS_TO_TICKS(3000);

extern "C" void app_main(void)
{
    ESP_LOGI(
        TAG,
        "Ambient Physical AI - Ambient Runtime Console"
    );

    /*
     * ---------------------------------------------------------
     * 1. Tab5 platform and primary console
     * ---------------------------------------------------------
     */
    ESP_ERROR_CHECK(tab5_platform_init());
    ESP_ERROR_CHECK(ambient_console_init());
    ESP_ERROR_CHECK(tab5_platform_backlight_set(100));

    i2c_master_bus_handle_t bus =
        tab5_platform_get_port_a_i2c_bus();

    if (bus == nullptr) {
        ESP_LOGE(TAG, "PORT A I2C bus is not available");
        return;
    }

    /*
     * ---------------------------------------------------------
     * 2. Mini OLED initialization — PaHub channel 1
     * ---------------------------------------------------------
     */
    bool mini_oled_ok = false;

    esp_err_t ret = pahub_select_channel(
        bus,
        PAHUB_CHANNEL_OLED
    );

    if (ret == ESP_OK) {
        ret = oled_sh1107_init(bus);
    }

    if (ret == ESP_OK) {
        ret = oled_sh1107_print_ambient_test();
    }

    if (ret == ESP_OK) {
        mini_oled_ok = true;
        ESP_LOGI(TAG, "Mini OLED initialized");
    } else {
        ESP_LOGW(
            TAG,
            "Mini OLED initialization failed: %s",
            esp_err_to_name(ret)
        );
    }

    /*
     * ---------------------------------------------------------
     * 3. Ambient Runtime loop
     * ---------------------------------------------------------
     */
    while (true) {
        env_iv_sht40_data_t sht40 = {};
        env_iv_bmp280_data_t bmp280 = {};

        float lux = 0.0f;

        bool pahub_ok = true;
        bool env_iv_ok = false;
        bool dlight_ok = false;

        /*
         * ENV-IV — PaHub channel 0
         */
        esp_err_t channel_ret = pahub_select_channel(
            bus,
            PAHUB_CHANNEL_ENV_IV
        );

        esp_err_t sht_ret = ESP_FAIL;
        esp_err_t bmp_ret = ESP_FAIL;

        if (channel_ret == ESP_OK) {
            sht_ret = env_iv_sht40_read(
                bus,
                &sht40
            );

            bmp_ret = env_iv_bmp280_read(
                bus,
                &bmp280
            );
        } else {
            pahub_ok = false;

            ESP_LOGW(
                TAG,
                "Unable to select ENV-IV channel: %s",
                esp_err_to_name(channel_ret)
            );
        }

        if (sht_ret == ESP_OK) {
            ESP_LOGI(
                TAG,
                "SHT40 Temp: %.2f C  Hum: %.2f %%",
                sht40.temperature_c,
                sht40.humidity_percent
            );
        } else {
            ESP_LOGW(
                TAG,
                "SHT40 read failed: %s",
                esp_err_to_name(sht_ret)
            );
        }

        if (bmp_ret == ESP_OK) {
            ESP_LOGI(
                TAG,
                "BMP280 Temp: %.2f C  Press: %.2f hPa",
                bmp280.temperature_c,
                bmp280.pressure_hpa
            );
        } else {
            ESP_LOGW(
                TAG,
                "BMP280 read failed: %s",
                esp_err_to_name(bmp_ret)
            );
        }

        env_iv_ok =
            (sht_ret == ESP_OK) &&
            (bmp_ret == ESP_OK);

        /*
         * DLight — PaHub channel 2
         */
        channel_ret = pahub_select_channel(
            bus,
            PAHUB_CHANNEL_DLIGHT
        );

        esp_err_t lux_ret = ESP_FAIL;

        if (channel_ret == ESP_OK) {
            lux_ret = dlight_read_lux(
                bus,
                &lux
            );
        } else {
            pahub_ok = false;

            ESP_LOGW(
                TAG,
                "Unable to select DLight channel: %s",
                esp_err_to_name(channel_ret)
            );
        }

        if (lux_ret == ESP_OK) {
            dlight_ok = true;

            ESP_LOGI(
                TAG,
                "DLight Lux: %.2f lx",
                lux
            );
        } else {
            ESP_LOGW(
                TAG,
                "DLight read failed: %s",
                esp_err_to_name(lux_ret)
            );
        }

        /*
         * Verifica novamente o canal do Mini OLED.
         */
        channel_ret = pahub_select_channel(
            bus,
            PAHUB_CHANNEL_OLED
        );

        if (channel_ret != ESP_OK) {
            pahub_ok = false;
            mini_oled_ok = false;

            ESP_LOGW(
                TAG,
                "Unable to select Mini OLED channel: %s",
                esp_err_to_name(channel_ret)
            );
        }

        /*
         * -----------------------------------------------------
         * Ambient Runtime Console
         *
         * Wi-Fi permanece explicitamente PENDING.
         * A integração será retomada em laboratório separado.
         * -----------------------------------------------------
         */
        ambient_console_data_t console = {};

        console.temperature_c =
            sht40.temperature_c;

        console.humidity_percent =
            sht40.humidity_percent;

        console.pressure_hpa =
            bmp280.pressure_hpa;

        console.light_lux =
            lux;

        console.wifi_connected =
            false;

        console.cognitive_connected =
            false;

        console.pahub_ok =
            pahub_ok;

        console.env_iv_ok =
            env_iv_ok;

        console.dlight_ok =
            dlight_ok;

        console.mini_oled_ok =
            mini_oled_ok;

        ret = ambient_console_render(
            &console
        );

        if (ret != ESP_OK) {
            ESP_LOGE(
                TAG,
                "Ambient Console render failed: %s",
                esp_err_to_name(ret)
            );
        }

        ESP_LOGI(
            TAG,
            "Ambient Runtime alive | "
            "Platform=OK | I2C=%s | ENV-IV=%s | "
            "DLight=%s | OLED=%s | Wi-Fi=PENDING",
            pahub_ok ? "OK" : "ERROR",
            env_iv_ok ? "OK" : "ERROR",
            dlight_ok ? "OK" : "ERROR",
            mini_oled_ok ? "OK" : "ERROR"
        );

        vTaskDelay(RUNTIME_UPDATE_INTERVAL);
    }
}