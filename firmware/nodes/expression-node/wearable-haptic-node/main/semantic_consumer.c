#include "semantic_consumer.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "expression_processor.h"


static const char *TAG = "semantic_consumer";

/*
 * Runtime State names are intentionally bounded before they enter the
 * asynchronous queue. The current vocabulary uses substantially shorter
 * names, but this capacity leaves room for future cognitive states.
 */
#define RUNTIME_STATE_NAME_CAPACITY 32

/*
 * The queue absorbs short bursts such as:
 *
 * thinking -> responding -> idle
 *
 * while the worker preserves the minimum visual presentation time of each
 * state. Eight entries provide sufficient capacity for the current runtime
 * pipeline without introducing an unnecessarily large static allocation.
 */
#define RUNTIME_STATE_QUEUE_LENGTH 8

/*
 * The worker only coordinates local visual presentation. Networking and JSON
 * validation remain in the UDP receiver task.
 */
#define RUNTIME_STATE_TASK_STACK_SIZE 4096
#define RUNTIME_STATE_TASK_PRIORITY   5


typedef struct {
    char state[RUNTIME_STATE_NAME_CAPACITY];
} runtime_state_message_t;


typedef struct {
    const char *state;
    uint32_t minimum_display_ms;
} runtime_state_policy_t;


/*
 * Minimum presentation times apply only to the local StickC indicator.
 *
 * They do not delay the AX630C, Semantic Event generation, SemanticDispatcher
 * or any other Expression Layer node.
 */
static const runtime_state_policy_t RUNTIME_STATE_POLICIES[] = {
    {"thinking",   500},
    {"responding", 800},
    {"error",     1500},
    {"alert",     1500},
};


static QueueHandle_t s_runtime_state_queue = NULL;
static TaskHandle_t s_runtime_state_task = NULL;
static bool s_initialized = false;


/**
 * @brief Returns the minimum local presentation time for one state.
 *
 * States not explicitly listed in RUNTIME_STATE_POLICIES have no mandatory
 * minimum duration. They remain visible until another queued state is
 * presented.
 */
static uint32_t get_minimum_display_time_ms(const char *state)
{
    const size_t policy_count =
        sizeof(RUNTIME_STATE_POLICIES) /
        sizeof(RUNTIME_STATE_POLICIES[0]);

    for (size_t index = 0; index < policy_count; index++) {
        if (strcmp(
                state,
                RUNTIME_STATE_POLICIES[index].state
            ) == 0) {

            return RUNTIME_STATE_POLICIES[index]
                .minimum_display_ms;
        }
    }

    return 0;
}


/**
 * @brief Presents queued Runtime States sequentially.
 *
 * The task is the only asynchronous component that calls the expression
 * processor during normal UDP operation. Waiting here does not block packet
 * reception because semantic_consumer_process() executes in the receiver task
 * and only copies accepted states into the queue.
 */
static void runtime_state_worker_task(void *argument)
{
    (void)argument;

    runtime_state_message_t message;

    ESP_LOGI(TAG, "Runtime State worker started");

    while (true) {
        if (xQueueReceive(
                s_runtime_state_queue,
                &message,
                portMAX_DELAY
            ) != pdTRUE) {

            continue;
        }

        ESP_LOGI(
            TAG,
            "Presenting queued Runtime State: %s",
            message.state
        );

        const int result =
            expression_processor_process(message.state);

        if (result != 0) {
            ESP_LOGE(
                TAG,
                "Runtime State presentation failed: %s",
                message.state
            );

            continue;
        }

        const uint32_t minimum_display_ms =
            get_minimum_display_time_ms(message.state);

        if (minimum_display_ms > 0) {
            ESP_LOGI(
                TAG,
                "Holding Runtime State %s for at least %lu ms",
                message.state,
                (unsigned long)minimum_display_ms
            );

            vTaskDelay(pdMS_TO_TICKS(minimum_display_ms));
        }
    }
}


