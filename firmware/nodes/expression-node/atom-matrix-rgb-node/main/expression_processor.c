#include "expression_processor.h"

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "rgb_effects.h"

static const char *TAG = "expression_processor";

int expression_processor_process(const char *event_type)
{
    if (event_type == NULL || event_type[0] == '\0') {
        ESP_LOGE(TAG, "Invalid semantic event");
        return (int)ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Processing semantic event: %s", event_type);

    if (strcmp(event_type, "boot") == 0) {
        return rgb_effects_boot();
    }

    if (strcmp(event_type, "idle") == 0) {
        return rgb_effects_idle();
    }

    if (strcmp(event_type, "presence_detected") == 0) {
        return rgb_effects_presence_detected();
    }

    if (strcmp(event_type, "identity_authenticated") == 0) {
        return rgb_effects_identity_authenticated();
    }

    if (strcmp(event_type, "processing") == 0) {
        return rgb_effects_processing();
    }

    if (strcmp(event_type, "system_error") == 0) {
        return rgb_effects_error();
    }

    ESP_LOGW(TAG, "Unsupported semantic event: %s", event_type);

    return (int)ESP_ERR_NOT_SUPPORTED;
}