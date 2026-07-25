#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the StickC Plus 2 RGB output controller.
 *
 * The controller owns all hardware-specific responsibilities:
 *
 * - initialization of M5Unified;
 * - initialization of the integrated display;
 * - initialization of the external WS2812 output;
 * - GPIO 32 configuration;
 * - control of eight RGB LEDs;
 * - synchronized display and LED color updates.
 *
 * Higher architectural layers must not depend directly on M5Unified,
 * the WS2812 driver, GPIO assignments or the physical LED count.
 *
 * @return 0 when initialization succeeds.
 * @return Non-zero ESP-IDF error code when initialization fails.
 */
int rgb_controller_init(void);

/**
 * @brief Applies one RGB color to all LEDs and to the device display.
 *
 * The function is the hardware boundary used by the visual-effects layer.
 * All physical LEDs receive the same RGB value, and the display is updated
 * to represent the commanded output.
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