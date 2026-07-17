#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_strip.h"
#include "esp_log.h"

#define RGB_GPIO 2
#define RGB_COUNT 29

static const char *TAG = "NEOPIXEL";

static led_strip_handle_t strip;

static void set_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < RGB_COUNT; i++)
    {
        led_strip_set_pixel(strip, i, r, g, b);
    }

    led_strip_refresh(strip);
}

void app_main(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_GPIO,
        .max_leds = RGB_COUNT,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10000000,
    };

    ESP_ERROR_CHECK(
        led_strip_new_rmt_device(
            &strip_config,
            &rmt_config,
            &strip));

    ESP_LOGI(TAG, "TESTE NEOPIXEL M5STACK");

    while (1)
    {
        ESP_LOGI(TAG, "VERMELHO");
        set_all(8, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));

        ESP_LOGI(TAG, "VERDE");
        set_all(0, 8, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));

        ESP_LOGI(TAG, "AZUL");
        set_all(0, 0, 8);
        vTaskDelay(pdMS_TO_TICKS(5000));

        ESP_LOGI(TAG, "BRANCO");
        set_all(8, 8, 8);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}