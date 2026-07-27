#include "expression_processor.h"

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "rgb_effects.h"

static const char *TAG = "expression_processor";

static int process_identity_authenticated(
    const semantic_context_t *context
)
{
    const char *user_id =
        (context != NULL &&
         context->user_id != NULL &&
         context->user_id[0] != '\0')
            ? context->user_id
            : "unknown";

    ESP_LOGI(TAG, "Authenticated profile: %s", user_id);

    if (strcmp(user_id, "claudio") == 0) {
        ESP_LOGI(TAG, "Applying profile: Claudio");
        return rgb_effects_profile_claudio();
    }

    if (strcmp(user_id, "herminio") == 0) {
        ESP_LOGI(TAG, "Applying profile: Herminio");
        return rgb_effects_profile_herminio();
    }

    if (strcmp(user_id, "mariana") == 0 ||
        strcmp(user_id, "student") == 0) {

        ESP_LOGI(TAG, "Applying profile: Mariana");
        return rgb_effects_profile_mariana();
    }

    if (strcmp(user_id, "visitor") == 0 ||
        strcmp(user_id, "guest") == 0) {

        ESP_LOGI(TAG, "Applying profile: Visitor");
        return rgb_effects_profile_visitor();
    }

    ESP_LOGW(
        TAG,
        "Unknown profile '%s'; applying Visitor fallback",
        user_id
    );

    return rgb_effects_profile_visitor();
}

int expression_processor_process(
    const char *event_type,
    const semantic_context_t *context
)
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
        return process_identity_authenticated(context);
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
