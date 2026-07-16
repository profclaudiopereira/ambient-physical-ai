#include "ld2410_internal.h"

#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "ld2410";
#define LD2410_READ_CHUNK_SIZE 128

esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle)
{
    if (config == NULL || out_handle == NULL ||
        config->rx_buffer_size <= 0 || config->baud_rate <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    struct ld2410_driver *driver = calloc(1, sizeof(*driver));
    if (driver == NULL) {
        return ESP_ERR_NO_MEM;
    }

    driver->config = *config;
    ld2410_parser_reset(&driver->parser);

    const uart_config_t uart_config = {
        .baud_rate = config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

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

esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks)
{
    if (handle == NULL || out_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buffer[LD2410_READ_CHUNK_SIZE];
    const TickType_t start = xTaskGetTickCount();

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

esp_err_t ld2410_delete(ld2410_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_err_t err = uart_driver_delete(handle->config.uart_port);
    free(handle);
    return err;
}

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
