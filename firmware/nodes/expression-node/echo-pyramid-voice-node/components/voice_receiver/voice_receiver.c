/**
 * @file voice_receiver.c
 * @brief Streaming TCP receiver for signed 16-bit PCM voice responses.
 *
 * Protocol V1 header (16 bytes):
 *
 *   Bytes 0..3   Magic: "APAI"
 *   Byte  4      Version: 1
 *   Byte  5      Channels: 1
 *   Byte  6      Bits per sample: 16
 *   Byte  7      Reserved: 0
 *   Bytes 8..11  Sample rate, unsigned 32-bit network byte order
 *   Bytes 12..15 Total sample count, unsigned 32-bit network byte order
 *
 * Payload:
 *   Signed 16-bit little-endian mono PCM samples.
 *
 * TCP was selected for V1 because it provides ordered and reliable delivery
 * without implementing UDP fragmentation/reassembly inside the Voice Node.
 */

#include "voice_receiver.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define VOICE_HEADER_SIZE            16
#define VOICE_PROTOCOL_VERSION       1
#define VOICE_EXPECTED_CHANNELS      1
#define VOICE_EXPECTED_BITS          16
#define VOICE_EXPECTED_SAMPLE_RATE   16000
#define VOICE_STREAM_BLOCK_SAMPLES   1024
#define VOICE_MAX_SAMPLE_COUNT       (VOICE_EXPECTED_SAMPLE_RATE * 60U)
#define VOICE_SOCKET_TIMEOUT_SECONDS 10
#define VOICE_TASK_STACK_SIZE        6144
#define VOICE_TASK_PRIORITY          6

static const char *TAG = "voice_receiver";

typedef struct {
    uint16_t port;
    voice_receiver_playback_cb_t playback_cb;
} voice_receiver_context_t;

static uint32_t read_u32_be(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

static bool receive_exact(int socket_fd, void *buffer, size_t length)
{
    uint8_t *cursor = (uint8_t *)buffer;
    size_t received_total = 0;

    while (received_total < length) {
        int received = recv(
            socket_fd,
            cursor + received_total,
            length - received_total,
            0);

        if (received == 0) {
            ESP_LOGW(TAG, "Voice client closed the connection early");
            return false;
        }

        if (received < 0) {
            if (errno == EINTR) {
                continue;
            }

            ESP_LOGE(TAG, "TCP receive failed: errno=%d", errno);
            return false;
        }

        received_total += (size_t)received;
    }

    return true;
}

static bool validate_header(
    const uint8_t header[VOICE_HEADER_SIZE],
    uint32_t *sample_count)
{
    if (memcmp(header, "APAI", 4) != 0) {
        ESP_LOGE(TAG, "Invalid voice stream magic");
        return false;
    }

    if (header[4] != VOICE_PROTOCOL_VERSION) {
        ESP_LOGE(TAG, "Unsupported voice protocol version: %u", header[4]);
        return false;
    }

    if (header[5] != VOICE_EXPECTED_CHANNELS ||
        header[6] != VOICE_EXPECTED_BITS) {
        ESP_LOGE(TAG,
                 "Unsupported PCM format: channels=%u bits=%u",
                 header[5],
                 header[6]);
        return false;
    }

    uint32_t sample_rate = read_u32_be(&header[8]);
    uint32_t total_samples = read_u32_be(&header[12]);

    if (sample_rate != VOICE_EXPECTED_SAMPLE_RATE) {
        ESP_LOGE(TAG,
                 "Unsupported sample rate: %lu Hz",
                 (unsigned long)sample_rate);
        return false;
    }

    if (total_samples == 0 ||
        total_samples > VOICE_MAX_SAMPLE_COUNT) {
        ESP_LOGE(TAG,
                 "Invalid sample count: %lu",
                 (unsigned long)total_samples);
        return false;
    }

    *sample_count = total_samples;
    return true;
}

static esp_err_t consume_voice_stream(
    int client_socket,
    voice_receiver_playback_cb_t playback_cb)
{
    uint8_t header[VOICE_HEADER_SIZE];

    if (!receive_exact(client_socket, header, sizeof(header))) {
        return ESP_FAIL;
    }

    uint32_t total_samples = 0;
    if (!validate_header(header, &total_samples)) {
        return ESP_ERR_INVALID_ARG;
    }

    int16_t *pcm_block = heap_caps_malloc(
        VOICE_STREAM_BLOCK_SAMPLES * sizeof(int16_t),
        MALLOC_CAP_8BIT);

    if (pcm_block == NULL) {
        ESP_LOGE(TAG, "Unable to allocate PCM receive block");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG,
             "Voice stream accepted: %lu samples, 16000 Hz, mono, 16-bit",
             (unsigned long)total_samples);

    uint32_t remaining_samples = total_samples;
    esp_err_t result = ESP_OK;

    while (remaining_samples > 0) {
        size_t block_samples =
            remaining_samples > VOICE_STREAM_BLOCK_SAMPLES
                ? VOICE_STREAM_BLOCK_SAMPLES
                : (size_t)remaining_samples;

        size_t block_bytes = block_samples * sizeof(int16_t);

        if (!receive_exact(client_socket, pcm_block, block_bytes)) {
            result = ESP_FAIL;
            break;
        }

        result = playback_cb(pcm_block, block_samples);
        if (result != ESP_OK) {
            ESP_LOGE(TAG,
                     "PCM playback callback failed: %s",
                     esp_err_to_name(result));
            break;
        }

        remaining_samples -= (uint32_t)block_samples;
    }

    heap_caps_free(pcm_block);

    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Voice stream playback completed");
    }

    return result;
}

