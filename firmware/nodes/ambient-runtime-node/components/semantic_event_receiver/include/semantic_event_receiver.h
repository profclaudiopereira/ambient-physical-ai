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

/**
 * @brief Operational snapshot of the Semantic Event Receiver.
 *
 * This structure exposes the current receiver state to other Ambient Runtime
 * components. It stores only the most recently consumed event and is not an
 * event queue or persistent history.
 */
typedef struct {
    bool initialized;
    bool listening;
    bool event_received;

    uint32_t received_count;

    /**
     * Semantic name of the most recently consumed event.
     *
     * Example: "identity_authenticated".
     */
    char last_event_type[SEMANTIC_EVENT_TYPE_LENGTH];

    /**
     * Destination declared by the most recently consumed event.
     *
     * Example: "ambient_runtime".
     */
    char last_target[SEMANTIC_EVENT_TARGET_LENGTH];

} semantic_event_receiver_status_t;

/**
 * @brief Initializes the UDP Semantic Event Receiver.
 *
 * Creates the receiver task, binds UDP port 5555 and starts listening for
 * controlled Semantic Event payloads produced by the Cognitive Runtime.
 *
 * @return ESP_OK when the component is initialized successfully.
 * @return ESP_ERR_NO_MEM when the receiver task cannot be created.
 */
esp_err_t semantic_event_receiver_init(void);

/**
 * @brief Returns a thread-safe snapshot of the receiver state.
 *
 * The returned structure is copied while the internal status lock is held,
 * allowing the caller to inspect it without sharing mutable component state.
 *
 * @return Current Semantic Event Receiver status.
 */
semantic_event_receiver_status_t
semantic_event_receiver_get_status(void);

#ifdef __cplusplus
}
#endif