/******************************************************************************
 * @file    runtime_uart.c
 * @brief   Receives newline-delimited runtime status JSON through UART.
 *
 * This module is responsible only for transport concerns:
 *  - Configuring the UART peripheral.
 *  - Reconstructing one line-delimited JSON document.
 *  - Invoking the runtime parser.
 *  - Publishing valid snapshots through the UI Manager API.
 *
 * JSON interpretation remains owned by runtime_status.c, while screen updates
 * remain owned by ui_manager.c.
 ******************************************************************************/

#include "runtime_uart.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "runtime_uart_config.h"

#include "runtime/runtime_status.h"
#include "ui/ui_manager.h"

static const char *TAG = "runtime_uart";

/**
 * @brief Processes one complete null-terminated JSON document.
 */
static void process_runtime_document(
    runtime_status_t *status,
    const char *json)
{
    if (!runtime_status_apply_json(status, json)) {
        ESP_LOGW(TAG, "Rejected JSON document");
        return;
    }

    if (!ui_manager_update_runtime_status(status)) {
        ESP_LOGE(TAG, "Unable to publish runtime snapshot");
        return;
    }

    ESP_LOGI(
        TAG,
        "Runtime update applied: state=%s user=%s",
        status->cognitive_state,
        status->user);
}

/**
 * @brief FreeRTOS task responsible for UART reception and line reconstruction.
 *
 * Input is accumulated until a newline is received. Carriage returns are
 * ignored so both LF and CRLF senders are accepted.
 *
 * Oversized documents are discarded completely to prevent applying truncated
 * JSON data.
 */
static void runtime_uart_task(void *argument)
{
    (void)argument;

    runtime_status_t runtime_status;

    if (!runtime_status_init_mock(&runtime_status)) {
        ESP_LOGE(TAG, "Unable to initialize receiver status model");
        vTaskDelete(NULL);
        return;
    }

    uint8_t receive_buffer[128];
    char json_buffer[RUNTIME_JSON_BUFFER_SIZE];

    size_t json_length = 0;
    bool discard_until_newline = false;

    ESP_LOGI(
        TAG,
        "Receiver active on UART%d, RX GPIO%d, %d bit/s",
        RUNTIME_UART_PORT,
        RUNTIME_UART_RX_GPIO,
        RUNTIME_UART_BAUD_RATE);

    while (true) {
        const int received = uart_read_bytes(
            RUNTIME_UART_PORT,
            receive_buffer,
            sizeof(receive_buffer),
            pdMS_TO_TICKS(100));

        if (received <= 0) {
            continue;
        }

        for (int index = 0; index < received; ++index) {
            const char character = (char)receive_buffer[index];

            if (character == '\r') {
                continue;
            }

            if (character == '\n') {
                if (discard_until_newline) {
                    ESP_LOGW(TAG, "Oversized JSON document discarded");
                } else if (json_length > 0) {
                    json_buffer[json_length] = '\0';

                    process_runtime_document(
                        &runtime_status,
                        json_buffer);
                }

                json_length = 0;
                discard_until_newline = false;
                continue;
            }

            if (discard_until_newline) {
                continue;
            }

            if (json_length >= (sizeof(json_buffer) - 1)) {
                json_length = 0;
                discard_until_newline = true;
                continue;
            }

            json_buffer[json_length++] = character;
        }
    }
}

bool runtime_uart_start(void)
{
    const uart_config_t uart_config = {
        .baud_rate = RUNTIME_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t result = uart_param_config(
        RUNTIME_UART_PORT,
        &uart_config);

    if (result != ESP_OK) {
        ESP_LOGE(
            TAG,
            "uart_param_config failed: %s",
            esp_err_to_name(result));
        return false;
    }

    result = uart_set_pin(
        RUNTIME_UART_PORT,
        RUNTIME_UART_TX_GPIO,
        RUNTIME_UART_RX_GPIO,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE);

    if (result != ESP_OK) {
        ESP_LOGE(
            TAG,
            "uart_set_pin failed: %s",
            esp_err_to_name(result));
        return false;
    }

    result = uart_driver_install(
        RUNTIME_UART_PORT,
        RUNTIME_UART_DRIVER_BUFFER,
        0,
        0,
        NULL,
        0);

    if (result != ESP_OK) {
        ESP_LOGE(
            TAG,
            "uart_driver_install failed: %s",
            esp_err_to_name(result));
        return false;
    }

    const BaseType_t task_created = xTaskCreate(
        runtime_uart_task,
        "runtime_uart",
        RUNTIME_UART_TASK_STACK_SIZE,
        NULL,
        RUNTIME_UART_TASK_PRIORITY,
        NULL);

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Unable to create UART receiver task");
        uart_driver_delete(RUNTIME_UART_PORT);
        return false;
    }

    return true;
}