/**
 * @file wake_word_detector.c
 * @brief ESP-SR WakeNet + MultiNet6 integration for Echo Pyramid.
 *
 * Audio is captured exclusively through audio_bridge_record_pcm(). The AFE
 * receives one microphone channel and one playback-reference channel in the
 * interleaved "MR" format required by ESP-SR.
 */

#include "wake_word_detector.h"

#include <stdlib.h>
#include <string.h>

#include "audio_bridge.h"

#include "esp_afe_config.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_log.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_mn_speech_commands.h"
#include "model_path.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "wake_word_detector";

enum {
    CONTEXT_CMD_RESEARCH = 1,
    CONTEXT_CMD_LAB = 2,
    CONTEXT_CMD_MEETING = 3,
    CONTEXT_CMD_CLASSROOM = 4,
    CONTEXT_CMD_DEMO = 5,
};

static const esp_afe_sr_iface_t *s_afe = NULL;
static esp_afe_sr_data_t *s_afe_data = NULL;
static srmodel_list_t *s_models = NULL;

static esp_mn_iface_t *s_multinet = NULL;
static model_iface_data_t *s_multinet_data = NULL;

static TaskHandle_t s_feed_task = NULL;
static TaskHandle_t s_fetch_task = NULL;

static wake_word_detected_cb_t s_wake_callback = NULL;
static speech_command_detected_cb_t s_command_callback = NULL;

static volatile bool s_started = false;
static volatile bool s_listening = false;
static volatile bool s_command_window = false;

static esp_err_t configure_context_commands(void)
{
    esp_err_t err = esp_mn_commands_clear();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Unable to clear MultiNet commands: %s",
                 esp_err_to_name(err));
        return err;
    }

    struct {
        int id;
        char *phrase;
    } commands[] = {
        {CONTEXT_CMD_RESEARCH,  "RESEARCH"},
        {CONTEXT_CMD_LAB,       "LAB"},
        {CONTEXT_CMD_MEETING,   "MEETING"},
        {CONTEXT_CMD_CLASSROOM, "CLASSROOM"},
        {CONTEXT_CMD_DEMO,      "DEMO"},
    };

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        err = esp_mn_commands_add(commands[i].id, commands[i].phrase);
        if (err != ESP_OK) {
            ESP_LOGE(TAG,
                     "Unable to add MultiNet command %d (%s): %s",
                     commands[i].id,
                     commands[i].phrase,
                     esp_err_to_name(err));
            return err;
        }
    }

    esp_mn_error_t *update_error = esp_mn_commands_update();
    if (update_error != NULL) {
        ESP_LOGE(TAG, "MultiNet command grammar update failed");
        return ESP_FAIL;
    }

    esp_mn_active_commands_print();
    return ESP_OK;
}

static void wake_word_feed_task(void *arg)
{
    (void)arg;

    const int frame_samples = s_afe->get_feed_chunksize(s_afe_data);
    const int feed_channels = s_afe->get_feed_channel_num(s_afe_data);

    if (feed_channels != 2) {
        ESP_LOGE(TAG,
                 "Unexpected AFE feed channel count: %d; expected MR=2",
                 feed_channels);
        vTaskDelete(NULL);
        return;
    }

    int16_t *microphone = calloc((size_t)frame_samples, sizeof(int16_t));
    int16_t *reference = calloc((size_t)frame_samples, sizeof(int16_t));
    int16_t *interleaved =
        calloc((size_t)frame_samples * 2U, sizeof(int16_t));

    if (microphone == NULL || reference == NULL || interleaved == NULL) {
        ESP_LOGE(TAG, "Unable to allocate WakeNet feed buffers");
        free(microphone);
        free(reference);
        free(interleaved);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Feed task active | frame=%d | channels=%d",
             frame_samples, feed_channels);

    while (s_started) {
        if (!s_listening) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        esp_err_t err = audio_bridge_record_pcm(
            microphone, reference, (size_t)frame_samples);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Audio capture failed: %s",
                     esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        for (int i = 0; i < frame_samples; ++i) {
            interleaved[i * 2] = microphone[i];
            interleaved[(i * 2) + 1] = reference[i];
        }

        const int fed = s_afe->feed(s_afe_data, interleaved);
        if (fed < 0) {
            ESP_LOGW(TAG, "AFE feed returned %d", fed);
        }
    }

    free(microphone);
    free(reference);
    free(interleaved);
    s_feed_task = NULL;
    vTaskDelete(NULL);
}

