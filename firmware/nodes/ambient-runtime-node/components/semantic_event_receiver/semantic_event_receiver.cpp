#include "semantic_event_receiver.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"

static const char *TAG = "semantic-receiver";

#define SEMANTIC_MESSAGE_TYPE_LENGTH 32


/*
 * Internal representation of a Semantic Event.
 *
 * This structure is private to the component for now.
 * It can be promoted to a shared public contract later,
 * after the event schema is fully stabilized.
 */
/**
 * Internal representation of a parsed Semantic Event.
 *
 * The structure remains private because transport parsing is an
 * implementation detail of this component. Only the operational snapshot
 * defined in the public header is exposed to the Ambient Runtime.
 */
typedef struct
{
    char type[SEMANTIC_MESSAGE_TYPE_LENGTH];
    char event_type[SEMANTIC_EVENT_TYPE_LENGTH];
    char target[SEMANTIC_EVENT_TARGET_LENGTH];

} semantic_event_t;
static portMUX_TYPE s_status_lock =
    portMUX_INITIALIZER_UNLOCKED;

static semantic_event_receiver_status_t s_status = {
    .initialized = false,
    .listening = false,
    .event_received = false,
    .received_count = 0,
    .last_event_type = "",
    .last_target = ""
};

static void set_listening(bool listening)
{
    portENTER_CRITICAL(&s_status_lock);
    s_status.listening = listening;
    portEXIT_CRITICAL(&s_status_lock);
}

/**
 * Records the most recently consumed Semantic Event.
 *
 * The receiver exposes a snapshot rather than an event history. Both fields
 * are updated inside the same critical section so callers never observe an
 * event type associated with a target from a different datagram.
 */
static void register_received_event(
    const char *event_type,
    const char *target
)
{
    if (event_type == nullptr || target == nullptr) {
        return;
    }

    portENTER_CRITICAL(&s_status_lock);

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

    portEXIT_CRITICAL(&s_status_lock);
}



/*
 * Extracts a string field from the controlled JSON payload
 * produced by the Cognitive Runtime.
 *
 * This is intentionally a lightweight parser for the current
 * integration milestone. A complete JSON parser can be introduced
 * later if the Semantic Event contract becomes more complex.
 */
static bool extract_json_string_field(
    const char *payload,
    const char *field_name,
    char *output,
    size_t output_size
)
{
    if (
        payload == nullptr ||
        field_name == nullptr ||
        output == nullptr ||
        output_size == 0
    ) {
        return false;
    }

    output[0] = '\0';

    char field_pattern[96];

    int pattern_length = snprintf(
        field_pattern,
        sizeof(field_pattern),
        "\"%s\"",
        field_name
    );

    if (
        pattern_length <= 0 ||
        static_cast<size_t>(pattern_length) >= sizeof(field_pattern)
    ) {
        return false;
    }

    const char *field =
        strstr(payload, field_pattern);

    if (field == nullptr) {
        return false;
    }

    const char *colon =
        strchr(field + pattern_length, ':');

    if (colon == nullptr) {
        return false;
    }

    const char *start =
        strchr(colon, '"');

    if (start == nullptr) {
        return false;
    }

    start++;

    const char *end =
        strchr(start, '"');

    if (end == nullptr) {
        return false;
    }

    size_t length =
        static_cast<size_t>(end - start);

    if (length >= output_size) {
        length = output_size - 1;
    }

    memcpy(
        output,
        start,
        length
    );

    output[length] = '\0';

    return true;
}

/*
 * Parses the controlled Semantic Event payload.
 *
 * Missing fields receive safe fallback values so the receiver
 * remains operational while the event contract evolves.
 */
static void parse_semantic_event(
    const char *payload,
    semantic_event_t *event
)
{
    if (event == nullptr) {
        return;
    }

    memset(
        event,
        0,
        sizeof(*event)
    );

    if (
        !extract_json_string_field(
            payload,
            "type",
            event->type,
            sizeof(event->type)
        )
    ) {
        snprintf(
            event->type,
            sizeof(event->type),
            "unknown"
        );
    }

    if (
        !extract_json_string_field(
            payload,
            "event_type",
            event->event_type,
            sizeof(event->event_type)
        )
    ) {
        snprintf(
            event->event_type,
            sizeof(event->event_type),
            "unknown"
        );
    }

    if (
        !extract_json_string_field(
            payload,
            "target",
            event->target,
            sizeof(event->target)
        )
    ) {
        snprintf(
            event->target,
            sizeof(event->target),
            "unspecified"
        );
    }
}


/*
 * Processes Semantic Events addressed to the Ambient Runtime.
 *
 * This function is intentionally minimal at this milestone.
 * Future versions will dispatch events to the display,
 * Mini OLED, environmental services and other runtime
 * components.
 */
