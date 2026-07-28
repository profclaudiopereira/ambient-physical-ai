/**
 * @file network_config.h
 * @brief Network interface for the Echo Pyramid Voice Node.
 *
 * This module centralizes Wi-Fi station initialization and the fixed network
 * identity adopted by the Ambient Physical AI distributed runtime.
 */

#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes Wi-Fi station mode and applies the fixed IPv4 settings.
 *
 * The function initializes NVS, ESP-NETIF, the default event loop, the station
 * interface, static IPv4 configuration, hostname, DNS servers and Wi-Fi.
 *
 * @return ESP_OK when initialization starts successfully, otherwise an
 *         ESP-IDF error code.
 */
esp_err_t network_init(void);

/**
 * @brief Reports whether the station has completed network association.
 *
 * @return true after the station receives the IP event, otherwise false.
 */
bool network_is_connected(void);

/**
 * @brief Returns the most recently confirmed local IPv4 address.
 *
 * Before connection, the function returns the configured static address.
 *
 * @return Null-terminated IPv4 string owned by the module.
 */
const char *network_get_ip(void);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_CONFIG_H */
