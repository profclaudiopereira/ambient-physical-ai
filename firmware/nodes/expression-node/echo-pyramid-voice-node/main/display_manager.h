/**
 * @file display_manager.h
 * @brief Local status display API for the Echo Pyramid Voice Node.
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t display_manager_init(void);
esp_err_t display_show_boot(void);
esp_err_t display_show_ready(const char *ip_address);
esp_err_t display_show_welcome(const char *user_name);
esp_err_t display_show_error(const char *message);

#ifdef __cplusplus
}
#endif
