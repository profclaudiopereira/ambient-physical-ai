#include "semantic_event_receiver.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

static const char *TAG = "semantic-receiver";

#define SEMANTIC_MESSAGE_TYPE_LENGTH 32
#define UDP_RECEIVE_BUFFER_LENGTH 1024

typedef struct {
    char type[SEMANTIC_MESSAGE_TYPE_LENGTH];
    char event_type[SEMANTIC_EVENT_TYPE_LENGTH];
    char target[SEMANTIC_EVENT_TARGET_LENGTH];
} semantic_event_t;

static portMUX_TYPE s_state_lock = portMUX_INITIALIZER_UNLOCKED;

static semantic_event_receiver_status_t s_status = {
    .initialized = false,
    .listening = false,
    .event_received = false,
    .received_count = 0,
    .last_event_type = "",
    .last_target = ""
};

static ambient_context_snapshot_t s_context = {};

static uint64_t monotonic_ms(void)
{
    return (uint64_t)(esp_timer_get_time() / 1000ULL);
}

static void copy_json_string(
    const cJSON *object,
    const char *field,
    char *destination,
    size_t destination_size
)
{
    if (destination == nullptr || destination_size == 0) {
        return;
    }

    destination[0] = '\0';

    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, field);
    if (!cJSON_IsString(item) || item->valuestring == nullptr) {
        return;
    }

    snprintf(destination, destination_size, "%s", item->valuestring);
}

static bool json_bool_or(
    const cJSON *object,
    const char *field,
    bool fallback
)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, field);
    return cJSON_IsBool(item) ? cJSON_IsTrue(item) : fallback;
}

static double json_number_or(
    const cJSON *object,
    const char *field,
    double fallback
)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, field);
    return cJSON_IsNumber(item) ? item->valuedouble : fallback;
}

static void set_listening(bool listening)
{
    portENTER_CRITICAL(&s_state_lock);
    s_status.listening = listening;
    portEXIT_CRITICAL(&s_state_lock);
}

static void register_received_event(
    const char *event_type,
    const char *target
)
{
    if (event_type == nullptr || target == nullptr) {
        return;
    }

    portENTER_CRITICAL(&s_state_lock);

    s_status.event_received = true;
    s_status.received_count++;

    snprintf(
        s_status.last_event_type,
        sizeof(s_status.last_event_type),
        "%s",
        event_type
    );

    snprintf(
        s_status.last_target,
        sizeof(s_status.last_target),
        "%s",
        target
    );

    portEXIT_CRITICAL(&s_state_lock);
}

static bool parse_semantic_event(
    const cJSON *root,
    semantic_event_t *event
)
{
    if (root == nullptr || event == nullptr) {
        return false;
    }

    memset(event, 0, sizeof(*event));

    copy_json_string(root, "type", event->type, sizeof(event->type));
    copy_json_string(
        root,
        "event_type",
        event->event_type,
        sizeof(event->event_type)
    );
    copy_json_string(root, "target", event->target, sizeof(event->target));

    return event->event_type[0] != '\0' && event->target[0] != '\0';
}

