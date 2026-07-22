#include "semantic_consumer.h"

#include <string.h>

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

#include "expression_processor.h"

static const char *TAG = "semantic_consumer";

int semantic_consumer_process(const char *payload)
{
    if (payload == NULL || payload[0] == '\0') {
        ESP_LOGE(TAG, "Empty semantic payload");
        return (int)ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(payload);

    if (root == NULL) {
        ESP_LOGE(TAG, "Invalid JSON payload");
        return (int)ESP_ERR_INVALID_ARG;
    }

    const cJSON *type =
        cJSON_GetObjectItemCaseSensitive(root, "type");

    const cJSON *event =
        cJSON_GetObjectItemCaseSensitive(root, "event");

    const cJSON *target =
        cJSON_GetObjectItemCaseSensitive(root, "target");

    if (!cJSON_IsString(type) ||
        type->valuestring == NULL ||
        strcmp(type->valuestring, "semantic_event") != 0) {

        ESP_LOGW(TAG, "Unsupported message type");
        cJSON_Delete(root);
        return (int)ESP_ERR_NOT_SUPPORTED;
    }

    if (!cJSON_IsString(event) ||
        event->valuestring == NULL ||
        event->valuestring[0] == '\0') {

        ESP_LOGE(TAG, "Missing semantic event");
        cJSON_Delete(root);
        return (int)ESP_ERR_INVALID_ARG;
    }

    if (!cJSON_IsString(target) ||
        target->valuestring == NULL) {

        ESP_LOGE(TAG, "Missing semantic target");
        cJSON_Delete(root);
        return (int)ESP_ERR_INVALID_ARG;
    }

    /*
     * Este node aceita eventos destinados diretamente ao rgb_strip
     * ou genericamente à expression_layer.
     */
    if (strcmp(target->valuestring, "rgb_strip") != 0 &&
        strcmp(target->valuestring, "expression_layer") != 0) {

        ESP_LOGW(
            TAG,
            "Event ignored; target is %s",
            target->valuestring
        );

        cJSON_Delete(root);
        return (int)ESP_ERR_NOT_SUPPORTED;
    }

    ESP_LOGI(
        TAG,
        "Semantic event accepted: %s",
        event->valuestring
    );

    int result =
        expression_processor_process(event->valuestring);

    if (result != 0) {
        ESP_LOGE(
            TAG,
            "Expression processing failed for event: %s",
            event->valuestring
        );
    }

    cJSON_Delete(root);

    return result;
}