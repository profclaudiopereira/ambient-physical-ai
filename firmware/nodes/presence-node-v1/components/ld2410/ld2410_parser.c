#include "ld2410_internal.h"

/**
 * @file ld2410_parser.c
 * @brief LD2410 UART frame parser.
 *
 * Implements the byte-oriented state machine responsible for assembling,
 * validating and decoding normal data frames received from the LD2410 radar.
 * The parser is intentionally stateless outside the parser context so it can
 * be safely reused by higher driver layers.
 */

#include <string.h>

/**
 * @brief Fixed synchronization sequences defined by the LD2410 protocol.
 *
 * Every normal frame begins with FRAME_HEADER and terminates with
 * FRAME_FOOTER. These signatures are used to synchronize the parser after
 * communication errors or byte loss.
 */
static const uint8_t FRAME_HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
static const uint8_t FRAME_FOOTER[4] = {0xF8, 0xF7, 0xF6, 0xF5};

/**
 * @brief Reads a 16-bit little-endian value from two protocol bytes.
 *
 * The LD2410 protocol transmits multi-byte numeric fields using little-endian
 * byte ordering.
 */
static uint16_t read_u16_le(uint8_t low, uint8_t high)
{
    return (uint16_t)low | ((uint16_t)high << 8);
}

/**
 * @brief Verifies the structural integrity of one normal protocol frame.
 *
 * Validation includes synchronization bytes, payload length, protocol
 * markers and frame footer before any application data is decoded.
 */
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

/**
 * @brief Converts a validated protocol frame into the public driver format.
 *
 * This function translates raw protocol fields into the canonical
 * ld2410_target_data_t structure used by the rest of the application.
 */
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

/**
 * @brief Resets the parser to its initial synchronization state.
 *
 * Any partially received frame is discarded and the next incoming byte will
 * be interpreted as the beginning of a new synchronization sequence.
 */
void ld2410_parser_reset(ld2410_parser_t *parser)
{
    if (parser != NULL) {
        parser->length = 0;
    }
}

/**
 * @brief Incrementally processes one received UART byte.
 *
 * The parser operates as a streaming state machine. Bytes are accumulated
 * until an entire normal frame has been received. A decoded target report is
 * produced only after the complete frame passes protocol validation.
 *
 * The parser automatically resynchronizes when synchronization bytes are
 * lost or corrupted, avoiding the propagation of invalid frames.
 *
 * @param parser Parser instance.
 * @param byte Newly received UART byte.
 * @param out_data Receives decoded target information.
 *
 * @return true when a complete valid frame has been decoded.
 */
bool ld2410_parser_push_byte(
    ld2410_parser_t *parser,
    uint8_t byte,
    ld2410_target_data_t *out_data)
{
    if (parser == NULL || out_data == NULL) {
        return false;
    }

    /*
    * Synchronization state:
    *
    * Frame acquisition starts only after the first synchronization byte is
    * detected. All preceding bytes are ignored, allowing recovery from UART
    * noise or arbitrary stream alignment.
    */
    if (parser->length == 0) {
        if (byte == FRAME_HEADER[0]) {
            parser->bytes[0] = byte;
            parser->length = 1;
        }
        return false;
    }
    
    /*
    * Header verification.
    *
    * Every subsequent synchronization byte must match the expected header
    * sequence. Any mismatch immediately resets the parser so synchronization
    * can restart without buffering invalid data.
    */
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


    /*
    * After header synchronization, all remaining bytes are copied directly into
    * the frame buffer until the expected protocol frame length is reached.
    */
    parser->bytes[parser->length++] = byte;

    if (parser->length < LD2410_NORMAL_FRAME_SIZE) {
        return false;
    }
    
    /*
    * Complete frame received.
    *
    * Decode the frame, then immediately reset the parser so the next UART byte
    * begins a new acquisition cycle regardless of the decoding result.
    */
    const bool decoded = decode_normal_frame(parser->bytes, out_data);
    ld2410_parser_reset(parser);
    return decoded;
}
