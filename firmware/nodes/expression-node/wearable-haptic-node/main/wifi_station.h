#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inicializa o Wi-Fi em modo Station e inicia a conexão.
 *
 * @return 0 em caso de sucesso.
 */
int wifi_station_init(void);

/**
 * Informa se o node recebeu endereço IP.
 */
bool wifi_station_is_connected(void);

#ifdef __cplusplus
}
#endif