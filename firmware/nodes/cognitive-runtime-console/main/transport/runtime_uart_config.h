/******************************************************************************
 * @file    runtime_uart_config.h
 * @brief   Hardware configuration for the Cognitive Runtime UART transport.
 *
 * This file centralizes the UART peripheral, GPIO mapping and communication
 * parameters used by the CoreS3 Lite console.
 *
 * The selected pins avoid the USB programming UART and preserve Port A for
 * future I2C peripherals.
 ******************************************************************************/

#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"

#define RUNTIME_UART_PORT             UART_NUM_1

/*
 * CoreS3 Lite M-BUS GPIO mapping:
 *
 * GPIO17 is used as the console receive pin.
 * GPIO18 is reserved as the console transmit pin for future bidirectional
 * communication, although the first validation uses receive only.
 */
#define RUNTIME_UART_RX_GPIO          GPIO_NUM_18
#define RUNTIME_UART_TX_GPIO          GPIO_NUM_17

#define RUNTIME_UART_BAUD_RATE        115200

#define RUNTIME_UART_DRIVER_BUFFER    2048
#define RUNTIME_JSON_BUFFER_SIZE      2048
#define RUNTIME_UART_TASK_STACK_SIZE  6144
#define RUNTIME_UART_TASK_PRIORITY    5