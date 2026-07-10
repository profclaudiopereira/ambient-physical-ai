#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float temperature_c;
    float humidity_percent;
    float pressure_hpa;
    float light_lux;

    bool wifi_connected;
    bool cognitive_connected;

    bool pahub_ok;
    bool env_iv_ok;
    bool dlight_ok;
    bool mini_oled_ok;
    
} ambient_console_data_t;

/**
 * Inicializa o framebuffer e os recursos do Ambient Runtime Console.
 */
esp_err_t ambient_console_init(void);

/**
 * Renderização estática usada durante o bring-up gráfico.
 *
 * Será mantida temporariamente até a API dinâmica estar validada.
 */
esp_err_t ambient_console_render_static(void);

/**
 * Renderiza o Ambient Runtime Console usando dados reais do runtime.
 */
esp_err_t ambient_console_render(
    const ambient_console_data_t *data
);

#ifdef __cplusplus
}
#endif