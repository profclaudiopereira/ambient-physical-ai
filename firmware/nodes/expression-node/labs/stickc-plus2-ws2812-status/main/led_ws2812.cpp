#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "M5Unified.h"

// ================= CONFIG =================
#define LED_GPIO 32
#define LED_COUNT 8
#define BRIGHTNESS 64  // 0–255 (~25% recomendado)

// ================= HANDLE =================
led_strip_handle_t strip;

// ================= UTILS =================
static inline uint8_t scale(uint8_t v) {
    return (v * BRIGHTNESS) / 255;
}

// ================= LED =================
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

// ================= DISPLAY =================
void update_display(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = M5.Display.color565(r, g, b);

    // pinta tela inteira
    M5.Display.fillScreen(color);

    // texto de debug
    M5.Display.setTextColor(WHITE, color);
    M5.Display.setCursor(10, 10);
    M5.Display.setTextSize(2);
    M5.Display.printf("R:%3d\nG:%3d\nB:%3d", r, g, b);
}

// ================= CONTROLE =================
void show_color(uint8_t r, uint8_t g, uint8_t b)
{
    set_all(r, g, b);
    update_display(r, g, b);
}

// ================= MAIN =================
extern "C" void app_main(void)
{
    // Inicializa M5 (display, botões, etc)
    M5.begin();

    M5.Display.setRotation(1);

    // Config LED Strip
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

    // Loop principal
    while (1) {

        show_color(255, 0, 0);    // vermelho
        vTaskDelay(pdMS_TO_TICKS(1000));

        show_color(0, 255, 0);    // verde
        vTaskDelay(pdMS_TO_TICKS(1000));

        show_color(0, 0, 255);    // azul
        vTaskDelay(pdMS_TO_TICKS(1000));

        show_color(255, 255, 255); // branco (stress)
        vTaskDelay(pdMS_TO_TICKS(1000));
	show_color(255, 255, 0);   // amarelo
	vTaskDelay(pdMS_TO_TICKS(1000));
	show_color(0, 255, 255);   // ciano
	vTaskDelay(pdMS_TO_TICKS(1000));
	show_color(255, 0, 255);   // magenta
	vTaskDelay(pdMS_TO_TICKS(1000));
	show_color(255, 128, 0);   // laranja
	vTaskDelay(pdMS_TO_TICKS(1000));



    }
}