static bool parse_ambient_context(
    const cJSON *root,
    ambient_context_snapshot_t *context
)
{
    if (root == nullptr || context == nullptr) {
        return false;
    }

    const cJSON *global =
        cJSON_GetObjectItemCaseSensitive(root, "global");

    const cJSON *personal =
        cJSON_GetObjectItemCaseSensitive(root, "personal");

    if (!cJSON_IsObject(global) || !cJSON_IsObject(personal)) {
        return false;
    }

    memset(context, 0, sizeof(*context));

    context->context_received = true;
    context->authenticated =
        json_bool_or(root, "authenticated", false);

    context->sequence = (uint32_t)json_number_or(root, "sequence", 0);
    context->ttl_seconds =
        (uint32_t)json_number_or(root, "ttl_seconds", 900);

    if (context->ttl_seconds == 0) {
        context->ttl_seconds = 900;
    }

    context->received_at_ms = monotonic_ms();

    copy_json_string(
        root,
        "profile_id",
        context->profile_id,
        sizeof(context->profile_id)
    );

    context->global_available =
        json_bool_or(global, "available", false);

    copy_json_string(
        global,
        "location",
        context->location,
        sizeof(context->location)
    );

    copy_json_string(
        global,
        "weather_summary",
        context->weather_summary,
        sizeof(context->weather_summary)
    );

    context->temperature_c =
        (float)json_number_or(global, "temperature_c", 0.0);

    context->uv_index =
        (float)json_number_or(global, "uv_index", 0.0);

    copy_json_string(
        global,
        "uv_label",
        context->uv_label,
        sizeof(context->uv_label)
    );

    context->personal_available =
        json_bool_or(personal, "available", false);

    copy_json_string(
        personal,
        "title",
        context->personal_title,
        sizeof(context->personal_title)
    );

    copy_json_string(
        personal,
        "value",
        context->personal_value,
        sizeof(context->personal_value)
    );

    copy_json_string(
        personal,
        "secondary",
        context->personal_secondary,
        sizeof(context->personal_secondary)
    );

    return true;
}

static void register_ambient_context(
    const ambient_context_snapshot_t *context
)
{
    if (context == nullptr) {
        return;
    }

    portENTER_CRITICAL(&s_state_lock);

    uint32_t next_count = s_context.received_count + 1U;
    s_context = *context;
    s_context.received_count = next_count;

    portEXIT_CRITICAL(&s_state_lock);
}

static void process_ambient_runtime_event(
    const semantic_event_t *event
)
{
    if (event == nullptr) {
        return;
    }

    if (strcmp(event->target, "ambient_runtime") != 0) {
        ESP_LOGI(TAG, "Ignoring event for target: %s", event->target);
        return;
    }

    if (strcmp(event->event_type, "identity_authenticated") == 0) {
        ESP_LOGI(TAG, "identity_authenticated received");
        return;
    }

    ESP_LOGI(TAG, "Event '%s' not implemented yet", event->event_type);
}

static void consume_semantic_event(
    const semantic_event_t *event
)
{
    register_received_event(event->event_type, event->target);

    ESP_LOGI(
        TAG,
        "Semantic event consumed | Type=%s Event=%s Target=%s",
        event->type,
        event->event_type,
        event->target
    );

    process_ambient_runtime_event(event);
}

static void consume_ambient_context(
    const ambient_context_snapshot_t *context
)
{
    register_ambient_context(context);

    ESP_LOGI(
        TAG,
        "Ambient context consumed | Seq=%u Profile=%s Global=%s Personal=%s",
        (unsigned)context->sequence,
        context->profile_id,
        context->global_available ? "YES" : "NO",
        context->personal_available ? "YES" : "NO"
    );
}

static void dispatch_json_message(const char *payload)
{
    cJSON *root = cJSON_Parse(payload);
    if (root == nullptr) {
        ESP_LOGW(TAG, "Invalid JSON payload ignored");
        return;
    }

    const cJSON *type =
        cJSON_GetObjectItemCaseSensitive(root, "type");

    if (!cJSON_IsString(type) || type->valuestring == nullptr) {
        ESP_LOGW(TAG, "Message without string field 'type' ignored");
        cJSON_Delete(root);
        return;
    }

    if (strcmp(type->valuestring, "semantic_event") == 0) {
        semantic_event_t event = {};

        if (parse_semantic_event(root, &event)) {
            consume_semantic_event(&event);
        } else {
            ESP_LOGW(TAG, "Incomplete semantic_event ignored");
        }
    } else if (strcmp(type->valuestring, "ambient_context") == 0) {
        ambient_context_snapshot_t context = {};

        if (parse_ambient_context(root, &context)) {
            consume_ambient_context(&context);
        } else {
            ESP_LOGW(TAG, "Incomplete ambient_context ignored");
        }
    } else {
        ESP_LOGI(TAG, "Unsupported message type: %s", type->valuestring);
    }

    cJSON_Delete(root);
}

