/**
 * @file network_config.h
 * @brief Public network status API for the Echo Pyramid Voice Node.
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t network_init(void);
bool network_is_connected(void);
const char *network_get_ip(void);

#ifdef __cplusplus
}
#endif
