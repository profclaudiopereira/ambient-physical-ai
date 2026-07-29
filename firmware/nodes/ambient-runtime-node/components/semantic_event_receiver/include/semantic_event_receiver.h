#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEMANTIC_EVENT_TYPE_LENGTH 64
#define SEMANTIC_EVENT_TARGET_LENGTH 64
#define SEMANTIC_EVENT_RECEIVER_PORT 5555

#define AMBIENT_CONTEXT_PROFILE_LENGTH 24
#define AMBIENT_CONTEXT_LOCATION_LENGTH 24
#define AMBIENT_CONTEXT_WEATHER_LENGTH 32
#define AMBIENT_CONTEXT_UV_LABEL_LENGTH 16
#define AMBIENT_CONTEXT_PERSONAL_TITLE_LENGTH 24
#define AMBIENT_CONTEXT_PERSONAL_VALUE_LENGTH 32
#define AMBIENT_CONTEXT_PERSONAL_SECONDARY_LENGTH 32

/**
 * Operational snapshot of the UDP receiver.
 *
 * This state remains separate from contextual content so monitoring data and
 * presentation data do not become coupled.
 */
typedef struct {
    bool initialized;
    bool listening;
    bool event_received;
    uint32_t received_count;
    char last_event_type[SEMANTIC_EVENT_TYPE_LENGTH];
    char last_target[SEMANTIC_EVENT_TARGET_LENGTH];
} semantic_event_receiver_status_t;

/**
 * Normalized context selected by the Cognitive Runtime for the Mini OLED.
 *
 * The Tab5 does not choose APIs, profiles, relevance or business rules. It
 * receives already normalized values and only decides the visual layout.
 */
typedef struct {
    bool context_received;
    bool authenticated;
    bool stale;

    uint32_t received_count;
    uint32_t sequence;
    uint32_t ttl_seconds;
    uint64_t received_at_ms;

    char profile_id[AMBIENT_CONTEXT_PROFILE_LENGTH];

    bool global_available;
    char location[AMBIENT_CONTEXT_LOCATION_LENGTH];
    char weather_summary[AMBIENT_CONTEXT_WEATHER_LENGTH];
    float temperature_c;
    float uv_index;
    char uv_label[AMBIENT_CONTEXT_UV_LABEL_LENGTH];

    bool personal_available;
    char personal_title[AMBIENT_CONTEXT_PERSONAL_TITLE_LENGTH];
    char personal_value[AMBIENT_CONTEXT_PERSONAL_VALUE_LENGTH];
    char personal_secondary[AMBIENT_CONTEXT_PERSONAL_SECONDARY_LENGTH];
} ambient_context_snapshot_t;

esp_err_t semantic_event_receiver_init(void);

semantic_event_receiver_status_t
semantic_event_receiver_get_status(void);

/**
 * Returns a thread-safe copy of the most recently accepted ambient context.
 *
 * The stale flag is calculated from monotonic local time and ttl_seconds.
 */
ambient_context_snapshot_t
semantic_event_receiver_get_ambient_context(void);

#ifdef __cplusplus
}
#endif
