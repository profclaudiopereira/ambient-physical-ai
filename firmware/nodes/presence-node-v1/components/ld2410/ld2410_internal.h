#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ld2410.h"

#define LD2410_NORMAL_FRAME_SIZE 23

typedef struct {
    uint8_t bytes[LD2410_NORMAL_FRAME_SIZE];
    size_t length;
} ld2410_parser_t;

struct ld2410_driver {
    ld2410_config_t config;
    ld2410_parser_t parser;
};

void ld2410_parser_reset(ld2410_parser_t *parser);

bool ld2410_parser_push_byte(
    ld2410_parser_t *parser,
    uint8_t byte,
    ld2410_target_data_t *out_data
);