static void voice_receiver_task(void *argument)
{
    voice_receiver_context_t context =
        *(voice_receiver_context_t *)argument;

    heap_caps_free(argument);

    while (true) {
        int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (listen_socket < 0) {
            ESP_LOGE(TAG, "Unable to create TCP listening socket");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        int reuse_address = 1;
        setsockopt(
            listen_socket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_address,
            sizeof(reuse_address));

        struct sockaddr_in server_address = {
            .sin_family = AF_INET,
            .sin_port = htons(context.port),
            .sin_addr.s_addr = htonl(INADDR_ANY),
        };

        if (bind(
                listen_socket,
                (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0) {
            ESP_LOGE(TAG, "TCP bind failed: errno=%d", errno);
            close(listen_socket);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        if (listen(listen_socket, 1) < 0) {
            ESP_LOGE(TAG, "TCP listen failed: errno=%d", errno);
            close(listen_socket);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG,
                 "Voice receiver listening on TCP port %u",
                 context.port);

        while (true) {
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);

            int client_socket = accept(
                listen_socket,
                (struct sockaddr *)&client_address,
                &client_address_length);

            if (client_socket < 0) {
                ESP_LOGE(TAG, "TCP accept failed: errno=%d", errno);
                break;
            }

            struct timeval timeout = {
                .tv_sec = VOICE_SOCKET_TIMEOUT_SECONDS,
                .tv_usec = 0,
            };

            setsockopt(
                client_socket,
                SOL_SOCKET,
                SO_RCVTIMEO,
                &timeout,
                sizeof(timeout));

            ESP_LOGI(TAG, "Voice client connected");
            consume_voice_stream(client_socket, context.playback_cb);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
        }

        close(listen_socket);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

esp_err_t voice_receiver_start(
    uint16_t port,
    voice_receiver_playback_cb_t playback_cb)
{
    if (port == 0 || playback_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    voice_receiver_context_t *context = heap_caps_malloc(
        sizeof(*context),
        MALLOC_CAP_8BIT);

    if (context == NULL) {
        return ESP_ERR_NO_MEM;
    }

    context->port = port;
    context->playback_cb = playback_cb;

    BaseType_t created = xTaskCreate(
        voice_receiver_task,
        "voice_receiver",
        VOICE_TASK_STACK_SIZE,
        context,
        VOICE_TASK_PRIORITY,
        NULL);

    if (created != pdPASS) {
        heap_caps_free(context);
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}
