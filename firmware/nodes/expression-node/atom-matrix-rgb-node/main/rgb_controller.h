#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the integrated Atom Matrix RGB controller.
 *
 * Hardware configuration:
 * - WS2812 addressable LEDs
 * - 5 x 5 integrated matrix
 * - GPIO 27
 * - 25 pixels
 * - Software-limited brightness
 *
 * @return 0 on success.
 * @return Non-zero error code on failure.
 */
int rgb_controller_init(void);

/**
 * @brief Applies one color uniformly to all pixels of the RGB matrix.
 *
 * The controller applies the configured brightness limit before sending the
 * values to the physical LEDs.
 *
 * @param red   Red channel intensity from 0 to 255.
 * @param green Green channel intensity from 0 to 255.
 * @param blue  Blue channel intensity from 0 to 255.
 *
 * @return 0 on success.
 * @return Non-zero error code on failure.
 */
int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
);

/**
 * @brief Turns off every pixel of the integrated RGB matrix.
 *
 * @return 0 on success.
 * @return Non-zero error code on failure.
 */
int rgb_controller_clear(void);

#ifdef __cplusplus
}
#endif