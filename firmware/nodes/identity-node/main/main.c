#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "identity-node";

void app_main(void)
{
    ESP_LOGI(TAG, "Identity Node v0.1 - M5Dial base firmware");
    ESP_LOGI(TAG, "Target: M5Dial V1.1");
    ESP_LOGI(TAG, "Scope: Presence -> Identity -> Context Package");

    while (1) {
        ESP_LOGI(TAG, "Identity Node alive");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}