/**
 * @file audio_bridge.h
 * @brief C-compatible interface for the M5EchoPyramid audio subsystem.
 *
 * The application firmware remains implemented in C. This bridge isolates
 * the C++ M5EchoPyramid API and reuses the I2C master bus owned by the
 * application.
 */

#ifndef AUDIO_BRIDGE_H
#define AUDIO_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Echo Pyramid audio subsystem.
 *
 * The supplied I2C bus remains owned by the application. The audio library
 * attaches to the existing bus and must not create or delete another bus.
 *
 * @param bus_handle Existing ESP-IDF I2C master bus.
 * @return ESP_OK on success, otherwise an ESP-IDF error.
 */
esp_err_t audio_bridge_init(i2c_master_bus_handle_t bus_handle);

/**
 * @brief Play mono 16-bit PCM samples through the Echo Pyramid speaker.
 *
 * The samples must use the audio subsystem sample rate of 16000 Hz.
 * Internally, the M5EchoPyramid library duplicates each mono sample into
 * the left and right I2S channels.
 *
 * @param samples Pointer to signed 16-bit mono PCM samples.
 * @param sample_count Number of mono samples to reproduce.
 * @return ESP_OK on success, otherwise an ESP-IDF error.
 */
esp_err_t audio_bridge_play_pcm(
    const int16_t *samples,
    size_t sample_count);

/**
 * @brief Capture mono PCM samples from the Echo Pyramid microphones.
 *
 * The microphone and reference channels are returned separately.
 *
 * @param microphone_samples Destination for microphone samples.
 * @param reference_samples Destination for acoustic reference samples.
 * @param sample_count Number of samples to capture.
 * @return ESP_OK on success, otherwise an ESP-IDF error.
 */
esp_err_t audio_bridge_record_pcm(
    int16_t *microphone_samples,
    int16_t *reference_samples,
    size_t sample_count);

/**
 * @brief Record audio from the microphone and immediately reproduce it.
 *
 * This diagnostic validates the complete path:
 *
 * microphone -> I2S RX -> PCM buffer -> I2S TX -> codec -> speaker
 *
 * @param duration_seconds Recording duration in seconds.
 * @return ESP_OK on success, otherwise an ESP-IDF error.
 */
esp_err_t audio_bridge_record_playback_test(
    uint32_t duration_seconds);

#ifdef __cplusplus
}
#endif

#endif