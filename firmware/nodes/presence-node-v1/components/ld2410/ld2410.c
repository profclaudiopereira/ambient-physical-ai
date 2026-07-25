#include "ld2410_internal.h"

/**
 * @file ld2410.c
 * @brief Public driver implementation for the LD2410 radar.
 *
 * This module owns the UART driver lifecycle and bridges the byte-oriented
 * parser with the application-facing API defined in ld2410.h.
 */

#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "ld2410";

/**
 * @brief Maximum number of UART bytes processed per read operation.
 *
 * Reading data in small chunks limits stack usage while still providing
 * efficient throughput for continuous radar reports.
 */
#define LD2410_READ_CHUNK_SIZE 128

/**
 * @brief Creates and initializes one LD2410 driver instance.
 *
 * The initialization sequence allocates the driver object, configures the
 * UART peripheral and prepares the parser state before exposing the handle
 * to the application.
 */
esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle)
{
    if (config == NULL || out_handle == NULL ||
        config->rx_buffer_size <= 0 || config->baud_rate <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    /*
    * Allocate a zero-initialized driver instance so every internal field starts
    * from a known state before hardware initialization begins.
    */
    struct ld2410_driver *driver = calloc(1, sizeof(*driver));
    if (driver == NULL) {
        return ESP_ERR_NO_MEM;
    }

    driver->config = *config;
    ld2410_parser_reset(&driver->parser);
    
    /*
    * Configure the UART peripheral according to the hardware requirements of
    * the connected LD2410 radar module.
    */
    const uart_config_t uart_config = {
        .baud_rate = config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    /*
    * Hardware initialization is performed incrementally. Any failure triggers
    * immediate cleanup so partially initialized resources are never leaked.
    */
    esp_err_t err = uart_driver_install(
        config->uart_port,
        config->rx_buffer_size,
        0,
        0,
        NULL,
        0
    );

    if (err != ESP_OK) {
        free(driver);
        return err;
    }

    err = uart_param_config(config->uart_port, &uart_config);
    if (err != ESP_OK) {
        uart_driver_delete(config->uart_port);
        free(driver);
        return err;
    }

    err = uart_set_pin(
        config->uart_port,
        config->tx_gpio,
        config->rx_gpio,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    );

    if (err != ESP_OK) {
        uart_driver_delete(config->uart_port);
        free(driver);
        return err;
    }

    ESP_LOGI(
        TAG,
        "UART%d baud=%d TX=GPIO%d RX=GPIO%d",
        config->uart_port,
        config->baud_rate,
        config->tx_gpio,
        config->rx_gpio
    );

    *out_handle = driver;
    return ESP_OK;
}

/**
 * @brief Reads radar data until one complete target report is decoded.
 *
 * UART bytes are continuously fed into the parser. The function returns as
 * soon as one valid protocol frame has been decoded or the timeout expires.
 */
esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks)
{
    if (handle == NULL || out_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    /*
    * A temporary stack buffer is used to minimize UART driver calls while
    * preserving incremental protocol decoding performed by the parser.
    */
    uint8_t buffer[LD2410_READ_CHUNK_SIZE];
    const TickType_t start = xTaskGetTickCount();
    
    /*
    * Continue reading until either a complete protocol frame is decoded or the
    * caller-defined timeout is reached.
    */
    while ((xTaskGetTickCount() - start) < timeout_ticks) {
        const TickType_t elapsed = xTaskGetTickCount() - start;
        const TickType_t remaining =
            timeout_ticks > elapsed ? timeout_ticks - elapsed : 0;

        const int received = uart_read_bytes(
            handle->config.uart_port,
            buffer,
            sizeof(buffer),
            remaining
        );

        if (received <= 0) {
            break;
        }
        /*
        * Feed every received UART byte into the streaming parser. Parsing remains
        * incremental regardless of how the UART driver groups incoming bytes.
        */
        for (int i = 0; i < received; ++i) {
            if (ld2410_parser_push_byte(
                    &handle->parser,
                    buffer[i],
                    out_data)) {
                return ESP_OK;
            }
        }
    }

    return ESP_ERR_TIMEOUT;
}

/**
 * @brief Releases the UART driver and all resources owned by the instance.
 *
 * After this function returns, the supplied driver handle becomes invalid.
 */
esp_err_t ld2410_delete(ld2410_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_err_t err = uart_driver_delete(handle->config.uart_port);
    free(handle);
    return err;
}

/**
 * @brief Converts a target state into a human-readable string.
 *
 * Intended for diagnostics, logging and debugging output.
 */
const char *ld2410_target_state_to_string(
    ld2410_target_state_t state)
{
    switch (state) {
        case LD2410_TARGET_NONE:
            return "NONE";
        case LD2410_TARGET_MOVING:
            return "MOVING";
        case LD2410_TARGET_STATIONARY:
            return "STATIONARY";
        case LD2410_TARGET_MOVING_AND_STATIONARY:
            return "MOVING_AND_STATIONARY";
        default:
            return "UNKNOWN";
    }
}
