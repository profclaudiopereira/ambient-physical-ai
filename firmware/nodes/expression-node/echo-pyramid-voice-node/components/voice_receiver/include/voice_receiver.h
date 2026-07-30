/**
 * @file voice_receiver.h
 * @brief TCP PCM receiver for the Echo Pyramid Voice Node.
 *
 * The receiver is deliberately independent from the hardware audio driver.
 * The application supplies a playback callback, preserving the boundary
 * between network transport and Echo Pyramid codec/I2S implementation.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VOICE_RECEIVER_DEFAULT_PORT 5006

/**
 * @brief Callback used to render one block of signed 16-bit mono PCM.
 *
 * PCM samples use the ESP32 native little-endian representation. The current
 * V1 transport contract accepts only 16 kHz, mono, signed 16-bit PCM.
 */
typedef esp_err_t (*voice_receiver_playback_cb_t)(
    const int16_t *samples,
    size_t sample_count);

/**
 * @brief Starts the dedicated TCP voice receiver task.
 *
 * @param port TCP listening port. Use VOICE_RECEIVER_DEFAULT_PORT for V1.
 * @param playback_cb Application-owned PCM playback callback.
 *
 * @return ESP_OK when the task is created successfully.
 */
esp_err_t voice_receiver_start(
    uint16_t port,
    voice_receiver_playback_cb_t playback_cb);

#ifdef __cplusplus
}
#endif
