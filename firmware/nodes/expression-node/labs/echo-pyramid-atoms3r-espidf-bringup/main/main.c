#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_chip_info.h"

static const char *TAG = "expression_e01";

void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Ambient Physical AI");
    ESP_LOGI(TAG, "Expression Layer - E01 ESP-IDF Bring-up");
    ESP_LOGI(TAG, "Hardware: Echo Pyramid + AtomS3R");
    ESP_LOGI(TAG, "Target: ESP32-S3");
    ESP_LOGI(TAG, "Purpose: boot + serial baseline");
    ESP_LOGI(TAG, "========================================");

    ESP_LOGI(TAG, "Chip cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);
    ESP_LOGI(TAG, "ESP-IDF boot baseline running");

    int counter = 0;

    while (true) {
        ESP_LOGI(TAG, "Expression node heartbeat: %d", counter++);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}