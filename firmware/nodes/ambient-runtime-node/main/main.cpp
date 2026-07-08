#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "tab5_platform.h"

extern "C" void app_main(void)
{
    ESP_LOGI("ambient-runtime", "Ambient Runtime - Tab5 Platform Validation");

    ESP_ERROR_CHECK(tab5_platform_init());

    ESP_ERROR_CHECK(tab5_platform_fill(0xF800));

    ESP_ERROR_CHECK(tab5_platform_backlight_set(100));

    while (true) {
        ESP_LOGI("ambient-runtime", "Tab5 Platform Alive");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}