static void process_ambient_runtime_event(
    const semantic_event_t *event
)
{
    if (event == nullptr) {
        return;
    }

    ESP_LOGI(
        TAG,
        "Processing Ambient Runtime event..."
    );

    if (
        strcmp(
            event->target,
            "ambient_runtime"
        ) != 0
    ) {
        ESP_LOGI(
            TAG,
            "Ignoring event for target: %s",
            event->target
        );

        return;
    }

    if (
        strcmp(
            event->event_type,
            "identity_authenticated"
        ) == 0
    ) {
        ESP_LOGI(
            TAG,
            "identity_authenticated received"
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Event '%s' not implemented yet",
        event->event_type
    );
}





/*
 * Local Semantic Event consumer for the Ambient Runtime.
 *
 * At this milestone, it preserves the previously validated
 * behavior: register the event and expose receiver status.
 *
 * Target filtering and Ambient Runtime actions will be introduced
 * in a subsequent controlled milestone.
 */


static void consume_semantic_event(
    const semantic_event_t *event
)
{
    if (event == nullptr) {
        return;
    }

   register_received_event(
      event->event_type,
      event->target
   );

    ESP_LOGI(
        TAG,
        "Semantic event consumed"
    );

    ESP_LOGI(
        TAG,
        "Type: %s",
        event->type
    );

    ESP_LOGI(
        TAG,
        "Event: %s",
        event->event_type
    );

    ESP_LOGI(
        TAG,
        "Target: %s",
        event->target
    );
    process_ambient_runtime_event(
        event
    );
}

static void semantic_receiver_task(void *argument)
{
    (void)argument;

    ESP_LOGI(
        TAG,
        "Semantic receiver task started"
    );

    char receive_buffer[512];

    while (true)
    {
        ESP_LOGI(
            TAG,
            "Creating UDP socket..."
        );

        int socket_fd =
            socket(
                AF_INET,
                SOCK_DGRAM,
                IPPROTO_IP
            );

        if (socket_fd < 0)
        {
            ESP_LOGE(
                TAG,
                "socket() failed errno=%d",
                errno
            );

            vTaskDelay(
                pdMS_TO_TICKS(2000)
            );

            continue;
        }

        ESP_LOGI(
            TAG,
            "Socket created"
        );

        struct sockaddr_in listen_address = {};

        listen_address.sin_family = AF_INET;
        listen_address.sin_port =
            htons(SEMANTIC_EVENT_RECEIVER_PORT);

        listen_address.sin_addr.s_addr =
            htonl(INADDR_ANY);

        ESP_LOGI(
            TAG,
            "Binding UDP port %d...",
            SEMANTIC_EVENT_RECEIVER_PORT
        );

        int bind_result =
            bind(
                socket_fd,
                reinterpret_cast<struct sockaddr *>(
                    &listen_address
                ),
                sizeof(listen_address)
            );

        if (bind_result < 0)
        {
            ESP_LOGE(
                TAG,
                "bind() failed errno=%d",
                errno
            );

            close(socket_fd);

            vTaskDelay(
                pdMS_TO_TICKS(2000)
            );

            continue;
        }

        ESP_LOGI(
            TAG,
            "UDP bind OK"
        );

        set_listening(true);

        ESP_LOGI(
            TAG,
            "Waiting for semantic events..."
        );

        while (true)
        {
            struct sockaddr_in source_address = {};

            socklen_t source_length =
                sizeof(source_address);

            int received =
                recvfrom(
                    socket_fd,
                    receive_buffer,
                    sizeof(receive_buffer) - 1,
                    0,
                    reinterpret_cast<struct sockaddr *>(
                        &source_address
                    ),
                    &source_length
                );

            if (received < 0)
            {
                ESP_LOGE(
                    TAG,
                    "recvfrom() failed errno=%d",
                    errno
                );

                break;
            }

            receive_buffer[received] = '\0';

            ESP_LOGI(
                TAG,
                "Received %d bytes",
                received
            );

            semantic_event_t event = {};

            parse_semantic_event(
                receive_buffer,
                &event
            );

            consume_semantic_event(
                &event
            );

            ESP_LOGI(
                TAG,
                "Payload: %s",
                receive_buffer
            );
        }

        set_listening(false);

        close(socket_fd);

        ESP_LOGW(
            TAG,
            "Receiver restarting..."
        );

        vTaskDelay(
            pdMS_TO_TICKS(1000)
        );
    }
}

esp_err_t semantic_event_receiver_init(void)
{
    if (s_status.initialized)
    {
        ESP_LOGW(
            TAG,
            "Semantic event receiver already initialized"
        );

        return ESP_OK;
    }

    portENTER_CRITICAL(&s_status_lock);

    memset(
        &s_status,
        0,
        sizeof(s_status)
    );

    s_status.initialized = true;

    portEXIT_CRITICAL(&s_status_lock);

    BaseType_t task_result =
        xTaskCreate(
            semantic_receiver_task,
            "semantic_receiver",
            4096,
            nullptr,
            5,
            nullptr
        );

    if (task_result != pdPASS)
    {
        portENTER_CRITICAL(&s_status_lock);
        s_status.initialized = false;
        portEXIT_CRITICAL(&s_status_lock);

        ESP_LOGE(
            TAG,
            "Unable to create semantic receiver task"
        );

        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(
        TAG,
        "Semantic event receiver initialized"
    );

    return ESP_OK;
}

semantic_event_receiver_status_t
semantic_event_receiver_get_status(void)
{
    semantic_event_receiver_status_t snapshot;

    portENTER_CRITICAL(&s_status_lock);
    snapshot = s_status;
    portEXIT_CRITICAL(&s_status_lock);

    return snapshot;
}