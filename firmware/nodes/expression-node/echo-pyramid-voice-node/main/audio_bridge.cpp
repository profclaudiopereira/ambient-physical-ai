/**
 * @file audio_bridge.cpp
 * @brief C++ bridge for the M5EchoPyramid audio subsystem.
 *
 * This file is the only application module that directly depends on the
 * M5EchoPyramid C++ API. The main firmware remains implemented in C.
 */

#include "audio_bridge.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "M5EchoPyramid.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr int AUDIO_BCLK_GPIO = 6;
constexpr int AUDIO_LRCK_GPIO = 8;
constexpr int AUDIO_DOUT_GPIO = 5;
constexpr int AUDIO_DIN_GPIO  = 7;

constexpr uint32_t AUDIO_SAMPLE_RATE = 16000;
constexpr int AUDIO_FRAME_SIZE = 256;
constexpr int AUDIO_VOLUME = 50;

const char *TAG = "echo_audio_bridge";

M5EchoPyramid echo_pyramid;
bool audio_initialized = false;

}  // namespace

extern "C" esp_err_t audio_bridge_init(
    i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == nullptr) {
        ESP_LOGE(TAG, "Cannot initialize audio with a null I2C bus");
        return ESP_ERR_INVALID_ARG;
    }

    if (audio_initialized) {
        ESP_LOGW(TAG, "Audio subsystem is already initialized");
        return ESP_OK;
    }

    ESP_LOGI(
        TAG,
        "Initializing Echo Pyramid audio at %lu Hz using shared I2C",
        static_cast<unsigned long>(AUDIO_SAMPLE_RATE));

    const bool initialized = echo_pyramid.begin(
        bus_handle,
        AUDIO_BCLK_GPIO,
        AUDIO_LRCK_GPIO,
        AUDIO_DOUT_GPIO,
        AUDIO_DIN_GPIO,
        AUDIO_SAMPLE_RATE);

    if (!initialized) {
        ESP_LOGE(TAG, "M5EchoPyramid initialization failed");
        return ESP_FAIL;
    }

    /*
     * Preserve the codec configuration validated by the original standalone
     * microphone recording and playback test.
     */
    echo_pyramid.codec().setVolume(AUDIO_VOLUME);
    echo_pyramid.codec().mute(false);

    audio_initialized = true;

    ESP_LOGI(
        TAG,
        "Echo Pyramid audio initialized | sample_rate=%lu | volume=%d",
        static_cast<unsigned long>(AUDIO_SAMPLE_RATE),
        AUDIO_VOLUME);

    return ESP_OK;
}

extern "C" esp_err_t audio_bridge_play_pcm(
    const int16_t *samples,
    size_t sample_count)
{
    if (!audio_initialized) {
        ESP_LOGE(TAG, "PCM playback requested before audio initialization");
        return ESP_ERR_INVALID_STATE;
    }

    if (samples == nullptr || sample_count == 0) {
        ESP_LOGE(TAG, "Invalid PCM playback buffer");
        return ESP_ERR_INVALID_ARG;
    }

    size_t samples_played = 0;

    while (samples_played < sample_count) {
        const size_t remaining = sample_count - samples_played;
        const int frame_count =
            remaining > static_cast<size_t>(AUDIO_FRAME_SIZE)
                ? AUDIO_FRAME_SIZE
                : static_cast<int>(remaining);

        /*
         * M5EchoPyramid::write() currently receives a mutable pointer even
         * though it does not modify the caller's mono PCM input.
         */
        echo_pyramid.write(
            const_cast<int16_t *>(samples + samples_played),
            frame_count);

        samples_played += static_cast<size_t>(frame_count);
    }

    return ESP_OK;
}

extern "C" esp_err_t audio_bridge_record_pcm(
    int16_t *microphone_samples,
    int16_t *reference_samples,
    size_t sample_count)
{
    if (!audio_initialized) {
        ESP_LOGE(TAG, "PCM capture requested before audio initialization");
        return ESP_ERR_INVALID_STATE;
    }

    if (microphone_samples == nullptr ||
        reference_samples == nullptr ||
        sample_count == 0) {
        ESP_LOGE(TAG, "Invalid PCM capture buffer");
        return ESP_ERR_INVALID_ARG;
    }

    size_t samples_recorded = 0;

    while (samples_recorded < sample_count) {
        const size_t remaining = sample_count - samples_recorded;
        const int frame_count =
            remaining > static_cast<size_t>(AUDIO_FRAME_SIZE)
                ? AUDIO_FRAME_SIZE
                : static_cast<int>(remaining);

        echo_pyramid.read(
            microphone_samples + samples_recorded,
            reference_samples + samples_recorded,
            frame_count);

        samples_recorded += static_cast<size_t>(frame_count);
    }

    return ESP_OK;
}

extern "C" esp_err_t audio_bridge_record_playback_test(
    uint32_t duration_seconds)
{
    if (!audio_initialized) {
        ESP_LOGE(
            TAG,
            "Record/playback test requested before audio initialization");
        return ESP_ERR_INVALID_STATE;
    }

    if (duration_seconds == 0 || duration_seconds > 10) {
        ESP_LOGE(
            TAG,
            "Invalid diagnostic duration: %lu seconds",
            static_cast<unsigned long>(duration_seconds));
        return ESP_ERR_INVALID_ARG;
    }

    const size_t total_samples =
        static_cast<size_t>(AUDIO_SAMPLE_RATE) *
        static_cast<size_t>(duration_seconds);

    const size_t buffer_bytes = total_samples * sizeof(int16_t);

    int16_t *record_buffer = static_cast<int16_t *>(
        heap_caps_malloc(buffer_bytes, MALLOC_CAP_8BIT));

    if (record_buffer == nullptr) {
        ESP_LOGE(
            TAG,
            "Unable to allocate microphone buffer: %u bytes",
            static_cast<unsigned>(buffer_bytes));
        return ESP_ERR_NO_MEM;
    }

    int16_t reference_frame[AUDIO_FRAME_SIZE];

    ESP_LOGI(
        TAG,
        "Recording diagnostic audio for %lu second(s)",
        static_cast<unsigned long>(duration_seconds));

    size_t samples_recorded = 0;

    while (samples_recorded < total_samples) {
        const size_t remaining = total_samples - samples_recorded;
        const int frame_count =
            remaining > static_cast<size_t>(AUDIO_FRAME_SIZE)
                ? AUDIO_FRAME_SIZE
                : static_cast<int>(remaining);

        echo_pyramid.read(
            record_buffer + samples_recorded,
            reference_frame,
            frame_count);

        samples_recorded += static_cast<size_t>(frame_count);
    }

    ESP_LOGI(TAG, "Diagnostic recording completed");

    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI(TAG, "Replaying diagnostic recording");

    const esp_err_t playback_result =
        audio_bridge_play_pcm(record_buffer, total_samples);

    free(record_buffer);

    if (playback_result != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Diagnostic playback failed: %s",
            esp_err_to_name(playback_result));
        return playback_result;
    }

    ESP_LOGI(TAG, "Record/playback diagnostic completed");
    return ESP_OK;
}
