#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LD2410_TARGET_NONE = 0x00,
    LD2410_TARGET_MOVING = 0x01,
    LD2410_TARGET_STATIONARY = 0x02,
    LD2410_TARGET_MOVING_AND_STATIONARY = 0x03,
    LD2410_TARGET_UNKNOWN = 0xFF
} ld2410_target_state_t;

typedef struct {
    ld2410_target_state_t state;
    uint16_t moving_distance_cm;
    uint8_t moving_energy;
    uint16_t stationary_distance_cm;
    uint8_t stationary_energy;
    uint16_t detection_distance_cm;
} ld2410_target_data_t;

typedef struct {
    uart_port_t uart_port;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
    int baud_rate;
    int rx_buffer_size;
} ld2410_config_t;

typedef struct ld2410_driver *ld2410_handle_t;

#define LD2410_CONFIG_DEFAULT() \
    {                           \
        .uart_port = UART_NUM_1,\
        .tx_gpio = GPIO_NUM_1,  \
        .rx_gpio = GPIO_NUM_2,  \
        .baud_rate = 256000,    \
        .rx_buffer_size = 2048  \
    }

esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle
);

esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks
);

esp_err_t ld2410_delete(ld2410_handle_t handle);

const char *ld2410_target_state_to_string(
    ld2410_target_state_t state
);

#ifdef __cplusplus
}
#endif
