#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "expression_processor.h"
#include "rgb_controller.h"
#include "semantic_receiver.h"
#include "wifi_station.h"

static const char *TAG = "atom_matrix_rgb_node";

/**
 * @brief Initializes the Atom Matrix RGB production node.
 *
 * The boot sequence intentionally follows the validated Expression Layer
 * architecture:
 *
 * 1. Initialize the physical RGB controller.
 * 2. Present the local boot expression.
 * 3. Initialize and connect the Wi-Fi station.
 * 4. Transition to the idle expression.
 * 5. Start receiving normalized Semantic Events over UDP.
 *
 * Cognitive reasoning is not performed locally. The node only translates
 * normalized Semantic Events into deterministic visual expressions.
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Atom Matrix RGB Node");

    if (rgb_controller_init() != 0) {
        ESP_LOGE(
            TAG,
            "RGB matrix controller initialization failed"
        );
        return;
    }

    expression_processor_process("boot", NULL);

    if (wifi_station_init() != 0) {
        ESP_LOGE(
            TAG,
            "Wi-Fi initialization failed"
        );

        expression_processor_process("system_error", NULL);
        return;
    }

    ESP_LOGI(TAG, "Atom Matrix RGB Node initialized");

    while (!wifi_station_is_connected()) {
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Network ready");

    expression_processor_process("idle", NULL);

    if (semantic_receiver_start() != 0) {
        ESP_LOGE(TAG, "Semantic Receiver failed");
        expression_processor_process("system_error", NULL);
        return;
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
