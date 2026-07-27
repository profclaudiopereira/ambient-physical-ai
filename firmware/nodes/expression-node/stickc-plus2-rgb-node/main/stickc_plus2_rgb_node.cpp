#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "expression_processor.h"
#include "rgb_controller.h"
#include "semantic_consumer.h"
#include "semantic_receiver.h"
#include "wifi_station.h"

static const char *TAG = "stickc_plus2_rgb_node";

/**
 * @brief Initializes and maintains the StickC Plus 2 RGB expression node.
 *
 * Initialization order follows the validated production RGB node contract:
 *
 * 1. initialize the physical expression controller;
 * 2. present the boot state;
 * 3. initialize the fixed-IP Wi-Fi station;
 * 4. wait for network availability;
 * 5. present the idle state;
 * 6. start the semantic-event UDP receiver.
 */
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting StickC Plus 2 RGB Node");

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

    ESP_LOGI(TAG, "StickC Plus 2 RGB Node initialized");

    while (!wifi_station_is_connected()) {
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Network ready");

    expression_processor_process("idle");

    if (semantic_consumer_init() != 0) {
        ESP_LOGE(
            TAG,
            "Runtime State consumer initialization failed"
        );

    expression_processor_process("error");
    return;
}


    if (semantic_receiver_start() != 0) {
        ESP_LOGE(TAG, "Semantic Receiver failed");
        expression_processor_process("error");
        return;
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}