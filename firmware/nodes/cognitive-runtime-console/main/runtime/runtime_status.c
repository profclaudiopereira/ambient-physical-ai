/******************************************************************************
 * @file    runtime_status.c
 * @brief   Implements runtime snapshot initialization and JSON ingestion.
 *
 * The module translates external JSON documents into the internal status
 * model consumed by the UI.
 *
 * Parsing is deliberately independent from transport. UART, UDP or another
 * provider only needs to deliver a complete null-terminated JSON document.
 ******************************************************************************/

#include "runtime_status.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "cJSON.h"

/**
 * @brief Copies text into a fixed-size destination buffer safely.
 */
static void copy_string(
    char *destination,
    size_t destination_size,
    const char *source)
{
    if (destination == NULL ||
        destination_size == 0 ||
        source == NULL) {
        return;
    }

    snprintf(destination, destination_size, "%s", source);
}

/**
 * @brief Updates a string field when the JSON member is a valid string.
 */
static void apply_string(
    const cJSON *object,
    const char *field_name,
    char *destination,
    size_t destination_size)
{
    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, field_name);

    if (cJSON_IsString(item) && item->valuestring != NULL) {
        copy_string(
            destination,
            destination_size,
            item->valuestring);
    }
}

/**
 * @brief Updates a Boolean field when the JSON member is valid.
 */
static void apply_boolean(
    const cJSON *object,
    const char *field_name,
    bool *destination)
{
    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, field_name);

    if (cJSON_IsBool(item)) {
        *destination = cJSON_IsTrue(item);
    }
}

/**
 * @brief Reads a bounded integer from a JSON object.
 *
 * Values outside the requested range are rejected rather than silently
 * truncated. This protects the deterministic integer fields in the model.
 */
static bool read_bounded_integer(
    const cJSON *object,
    const char *field_name,
    int minimum,
    int maximum,
    int *value)
{
    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, field_name);

    if (!cJSON_IsNumber(item)) {
        return false;
    }

    const int candidate = item->valueint;

    if (candidate < minimum || candidate > maximum) {
        return false;
    }

    *value = candidate;
    return true;
}

/**
 * @brief Applies service availability fields.
 */
static void apply_services(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *services =
        cJSON_GetObjectItemCaseSensitive(root, "services");

    if (!cJSON_IsObject(services)) {
        return;
    }

    apply_boolean(services, "linux", &status->linux_ok);
    apply_boolean(services, "stackflow", &status->stackflow_ok);
    apply_boolean(services, "mcp", &status->mcp_ok);
    apply_boolean(services, "semantic", &status->semantic_ok);
}

/**
 * @brief Applies identity and environmental context fields.
 */
static void apply_identity(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *identity =
        cJSON_GetObjectItemCaseSensitive(root, "identity");

    if (!cJSON_IsObject(identity)) {
        return;
    }

    apply_string(
        identity,
        "user",
        status->user,
        sizeof(status->user));

    apply_string(
        identity,
        "context",
        status->context,
        sizeof(status->context));
}

/**
 * @brief Applies cognitive state and semantic event fields.
 */
static void apply_cognitive(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *cognitive =
        cJSON_GetObjectItemCaseSensitive(root, "cognitive");

    if (!cJSON_IsObject(cognitive)) {
        return;
    }

    apply_string(
        cognitive,
        "state",
        status->cognitive_state,
        sizeof(status->cognitive_state));

    apply_string(
        cognitive,
        "last_event",
        status->last_event,
        sizeof(status->last_event));

    apply_string(
        cognitive,
        "last_rx",
        status->last_rx,
        sizeof(status->last_rx));
}

/**
 * @brief Applies AX630C health telemetry.
 */
static void apply_health(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *health =
        cJSON_GetObjectItemCaseSensitive(root, "health");

    if (!cJSON_IsObject(health)) {
        return;
    }

    int value;

    if (read_bounded_integer(
            health,
            "cpu_percent",
            0,
            100,
            &value)) {
        status->cpu_percent = (uint8_t)value;
    }

    if (read_bounded_integer(
            health,
            "ram_used_mb",
            0,
            UINT16_MAX,
            &value)) {
        status->ram_used_mb = (uint16_t)value;
    }

    if (read_bounded_integer(
            health,
            "ram_total_mb",
            0,
            UINT16_MAX,
            &value)) {
        status->ram_total_mb = (uint16_t)value;
    }

    if (read_bounded_integer(
            health,
            "temperature_c",
            INT16_MIN,
            INT16_MAX,
            &value)) {
        status->temperature_c = (int16_t)value;
    }

    apply_string(
        health,
        "uptime",
        status->uptime,
        sizeof(status->uptime));
}

