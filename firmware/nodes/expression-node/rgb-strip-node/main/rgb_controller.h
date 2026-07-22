#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa o controlador da fita RGB.
 *
 * Configuração atual:
 * - WS2812
 * - GPIO 2
 * - 29 LEDs
 *
 * @return 0 em caso de sucesso.
 * @return valor diferente de zero em caso de falha.
 */
int rgb_controller_init(void);

/**
 * @brief Define a mesma cor para todos os LEDs da fita.
 *
 * @param red   Intensidade do canal vermelho, de 0 a 255.
 * @param green Intensidade do canal verde, de 0 a 255.
 * @param blue  Intensidade do canal azul, de 0 a 255.
 *
 * @return 0 em caso de sucesso.
 * @return valor diferente de zero em caso de falha.
 */
int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
);

/**
 * @brief Apaga todos os LEDs.
 *
 * @return 0 em caso de sucesso.
 * @return valor diferente de zero em caso de falha.
 */
int rgb_controller_clear(void);

#ifdef __cplusplus
}
#endif