/******************************************************************************
 * @file    runtime_status.h
 * @brief   Defines the Cognitive Runtime Console data model and JSON contract.
 *
 * The runtime status model is the stable contract between external data
 * providers and the presentation layer.
 *
 * Data may originate from UART, UDP or another transport, but screen modules
 * always consume the same presentation-neutral runtime_status_t snapshot.
 *
 * Architectural constraints:
 *  - No dependency on LVGL or board-specific drivers.
 *  - Transport modules must not manipulate UI widgets directly.
 *  - JSON updates may be partial; absent fields preserve their current values.
 *  - Fixed-size buffers provide deterministic memory usage.
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RUNTIME_USER_MAX_LENGTH          32
#define RUNTIME_CONTEXT_MAX_LENGTH       32
#define RUNTIME_STATE_MAX_LENGTH         32
#define RUNTIME_EVENT_MAX_LENGTH         64
#define RUNTIME_LAST_RX_MAX_LENGTH       32
#define RUNTIME_IP_MAX_LENGTH            24
#define RUNTIME_LINK_MAX_LENGTH          24
#define RUNTIME_UPTIME_MAX_LENGTH        24
#define RUNTIME_LOG_ENTRY_MAX_LENGTH     64
#define RUNTIME_LOG_ENTRY_COUNT          5

/**
 * @brief Complete runtime snapshot consumed by console screens.
 */
typedef struct
{
    bool linux_ok;
    bool stackflow_ok;
    bool mcp_ok;
    bool semantic_ok;

    char user[RUNTIME_USER_MAX_LENGTH];
    char context[RUNTIME_CONTEXT_MAX_LENGTH];
    char cognitive_state[RUNTIME_STATE_MAX_LENGTH];
    char last_event[RUNTIME_EVENT_MAX_LENGTH];
    char last_rx[RUNTIME_LAST_RX_MAX_LENGTH];

    uint8_t cpu_percent;
    uint16_t ram_used_mb;
    uint16_t ram_total_mb;
    int16_t temperature_c;
    char uptime[RUNTIME_UPTIME_MAX_LENGTH];

    bool network_connected;
    bool heartbeat_ok;
    int16_t rssi_dbm;
    char ip_address[RUNTIME_IP_MAX_LENGTH];
    char runtime_link[RUNTIME_LINK_MAX_LENGTH];

    char event_log[RUNTIME_LOG_ENTRY_COUNT][RUNTIME_LOG_ENTRY_MAX_LENGTH];
} runtime_status_t;

/**
 * @brief Initializes a status instance with deterministic fallback values.
 *
 * These values provide a safe initial interface while the first external
 * runtime packet has not yet been received.
 *
 * @param status Destination model.
 *
 * @return true on success; false when status is NULL.
 */
bool runtime_status_init_mock(runtime_status_t *status);

/**
 * @brief Applies a JSON runtime update to an existing status snapshot.
 *
 * The parser supports partial updates. Fields absent from the JSON document
 * retain their previous values, allowing the AX630C to send either complete
 * snapshots or smaller event-driven updates.
 *
 * Expected top-level sections:
 *
 * @code{.json}
 * {
 *   "type": "runtime_status",
 *   "services": {},
 *   "identity": {},
 *   "cognitive": {},
 *   "health": {},
 *   "network": {},
 *   "events": []
 * }
 * @endcode
 *
 * @param status Destination model to update.
 * @param json   Null-terminated JSON document.
 *
 * @return true when a valid runtime_status document is applied.
 */
bool runtime_status_apply_json(
    runtime_status_t *status,
    const char *json);

#ifdef __cplusplus
}
#endif