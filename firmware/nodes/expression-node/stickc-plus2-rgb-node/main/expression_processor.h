#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Processa um evento semântico destinado ao RGB Strip Node.
 *
 * @param event_type Nome do evento semântico.
 *
 * @return 0 em caso de sucesso.
 * @return valor diferente de zero se o evento for inválido,
 *         desconhecido ou se o efeito falhar.
 */
int expression_processor_process(const char *event_type);

#ifdef __cplusplus
}
#endif