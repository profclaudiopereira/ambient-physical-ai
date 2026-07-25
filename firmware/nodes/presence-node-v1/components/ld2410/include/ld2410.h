#pragma once

/**
 * @file ld2410.h
 * @brief Public interface for the LD2410 radar driver.
 *
 * This header defines the public API, configuration structures and shared
 * data types used by the Presence Node to communicate with the LD2410 radar.
 * Internal implementation details remain private to the driver component.
 */

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Canonical target states reported by the LD2410 driver.
 *
 * These values abstract the sensor protocol into a stable
 * application-facing representation.
 */
typedef enum {
    LD2410_TARGET_NONE = 0x00,
    LD2410_TARGET_MOVING = 0x01,
    LD2410_TARGET_STATIONARY = 0x02,
    LD2410_TARGET_MOVING_AND_STATIONARY = 0x03,
    LD2410_TARGET_UNKNOWN = 0xFF
} ld2410_target_state_t;

/**
 * @brief Decoded target information returned by the driver.
 *
 * All distance values are expressed in centimeters exactly as interpreted
 * from the LD2410 report frame. The structure contains the most recent
 * successfully decoded observation.
 */
typedef struct {
    ld2410_target_state_t state;
    uint16_t moving_distance_cm;
    uint8_t moving_energy;
    uint16_t stationary_distance_cm;
    uint8_t stationary_energy;
    uint16_t detection_distance_cm;
} ld2410_target_data_t;

/**
 * @brief Driver configuration supplied during initialization.
 *
 * The application is responsible for providing UART parameters compatible
 * with the connected LD2410 hardware and the board pin assignment.
 */
typedef struct {
    uart_port_t uart_port;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
    int baud_rate;
    int rx_buffer_size;
} ld2410_config_t;

/**
 * @brief Opaque handle representing one driver instance.
 *
 * The internal driver structure is intentionally hidden to preserve
 * encapsulation and allow implementation changes without affecting users
 * of the public API.
 */

typedef struct ld2410_driver *ld2410_handle_t;

/**
 * @brief Default configuration suitable for the Presence Node baseline.
 *
 * Projects targeting different hardware layouts may override any field
 * before creating the driver instance.
 */
#define LD2410_CONFIG_DEFAULT() \
    {                           \
        .uart_port = UART_NUM_1,\
        .tx_gpio = GPIO_NUM_1,  \
        .rx_gpio = GPIO_NUM_2,  \
        .baud_rate = 256000,    \
        .rx_buffer_size = 2048  \
    }

/**
 * @brief Creates and initializes an LD2410 driver instance.
 *
 * @param config Driver configuration.
 * @param out_handle Receives the created driver handle on success.
 *
 * @return ESP_OK on successful initialization or an ESP-IDF error code.
 */
esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle
);

/**
 * @brief Reads and decodes the next radar report.
 *
 * The function waits for a complete protocol frame, decodes its contents
 * and stores the normalized result in the output structure.
 *
 * @param handle Driver instance.
 * @param out_data Receives the decoded target information.
 * @param timeout_ticks Maximum wait time for a complete frame.
 *
 * @return ESP_OK on success or an ESP-IDF error code.
 */
esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks
);

/**
 * @brief Releases all resources owned by the driver instance.
 *
 * After this call the handle becomes invalid and shall not be reused.
 */
esp_err_t ld2410_delete(ld2410_handle_t handle);

/**
 * @brief Returns a human-readable representation of a target state.
 *
 * Intended primarily for logging, diagnostics and debugging output.
 */
const char *ld2410_target_state_to_string(
    ld2410_target_state_t state
);

#ifdef __cplusplus
}
#endif