/**
 * @brief Applies console-network and runtime-link information.
 */
static void apply_network(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *network =
        cJSON_GetObjectItemCaseSensitive(root, "network");

    if (!cJSON_IsObject(network)) {
        return;
    }

    apply_boolean(
        network,
        "connected",
        &status->network_connected);

    apply_boolean(
        network,
        "heartbeat",
        &status->heartbeat_ok);

    int value;

    if (read_bounded_integer(
            network,
            "rssi_dbm",
            INT16_MIN,
            INT16_MAX,
            &value)) {
        status->rssi_dbm = (int16_t)value;
    }

    apply_string(
        network,
        "ip",
        status->ip_address,
        sizeof(status->ip_address));

    apply_string(
        network,
        "runtime_link",
        status->runtime_link,
        sizeof(status->runtime_link));
}

/**
 * @brief Replaces the bounded event list when a valid array is supplied.
 */
static void apply_event_log(
    runtime_status_t *status,
    const cJSON *root)
{
    const cJSON *events =
        cJSON_GetObjectItemCaseSensitive(root, "events");

    if (!cJSON_IsArray(events)) {
        return;
    }

    memset(status->event_log, 0, sizeof(status->event_log));

    const size_t event_count =
        (size_t)cJSON_GetArraySize(events);

    const size_t copy_count =
        event_count < RUNTIME_LOG_ENTRY_COUNT
            ? event_count
            : RUNTIME_LOG_ENTRY_COUNT;

    for (size_t index = 0; index < copy_count; ++index) {
        const cJSON *entry =
            cJSON_GetArrayItem(events, (int)index);

        if (cJSON_IsString(entry) &&
            entry->valuestring != NULL) {
            copy_string(
                status->event_log[index],
                sizeof(status->event_log[index]),
                entry->valuestring);
        }
    }
}

bool runtime_status_init_mock(runtime_status_t *status)
{
    if (status == NULL) {
        return false;
    }

    memset(status, 0, sizeof(*status));

    status->linux_ok = true;
    status->stackflow_ok = true;
    status->mcp_ok = true;
    status->semantic_ok = true;

    copy_string(status->user, sizeof(status->user), "Unknown");
    copy_string(status->context, sizeof(status->context), "Waiting");
    copy_string(
        status->cognitive_state,
        sizeof(status->cognitive_state),
        "IDLE");
    copy_string(
        status->last_event,
        sizeof(status->last_event),
        "console_initialized");
    copy_string(
        status->last_rx,
        sizeof(status->last_rx),
        "LOCAL FALLBACK");

    status->cpu_percent = 0;
    status->ram_used_mb = 0;
    status->ram_total_mb = 0;
    status->temperature_c = 0;

    copy_string(status->uptime, sizeof(status->uptime), "--");

    status->network_connected = false;
    status->heartbeat_ok = false;
    status->rssi_dbm = 0;

    copy_string(
        status->ip_address,
        sizeof(status->ip_address),
        "0.0.0.0");
    copy_string(
        status->runtime_link,
        sizeof(status->runtime_link),
        "WAITING");

    copy_string(
        status->event_log[0],
        sizeof(status->event_log[0]),
        "Console initialized");

    return true;
}

bool runtime_status_apply_json(
    runtime_status_t *status,
    const char *json)
{
    if (status == NULL || json == NULL) {
        return false;
    }

    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        return false;
    }

    const cJSON *type =
        cJSON_GetObjectItemCaseSensitive(root, "type");

    /*
     * Reject unrelated packets early. The console may later share a transport
     * with semantic events, identity packages or diagnostic messages.
     */
    const bool valid_document =
        cJSON_IsString(type) &&
        type->valuestring != NULL &&
        strcmp(type->valuestring, "runtime_status") == 0;

    if (!valid_document) {
        cJSON_Delete(root);
        return false;
    }

    apply_services(status, root);
    apply_identity(status, root);
    apply_cognitive(status, root);
    apply_health(status, root);
    apply_network(status, root);
    apply_event_log(status, root);

    cJSON_Delete(root);
    return true;
}