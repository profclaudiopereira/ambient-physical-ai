#include "ld2410_internal.h"
#include <string.h>

static const uint8_t FRAME_HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
static const uint8_t FRAME_FOOTER[4] = {0xF8, 0xF7, 0xF6, 0xF5};

static uint16_t read_u16_le(uint8_t low, uint8_t high)
{
    return (uint16_t)low | ((uint16_t)high << 8);
}

static bool validate_normal_frame(const uint8_t *frame)
{
    if (memcmp(frame, FRAME_HEADER, sizeof(FRAME_HEADER)) != 0) {
        return false;
    }

    if (read_u16_le(frame[4], frame[5]) != 13) {
        return false;
    }

    if (frame[6] != 0x02 || frame[7] != 0xAA) {
        return false;
    }

    if (frame[17] != 0x55) {
        return false;
    }

    return memcmp(&frame[19], FRAME_FOOTER, sizeof(FRAME_FOOTER)) == 0;
}

static bool decode_normal_frame(
    const uint8_t *frame,
    ld2410_target_data_t *out_data)
{
    if (!validate_normal_frame(frame)) {
        return false;
    }

    const uint8_t raw_state = frame[8];
    out_data->state =
        raw_state <= LD2410_TARGET_MOVING_AND_STATIONARY
            ? (ld2410_target_state_t)raw_state
            : LD2410_TARGET_UNKNOWN;

    out_data->moving_distance_cm = read_u16_le(frame[9], frame[10]);
    out_data->moving_energy = frame[11];
    out_data->stationary_distance_cm = read_u16_le(frame[12], frame[13]);
    out_data->stationary_energy = frame[14];
    out_data->detection_distance_cm = read_u16_le(frame[15], frame[16]);

    return true;
}

void ld2410_parser_reset(ld2410_parser_t *parser)
{
    if (parser != NULL) {
        parser->length = 0;
    }
}

bool ld2410_parser_push_byte(
    ld2410_parser_t *parser,
    uint8_t byte,
    ld2410_target_data_t *out_data)
{
    if (parser == NULL || out_data == NULL) {
        return false;
    }

    if (parser->length == 0) {
        if (byte == FRAME_HEADER[0]) {
            parser->bytes[0] = byte;
            parser->length = 1;
        }
        return false;
    }

    if (parser->length < sizeof(FRAME_HEADER)) {
        const size_t expected = parser->length;

        if (byte == FRAME_HEADER[expected]) {
            parser->bytes[parser->length++] = byte;
            return false;
        }

        ld2410_parser_reset(parser);

        if (byte == FRAME_HEADER[0]) {
            parser->bytes[0] = byte;
            parser->length = 1;
        }

        return false;
    }

    parser->bytes[parser->length++] = byte;

    if (parser->length < LD2410_NORMAL_FRAME_SIZE) {
        return false;
    }

    const bool decoded = decode_normal_frame(parser->bytes, out_data);
    ld2410_parser_reset(parser);
    return decoded;
}
