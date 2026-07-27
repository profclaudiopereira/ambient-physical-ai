#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Optional semantic context forwarded with a normalized event.
 *
 * The Atom Matrix RGB Node currently consumes the authenticated user
 * identifier. The structure can be extended later without changing the
 * event-processing contract.
 */
typedef struct
{
    const char *user_id;
} semantic_context_t;

/**
 * @brief Processes a semantic event destined for the Atom Matrix RGB Node.
 *
 * @param event_type Normalized semantic event name.
 * @param context Optional event context. Pass NULL when the event has no
 *                associated profile information.
 *
 * @return 0 on success.
 * @return Non-zero when the event is invalid, unsupported, or the visual
 *         effect cannot be applied.
 */
int expression_processor_process(
    const char *event_type,
    const semantic_context_t *context
);

#ifdef __cplusplus
}
#endif
