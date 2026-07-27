#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Processes one Cognitive Runtime state.
 *
 * The processor maps the normalized Runtime State vocabulary to the
 * corresponding visual effect presented by the StickC Plus 2.
 *
 * Supported states:
 *
 * - idle
 * - presence
 * - listening
 * - thinking
 * - responding
 * - alert
 * - error
 * - offline
 * - learning
 *
 * @param runtime_state Null-terminated normalized Runtime State name.
 *
 * @return 0 when the state is recognized and presented successfully.
 * @return Non-zero ESP-IDF error code when the state is invalid,
 *         unsupported or cannot be presented.
 */
int expression_processor_process(const char *runtime_state);

#ifdef __cplusplus
}
#endif