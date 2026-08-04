#include "expression_processor.h"

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "rgb_effects.h"

static const char *TAG = "expression_processor";

/**
 * @brief Maps one normalized Cognitive Runtime state to a visual effect.
 *
 * This module owns the Runtime State vocabulary understood by the StickC
 * Plus 2. It does not access display, LED or networking APIs directly.
 */
int expression_processor_process(const char *runtime_state)
{
    if (runtime_state == NULL || runtime_state[0] == '\0') {
        ESP_LOGE(TAG, "Invalid Runtime State");
        return (int)ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(
        TAG,
        "Processing Runtime State: %s",
        runtime_state
    );

    if (strcmp(runtime_state, "idle") == 0) {
        return rgb_effects_idle();
    }

    if (strcmp(runtime_state, "presence") == 0) {
        return rgb_effects_presence();
    }

    if (strcmp(runtime_state, "listening") == 0) {
        return rgb_effects_listening();
    }

    if (strcmp(runtime_state, "thinking") == 0) {
        return rgb_effects_thinking();
    }

    if (strcmp(runtime_state, "responding") == 0) {
        return rgb_effects_responding();
    }

    if (strcmp(runtime_state, "alert") == 0) {
        return rgb_effects_alert();
    }

    if (strcmp(runtime_state, "error") == 0) {
        return rgb_effects_error();
    }

    if (strcmp(runtime_state, "offline") == 0) {
        return rgb_effects_offline();
    }

    if (strcmp(runtime_state, "learning") == 0) {
        return rgb_effects_learning();
    }

    ESP_LOGW(
        TAG,
        "Unsupported Runtime State: %s",
        runtime_state
    );

    return (int)ESP_ERR_NOT_SUPPORTED;
}