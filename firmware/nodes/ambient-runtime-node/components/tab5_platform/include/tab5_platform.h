#pragma once

#include "esp_err.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inicializa toda a infraestrutura da plataforma Tab5:
 * - I²C interno
 * - PI4IOE1
 * - Reset LCD/Touch
 * - Backlight
 * - MIPI DSI
 * - ST7121
 */
esp_err_t tab5_platform_init(void);

/**
 * Ajusta o brilho do backlight.
 */
esp_err_t tab5_platform_backlight_set(int brightness_percent);

/**
 * Preenche a tela inteira com uma cor RGB565.
 */
esp_err_t tab5_platform_fill(uint16_t color);

#ifdef __cplusplus
}
#endif