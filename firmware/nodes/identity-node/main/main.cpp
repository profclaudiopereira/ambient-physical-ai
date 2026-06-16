#include "M5Unified.h"
#include "esp_log.h"

static const char *TAG = "identity-node";

extern "C" void app_main(void)
{
    auto cfg = M5.config();
    M5.begin(cfg);

    ESP_LOGI(TAG, "Identity Console V1");
    ESP_LOGI(TAG, "Initializing M5Dial");

    // Display
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

    // Buzzer
    M5.Speaker.tone(2000, 150);

    while (true)
    {
        M5.update();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}