static void wake_word_fetch_task(void *arg)
{
    (void)arg;

    const int afe_chunk = s_afe->get_fetch_chunksize(s_afe_data);
    const int mn_chunk =
        s_multinet->get_samp_chunksize(s_multinet_data);

    if (afe_chunk != mn_chunk) {
        ESP_LOGE(TAG,
                 "AFE/MultiNet chunk mismatch | afe=%d | multinet=%d",
                 afe_chunk, mn_chunk);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "WakeNet/MultiNet fetch task active | chunk=%d",
             afe_chunk);

    while (s_started) {
        if (!s_listening) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        afe_fetch_result_t *result =
            s_afe->fetch_with_delay(s_afe_data, pdMS_TO_TICKS(500));

        if (result == NULL) {
            continue;
        }

        if (result->ret_value == ESP_FAIL) {
            ESP_LOGE(TAG, "AFE fetch failed");
            continue;
        }

        const bool wake_detected =
            result->wakeup_state == WAKENET_DETECTED ||
            result->wakeup_state == WAKENET_CHANNEL_VERIFIED;

        if (wake_detected) {
            ESP_LOGI(TAG,
                     "WAKE WORD DETECTED | word=%d | model=%d | channel=%d",
                     result->wake_word_index,
                     result->wakenet_model_index,
                     result->trigger_channel_id);

            s_multinet->clean(s_multinet_data);
            s_command_window = true;

            ESP_LOGI(TAG,
                     "Context command window OPEN | "
                     "Research/Lab/Meeting/Classroom/Demo");

            if (s_wake_callback != NULL) {
                s_wake_callback(
                    result->wake_word_index,
                    result->wakenet_model_index);
            }

            continue;
        }

        if (!s_command_window) {
            continue;
        }

        esp_mn_state_t state =
            s_multinet->detect(s_multinet_data, result->data);

        if (state == ESP_MN_STATE_DETECTING) {
            continue;
        }

        if (state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result =
                s_multinet->get_results(s_multinet_data);

            if (mn_result != NULL && mn_result->num > 0) {
                const int command_id = mn_result->command_id[0];
                const float probability = mn_result->prob[0];

                ESP_LOGI(TAG,
                         "CONTEXT COMMAND DETECTED | id=%d | prob=%.3f",
                         command_id, probability);

                s_command_window = false;

                if (s_command_callback != NULL) {
                    s_command_callback(command_id, probability);
                }
            }
            continue;
        }

        if (state == ESP_MN_STATE_TIMEOUT) {
            ESP_LOGW(TAG, "Context command window TIMEOUT");

            s_command_window = false;
            s_multinet->clean(s_multinet_data);

            /*
            * O ID 0 é reservado como notificação local de timeout.
            * Ele não representa um contexto e nunca será enviado ao AX630C.
            */
            if (s_command_callback != NULL) {
                s_command_callback(0, 0.0f);
        }
    }
    }

    s_fetch_task = NULL;
    vTaskDelete(NULL);
}

esp_err_t wake_word_detector_start(
    wake_word_detected_cb_t wake_callback,
    speech_command_detected_cb_t command_callback)
{
    if (s_started) {
        ESP_LOGW(TAG, "Wake Word detector already started");
        return ESP_OK;
    }

    s_models = esp_srmodel_init("model");
    if (s_models == NULL) {
        ESP_LOGE(TAG,
                 "ESP-SR model partition/list was not initialized");
        return ESP_FAIL;
    }

    afe_config_t *config = afe_config_init(
        "MR", s_models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);

    if (config == NULL) {
        ESP_LOGE(TAG, "Unable to create AFE configuration");
        esp_srmodel_deinit(s_models);
        s_models = NULL;
        return ESP_FAIL;
    }

    config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    config->vad_init = true;
    config->aec_init = true;
    config->wakenet_init = true;

    s_afe = esp_afe_handle_from_config(config);
    if (s_afe == NULL) {
        ESP_LOGE(TAG, "Unable to obtain ESP-SR AFE interface");
        afe_config_free(config);
        esp_srmodel_deinit(s_models);
        s_models = NULL;
        return ESP_FAIL;
    }

    s_afe_data = s_afe->create_from_config(config);
    afe_config_free(config);

    if (s_afe_data == NULL) {
        ESP_LOGE(TAG, "Unable to create AFE instance");
        esp_srmodel_deinit(s_models);
        s_models = NULL;
        s_afe = NULL;
        return ESP_FAIL;
    }

    char *mn_name =
        esp_srmodel_filter(s_models, ESP_MN_PREFIX, ESP_MN_ENGLISH);

    if (mn_name == NULL) {
        ESP_LOGE(TAG, "English MultiNet model was not found");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "MultiNet model selected: %s", mn_name);

    s_multinet = esp_mn_handle_from_name(mn_name);
    if (s_multinet == NULL) {
        ESP_LOGE(TAG, "Unable to obtain MultiNet interface");
        return ESP_FAIL;
    }

    s_multinet_data = s_multinet->create(mn_name, 8000);
    if (s_multinet_data == NULL) {
        ESP_LOGE(TAG, "Unable to create MultiNet instance");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t command_err = configure_context_commands();
    if (command_err != ESP_OK) {
        s_multinet->destroy(s_multinet_data);
        s_multinet_data = NULL;
        return command_err;
    }

    s_wake_callback = wake_callback;
    s_command_callback = command_callback;
    s_started = true;
    s_listening = true;
    s_command_window = false;

    s_afe->print_pipeline(s_afe_data);

    BaseType_t feed_created = xTaskCreatePinnedToCore(
        wake_word_feed_task,
        "wake_word_feed",
        6144,
        NULL,
        6,
        &s_feed_task,
        0);

    BaseType_t fetch_created = xTaskCreatePinnedToCore(
        wake_word_fetch_task,
        "wake_word_fetch",
        8192,
        NULL,
        6,
        &s_fetch_task,
        1);

    if (feed_created != pdPASS || fetch_created != pdPASS) {
        ESP_LOGE(TAG, "Unable to create speech-recognition tasks");
        s_started = false;
        s_listening = false;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG,
             "Speech detector started | wake=Hi ESP | multinet=English");
    return ESP_OK;
}

esp_err_t wake_word_detector_pause(void)
{
    if (!s_started) {
        return ESP_ERR_INVALID_STATE;
    }

    s_listening = false;
    s_command_window = false;
    ESP_LOGI(TAG, "Speech detector paused");
    return ESP_OK;
}

esp_err_t wake_word_detector_resume(void)
{
    if (!s_started || s_afe == NULL || s_afe_data == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    s_afe->reset_buffer(s_afe_data);
    if (s_multinet != NULL && s_multinet_data != NULL) {
        s_multinet->clean(s_multinet_data);
    }

    s_command_window = false;
    s_listening = true;
    ESP_LOGI(TAG, "Speech detector resumed");
    return ESP_OK;
}

bool wake_word_detector_is_started(void)
{
    return s_started;
}

bool wake_word_detector_is_listening(void)
{
    return s_started && s_listening;
}
