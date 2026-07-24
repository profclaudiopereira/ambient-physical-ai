/******************************************************************************
 * @file    runtime_uart.h
 * @brief   UART transport interface for Cognitive Runtime status documents.
 *
 * The transport receives newline-delimited JSON documents from the AX630C and
 * publishes valid runtime snapshots to the console UI.
 ******************************************************************************/

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the Cognitive Runtime UART receiver.
 *
 * Configures the UART peripheral and creates the FreeRTOS reception task.
 *
 * @return true when initialization succeeds; false otherwise.
 */
bool runtime_uart_start(void);

#ifdef __cplusplus
}
#endif