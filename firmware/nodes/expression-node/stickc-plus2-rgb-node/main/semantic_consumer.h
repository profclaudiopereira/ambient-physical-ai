#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Valida e processa um pacote semantic_event em JSON.
 *
 * @param payload JSON recebido pela rede.
 *
 * @return 0 em caso de sucesso.
 * @return valor diferente de zero em caso de erro.
 */
int semantic_consumer_process(const char *payload);

#ifdef __cplusplus
}
#endif