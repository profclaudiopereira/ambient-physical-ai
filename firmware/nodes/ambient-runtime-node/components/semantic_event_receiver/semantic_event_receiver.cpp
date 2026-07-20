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

static portMUX_TYPE s_status_lock =
    portMUX_INITIALIZER_UNLOCKED;

static semantic_event_receiver_status_t s_status = {
    .initialized = false,
    .listening = false,
    .event_received = false,
    .received_count = 0,
    .last_event_type = ""
};

static void set_listening(bool listening)
{
    portENTER_CRITICAL(&s_status_lock);
    s_status.listening = listening;
    portEXIT_CRITICAL(&s_status_lock);
}

static void register_received_event(
    const char *event_type
)
{
    portENTER_CRITICAL(&s_status_lock);

    s_status.event_received = true;
    s_status.received_count++;

    snprintf(
        s_status.last_event_type,
        sizeof(s_status.last_event_type),
        "%s",
        event_type
    );

    portEXIT_CRITICAL(&s_status_lock);
}

/*
 * Extração mínima do campo event_type.
 *
 * Este milestone não implementa ainda um parser JSON completo.
 * Ele apenas reconhece o contrato controlado enviado pelo AX630C.
 */
static void extract_event_type(
    const char *payload,
    char *event_type,
    size_t event_type_size
)
{
    snprintf(
        event_type,
        event_type_size,
        "unknown"
    );

    const char *field =
        strstr(payload, "\"event_type\"");

    if (field == nullptr) {
        return;
    }

    const char *colon =
        strchr(field, ':');

    if (colon == nullptr) {
        return;
    }

    const char *start =
        strchr(colon, '"');

    if (start == nullptr) {
        return;
    }

    start++;

    const char *end =
        strchr(start, '"');

    if (end == nullptr) {
        return;
    }

    size_t length =
        static_cast<size_t>(end - start);

    if (length >= event_type_size) {
        length = event_type_size - 1;
    }

    memcpy(
        event_type,
        start,
        length
    );

    event_type[length] = '\0';
}

static void semantic_receiver_task(void *argument)
{
    (void)argument;

    ESP_LOGI(TAG, "Semantic receiver task started");

    char receive_buffer[512];

    while (true)
    {
        ESP_LOGI(TAG, "Creating UDP socket...");

        int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (socket_fd < 0)
        {
            ESP_LOGE(TAG, "socket() failed errno=%d", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Socket created");

        struct sockaddr_in listen_address = {};

        listen_address.sin_family = AF_INET;
        listen_address.sin_port = htons(SEMANTIC_EVENT_RECEIVER_PORT);
        listen_address.sin_addr.s_addr = htonl(INADDR_ANY);

        ESP_LOGI(TAG, "Binding UDP port %d...", SEMANTIC_EVENT_RECEIVER_PORT);

        int bind_result = bind(
            socket_fd,
            (struct sockaddr *)&listen_address,
            sizeof(listen_address));

        if (bind_result < 0)
        {
            ESP_LOGE(TAG,
                     "bind() failed errno=%d",
                     errno);

            close(socket_fd);

            vTaskDelay(pdMS_TO_TICKS(2000));

            continue;
        }

        ESP_LOGI(TAG, "UDP bind OK");

        set_listening(true);

        ESP_LOGI(TAG,
                 "Waiting for semantic events...");

        while (true)
        {
            struct sockaddr_in source_address;

            socklen_t source_length =
                sizeof(source_address);

            int received =
                recvfrom(socket_fd,
                         receive_buffer,
                         sizeof(receive_buffer) - 1,
                         0,
                         (struct sockaddr *)&source_address,
                         &source_length);

            if (received < 0)
            {
                ESP_LOGE(TAG,
                         "recvfrom() failed errno=%d",
                         errno);

                break;
            }

            receive_buffer[received] = '\0';

            ESP_LOGI(TAG,
                     "Received %d bytes",
                     received);

            char event_type[SEMANTIC_EVENT_TYPE_LENGTH];

            extract_event_type(
                receive_buffer,
                event_type,
                sizeof(event_type));

            register_received_event(event_type);

            ESP_LOGI(TAG,
                     "Event: %s",
                     event_type);

            ESP_LOGI(TAG,
                     "Payload: %s",
                     receive_buffer);
        }

        set_listening(false);

        close(socket_fd);

        ESP_LOGW(TAG,
                 "Receiver restarting...");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

esp_err_t semantic_event_receiver_init(void)
{
    if (s_status.initialized) {
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

    if (task_result != pdPASS) {
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