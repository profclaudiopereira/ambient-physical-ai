#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the StickC Plus 2 visual output controller.
 *
 * The controller owns all hardware-specific responsibilities:
 *
 * - initialization of M5Unified;
 * - initialization of the integrated display;
 * - initialization of the external WS2812 output;
 * - GPIO 32 configuration;
 * - control of eight RGB LEDs;
 * - synchronized display and LED updates.
 *
 * Initialization leaves both the display and the external RGB bar off.
 * No cognitive state is presented until an explicit command is received.
 *
 * Higher architectural layers must not depend directly on M5Unified,
 * the WS2812 driver, GPIO assignments or the physical LED count.
 *
 * @return 0 when initialization succeeds.
 * @return Non-zero ESP-IDF error code when initialization fails.
 */
int rgb_controller_init(void);

/**
 * @brief Presents one Cognitive Runtime state.
 *
 * The same RGB value is applied to all external LEDs and to the display
 * background. The display also presents the supplied state label.
 *
 * The controller treats the label as presentation data. It does not interpret
 * Cognitive Runtime semantics or select colors.
 *
 * @param label Null-terminated label displayed on the StickC Plus 2.
 * @param red Red-channel intensity from 0 to 255.
 * @param green Green-channel intensity from 0 to 255.
 * @param blue Blue-channel intensity from 0 to 255.
 *
 * @return 0 when the state presentation is updated successfully.
 * @return Non-zero ESP-IDF error code when an argument is invalid or a
 *         hardware operation fails.
 */
int rgb_controller_present_state(
    const char *label,
    uint8_t red,
    uint8_t green,
    uint8_t blue
);

/**
 * @brief Applies one RGB color to all LEDs and to the display background.
 *
 * This lower-level operation is retained for reusable primitive effects.
 * It does not present a Cognitive Runtime state label.
 *
 * @param red Red-channel intensity from 0 to 255.
 * @param green Green-channel intensity from 0 to 255.
 * @param blue Blue-channel intensity from 0 to 255.
 *
 * @return 0 when the output is updated successfully.
 * @return Non-zero ESP-IDF error code when an operation fails.
 */
int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
);

/**
 * @brief Clears the WS2812 output and turns the display black.
 *
 * @return 0 when the output is cleared successfully.
 * @return Non-zero ESP-IDF error code when an operation fails.
 */
int rgb_controller_clear(void);

#ifdef __cplusplus
}
#endif