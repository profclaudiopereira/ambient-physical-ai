#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AMBIENT_NETWORK_IPV4_LENGTH 16

typedef struct {
    bool initialized;
    bool connected;
    bool network_ready;

    char ipv4[AMBIENT_NETWORK_IPV4_LENGTH];
    char netmask[AMBIENT_NETWORK_IPV4_LENGTH];
    char gateway[AMBIENT_NETWORK_IPV4_LENGTH];

    int8_t rssi_dbm;
} ambient_network_status_t;

/**
 * Inicializa Wi-Fi Station através do ESP-Hosted/Wi-Fi Remote.
 *
 * A conexão ocorre de forma assíncrona.
 */
esp_err_t ambient_network_init(void);

/**
 * Retorna um snapshot do estado atual da rede.
 *
 * Quando conectado, a função também atualiza o RSSI.
 */
ambient_network_status_t ambient_network_get_status(void);

#ifdef __cplusplus
}
#endif