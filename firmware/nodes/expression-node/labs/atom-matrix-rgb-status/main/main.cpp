#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

// Atom Matrix usa GPIO 27 para os LEDs
#define LED_GPIO 27
#define LED_COUNT 25
#define BRIGHTNESS 64

led_strip_handle_t strip;

static inline uint8_t scale(uint8_t v) {
    return (v * BRIGHTNESS) / 255;
}

void set_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < LED_COUNT; i++) {
        led_strip_set_pixel(strip, i,
                            scale(r),
                            scale(g),
                            scale(b));
    }
    led_strip_refresh(strip);
}

extern "C" void app_main(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_COUNT,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
    ESP_ERROR_CHECK(led_strip_clear(strip));

    vTaskDelay(pdMS_TO_TICKS(50));

    while (1) {

        set_all(255, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        set_all(0, 255, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        set_all(0, 0, 255);
        vTaskDelay(pdMS_TO_TICKS(1000));

        set_all(255, 255, 255);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}