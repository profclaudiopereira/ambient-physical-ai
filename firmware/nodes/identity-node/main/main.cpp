#include "M5Unified.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp/m5dial.h"
#include "iot_knob.h"

static const char *TAG = "identity-node";

extern "C" void app_main(void)
{
    auto cfg = M5.config();
    M5.begin(cfg);

    ESP_LOGI(TAG, "Identity Console V1");
    ESP_LOGI(TAG, "Initializing M5Dial");

    M5.Display.setTextSize(2);
    M5.Display.fillScreen(BLACK);

    M5.Display.setCursor(20, 40);
    M5.Display.println("Ambient");

    M5.Display.setCursor(20, 70);
    M5.Display.println("Physical AI");

    M5.Display.setCursor(20, 120);
    M5.Display.println("Identity");

    M5.Display.setCursor(20, 150);
    M5.Display.println("Console V1");

    M5.Display.setCursor(20, 200);
    M5.Display.println("READY");

    M5.Speaker.tone(2000, 150);

    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = BSP_ENCODER_A,
        .gpio_encoder_b = BSP_ENCODER_B,
        .enable_power_save = false,
    };

    knob_handle_t knob = iot_knob_create(&knob_cfg);

    if (knob == NULL) {
        ESP_LOGE(TAG, "Failed to create knob encoder");
        M5.Display.setCursor(20, 220);
        M5.Display.println("ENC ERR");
    } else {
        ESP_LOGI(TAG, "Knob encoder initialized");
        M5.Display.setCursor(20, 220);
        M5.Display.println("ENC: 0");
    }

    int last_encoder_count = 0;

    while (true)
    {
        M5.update();

        if (knob != NULL) {
            int encoder_count = iot_knob_get_count_value(knob);

            if (encoder_count != last_encoder_count) {
                ESP_LOGI(TAG, "Encoder: %d", encoder_count);

                M5.Display.fillRect(0, 220, 240, 30, BLACK);
                M5.Display.setCursor(20, 220);
                M5.Display.printf("ENC: %d", encoder_count);

                last_encoder_count = encoder_count;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}