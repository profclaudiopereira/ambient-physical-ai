/**
 * @file wake_word_detector.h
 * @brief Local WakeNet + MultiNet detector for the Echo Pyramid Voice Node.
 */

#ifndef WAKE_WORD_DETECTOR_H
#define WAKE_WORD_DETECTOR_H

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*wake_word_detected_cb_t)(
    int wake_word_index,
    int model_index);

typedef void (*speech_command_detected_cb_t)(
    int command_id,
    float probability);

/**
 * @brief Start local WakeNet and MultiNet detection.
 *
 * WakeNet remains continuously active. After "Hi ESP", MultiNet opens a
 * bounded command window and recognizes one of the configured contexts.
 */
esp_err_t wake_word_detector_start(
    wake_word_detected_cb_t wake_callback,
    speech_command_detected_cb_t command_callback);

/** Pause microphone feeding without destroying the AFE/MultiNet instances. */
esp_err_t wake_word_detector_pause(void);

/** Resume microphone feeding and reset stale buffered audio. */
esp_err_t wake_word_detector_resume(void);

/** Return true when the detector has been initialized. */
bool wake_word_detector_is_started(void);

/** Return true while microphone feeding is enabled. */
bool wake_word_detector_is_listening(void);

#ifdef __cplusplus
}
#endif

#endif