int semantic_consumer_init(void)
{
    if (s_initialized) {
        ESP_LOGW(TAG, "Runtime State consumer already initialized");
        return 0;
    }

    s_runtime_state_queue = xQueueCreate(
        RUNTIME_STATE_QUEUE_LENGTH,
        sizeof(runtime_state_message_t)
    );

    if (s_runtime_state_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create Runtime State queue");
        return (int)ESP_ERR_NO_MEM;
    }

    const BaseType_t task_result = xTaskCreate(
        runtime_state_worker_task,
        "runtime_state_worker",
        RUNTIME_STATE_TASK_STACK_SIZE,
        NULL,
        RUNTIME_STATE_TASK_PRIORITY,
        &s_runtime_state_task
    );

    if (task_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Runtime State worker");

        vQueueDelete(s_runtime_state_queue);
        s_runtime_state_queue = NULL;

        return (int)ESP_ERR_NO_MEM;
    }

    s_initialized = true;

    ESP_LOGI(
        TAG,
        "Runtime State consumer initialized: queue=%d",
        RUNTIME_STATE_QUEUE_LENGTH
    );

    return 0;
}


int semantic_consumer_process(const char *payload)
{
    if (!s_initialized || s_runtime_state_queue == NULL) {
        ESP_LOGE(TAG, "Runtime State consumer is not initialized");
        return (int)ESP_ERR_INVALID_STATE;
    }

    if (payload == NULL || payload[0] == '\0') {
        ESP_LOGE(TAG, "Empty Runtime State payload");
        return (int)ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(payload);

    if (root == NULL) {
        ESP_LOGE(TAG, "Invalid JSON payload");
        return (int)ESP_ERR_INVALID_ARG;
    }

    const cJSON *type =
        cJSON_GetObjectItemCaseSensitive(root, "type");

    const cJSON *state =
        cJSON_GetObjectItemCaseSensitive(root, "state");

    const cJSON *target =
        cJSON_GetObjectItemCaseSensitive(root, "target");

    if (!cJSON_IsString(type) ||
        type->valuestring == NULL ||
        strcmp(type->valuestring, "runtime_state") != 0) {

        ESP_LOGW(TAG, "Unsupported message type");
        cJSON_Delete(root);
        return (int)ESP_ERR_NOT_SUPPORTED;
    }

    if (!cJSON_IsString(state) ||
        state->valuestring == NULL ||
        state->valuestring[0] == '\0') {

        ESP_LOGE(TAG, "Missing Runtime State");
        cJSON_Delete(root);
        return (int)ESP_ERR_INVALID_ARG;
    }

    if (!cJSON_IsString(target) ||
        target->valuestring == NULL) {

        ESP_LOGE(TAG, "Missing target");
        cJSON_Delete(root);
        return (int)ESP_ERR_INVALID_ARG;
    }

    /*
     * The StickC Plus 2 is the dedicated Runtime State Indicator.
     */
    if (strcmp(
            target->valuestring,
            "runtime_state_indicator"
        ) != 0) {

        ESP_LOGW(
            TAG,
            "Message ignored; target is %s",
            target->valuestring
        );

        cJSON_Delete(root);
        return (int)ESP_ERR_NOT_SUPPORTED;
    }

    const size_t state_length = strlen(state->valuestring);

    if (state_length >= RUNTIME_STATE_NAME_CAPACITY) {
        ESP_LOGE(
            TAG,
            "Runtime State name is too long: %u characters",
            (unsigned int)state_length
        );

        cJSON_Delete(root);
        return (int)ESP_ERR_INVALID_SIZE;
    }

    runtime_state_message_t message = {0};

    memcpy(
        message.state,
        state->valuestring,
        state_length + 1
    );

    /*
     * The JSON object is deleted after this point. The state must therefore
     * be copied into queue-owned storage rather than storing valuestring.
     */
    cJSON_Delete(root);

    if (xQueueSend(
            s_runtime_state_queue,
            &message,
            0
        ) != pdTRUE) {

        ESP_LOGE(
            TAG,
            "Runtime State queue is full; state rejected: %s",
            message.state
        );

        return (int)ESP_ERR_NO_MEM;
    }

    ESP_LOGI(
        TAG,
        "Runtime State accepted and queued: %s",
        message.state
    );

    return 0;
}