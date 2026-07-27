#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Minimal semantic context extracted from the Semantic Event.
 *
 * This structure intentionally contains only the information required
 * by the Expression Layer. It decouples the firmware from cJSON and
 * allows future expansion without changing the processing pipeline.
 */
typedef struct
{
    const char *user_id;
} semantic_context_t;

/**
 * @brief Process a Semantic Event destined for the RGB Strip Node.
 *
 * @param event_type Semantic event name.
 * @param context Optional semantic context. May be NULL for events that
 *        do not require profile-specific information.
 *
 * @return 0 on success.
 * @return A non-zero value if the event is invalid, unknown, or if the
 *         corresponding visual effect fails.
 */
int expression_processor_process(
    const char *event_type,
    const semantic_context_t *context
);

#ifdef __cplusplus
}
#endif