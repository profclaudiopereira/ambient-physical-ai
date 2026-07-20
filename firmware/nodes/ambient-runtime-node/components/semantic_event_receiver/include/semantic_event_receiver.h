#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEMANTIC_EVENT_TYPE_LENGTH 64
#define SEMANTIC_EVENT_RECEIVER_PORT 5555

typedef struct {
    bool initialized;
    bool listening;
    bool event_received;

    uint32_t received_count;

    char last_event_type[SEMANTIC_EVENT_TYPE_LENGTH];
} semantic_event_receiver_status_t;

/**
 * Initializes the semantic event receiver.
 *
 * At this initial milestone, the component only initializes
 * its internal state. UDP transport will be introduced next.
 */
esp_err_t semantic_event_receiver_init(void);

/**
 * Returns a snapshot of the current semantic receiver state.
 */
semantic_event_receiver_status_t
semantic_event_receiver_get_status(void);

#ifdef __cplusplus
}
#endif