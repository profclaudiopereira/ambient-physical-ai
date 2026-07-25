#pragma once

/**
 * @file ld2410_internal.h
 * @brief Internal definitions for the LD2410 driver implementation.
 *
 * This header contains private data structures and parser interfaces shared
 * only by the driver source files. It is not intended to be included by
 * application code.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ld2410.h"

/**
 * @brief Size, in bytes, of one normal data frame produced by the LD2410.
 *
 * The parser expects complete frames of this length before attempting
 * protocol decoding.
 */
#define LD2410_NORMAL_FRAME_SIZE 23

/**
 * @brief Incremental parser state used while assembling UART frames.
 *
 * Bytes received from the radar are accumulated until a complete protocol
 * frame becomes available for decoding.
 */
typedef struct {
    uint8_t bytes[LD2410_NORMAL_FRAME_SIZE];
    size_t length;
} ld2410_parser_t;

/**
 * @brief Private driver instance.
 *
 * Stores the runtime configuration and parser state associated with one
 * LD2410 driver object.
 */
struct ld2410_driver {
    ld2410_config_t config;
    ld2410_parser_t parser;
};

/**
 * @brief Resets the parser state.
 *
 * Clears any partially received frame so decoding can restart from the next
 * incoming byte stream.
 */
void ld2410_parser_reset(ld2410_parser_t *parser);

/**
 * @brief Feeds one byte into the parser state machine.
 *
 * The parser consumes the incoming UART stream one byte at a time and
 * returns true only when a complete and valid frame has been decoded.
 *
 * @param parser Parser instance.
 * @param byte Newly received UART byte.
 * @param out_data Receives decoded target information when a frame is completed.
 *
 * @return true if a complete frame was decoded, otherwise false.
 */
bool ld2410_parser_push_byte(
    ld2410_parser_t *parser,
    uint8_t byte,
    ld2410_target_data_t *out_data
);
