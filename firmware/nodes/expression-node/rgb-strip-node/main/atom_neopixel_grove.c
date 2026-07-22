#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "expression_processor.h"
#include "rgb_controller.h"
#include "semantic_receiver.h"
#include "wifi_station.h"

static const char *TAG = "rgb_strip_node";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting RGB Strip Node");

    if (rgb_controller_init() != 0) {
        ESP_LOGE(
            TAG,
            "RGB controller initialization failed"
        );
        return;
    }

    expression_processor_process("boot");

    if (wifi_station_init() != 0) {
        ESP_LOGE(
            TAG,
            "Wi-Fi initialization failed"
        );

        expression_processor_process("system_error");
        return;
    }

    ESP_LOGI(TAG, "RGB Strip Node initialized");

    while (!wifi_station_is_connected()) {
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Network ready");

    expression_processor_process("idle");

    if (semantic_receiver_start() != 0) {
        ESP_LOGE(TAG, "Semantic Receiver failed");
        expression_processor_process("system_error");
        return;
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}