static void semantic_receiver_task(void *argument)
{
    (void)argument;

    ESP_LOGI(TAG, "Semantic receiver task started");

    char receive_buffer[UDP_RECEIVE_BUFFER_LENGTH];

    while (true) {
        int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (socket_fd < 0) {
            ESP_LOGE(TAG, "socket() failed errno=%d", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        struct sockaddr_in listen_address = {};
        listen_address.sin_family = AF_INET;
        listen_address.sin_port = htons(SEMANTIC_EVENT_RECEIVER_PORT);
        listen_address.sin_addr.s_addr = htonl(INADDR_ANY);

        int bind_result = bind(
            socket_fd,
            reinterpret_cast<struct sockaddr *>(&listen_address),
            sizeof(listen_address)
        );

        if (bind_result < 0) {
            ESP_LOGE(TAG, "bind() failed errno=%d", errno);
            close(socket_fd);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        set_listening(true);
        ESP_LOGI(
            TAG,
            "Waiting for runtime messages on UDP port %d",
            SEMANTIC_EVENT_RECEIVER_PORT
        );

        while (true) {
            struct sockaddr_in source_address = {};
            socklen_t source_length = sizeof(source_address);

            int received = recvfrom(
                socket_fd,
                receive_buffer,
                sizeof(receive_buffer) - 1,
                0,
                reinterpret_cast<struct sockaddr *>(&source_address),
                &source_length
            );

            if (received < 0) {
                ESP_LOGE(TAG, "recvfrom() failed errno=%d", errno);
                break;
            }

            receive_buffer[received] = '\0';

            ESP_LOGI(TAG, "Received %d bytes", received);
            dispatch_json_message(receive_buffer);
        }

        set_listening(false);
        close(socket_fd);

        ESP_LOGW(TAG, "Receiver restarting...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

esp_err_t semantic_event_receiver_init(void)
{
    portENTER_CRITICAL(&s_state_lock);
    bool already_initialized = s_status.initialized;
    portEXIT_CRITICAL(&s_state_lock);

    if (already_initialized) {
        ESP_LOGW(TAG, "Semantic event receiver already initialized");
        return ESP_OK;
    }

    portENTER_CRITICAL(&s_state_lock);
    memset(&s_status, 0, sizeof(s_status));
    memset(&s_context, 0, sizeof(s_context));
    s_status.initialized = true;
    portEXIT_CRITICAL(&s_state_lock);

    BaseType_t task_result = xTaskCreate(
        semantic_receiver_task,
        "semantic_receiver",
        6144,
        nullptr,
        5,
        nullptr
    );

    if (task_result != pdPASS) {
        portENTER_CRITICAL(&s_state_lock);
        s_status.initialized = false;
        portEXIT_CRITICAL(&s_state_lock);

        ESP_LOGE(TAG, "Unable to create semantic receiver task");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Semantic event receiver initialized");
    return ESP_OK;
}

semantic_event_receiver_status_t
semantic_event_receiver_get_status(void)
{
    semantic_event_receiver_status_t snapshot;

    portENTER_CRITICAL(&s_state_lock);
    snapshot = s_status;
    portEXIT_CRITICAL(&s_state_lock);

    return snapshot;
}

ambient_context_snapshot_t
semantic_event_receiver_get_ambient_context(void)
{
    ambient_context_snapshot_t snapshot;

    portENTER_CRITICAL(&s_state_lock);
    snapshot = s_context;
    portEXIT_CRITICAL(&s_state_lock);

    if (snapshot.context_received && snapshot.ttl_seconds > 0) {
        uint64_t elapsed_ms = monotonic_ms() - snapshot.received_at_ms;
        snapshot.stale =
            elapsed_ms > ((uint64_t)snapshot.ttl_seconds * 1000ULL);
    }

    return snapshot;
}
