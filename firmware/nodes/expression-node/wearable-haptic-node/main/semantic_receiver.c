#include "semantic_receiver.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "semantic_consumer.h"

#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define UDP_PORT        5555
#define BUFFER_SIZE     512

static const char *TAG = "semantic_receiver";

static void receiver_task(void *arg)
{
    char rx_buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0)
    {
        ESP_LOGE(TAG, "Cannot create socket");
        vTaskDelete(NULL);
        return;
    }

    if (bind(sock,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "Bind failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Listening on UDP port %d", UDP_PORT);

    while (true)
    {
        struct sockaddr_storage source_addr;
        socklen_t socklen = sizeof(source_addr);

        int len = recvfrom(
            sock,
            rx_buffer,
            sizeof(rx_buffer) - 1,
            0,
            (struct sockaddr *)&source_addr,
            &socklen);

        if (len < 0)
            continue;

        rx_buffer[len] = 0;

        ESP_LOGI(TAG, "UDP received:");
	ESP_LOGI(TAG, "%s", rx_buffer);

	int result = semantic_consumer_process(rx_buffer);

	if (result != 0) {
    	ESP_LOGW(TAG, "Semantic payload was not processed");
	}
    }
}

int semantic_receiver_start(void)
{
    BaseType_t ok = xTaskCreate(
        receiver_task,
        "semantic_receiver",
        4096,
        NULL,
        5,
        NULL);

    return (ok == pdPASS) ? 0 : -1;
}