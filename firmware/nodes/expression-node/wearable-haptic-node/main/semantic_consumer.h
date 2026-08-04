#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the asynchronous Runtime State consumer.
 *
 * This function creates the internal Runtime State queue and the worker task
 * responsible for presenting accepted states in sequence.
 *
 * The worker applies minimum local presentation times without blocking the
 * UDP receiver or the Cognitive Runtime running on the AX630C.
 *
 * This function must be called once before semantic_consumer_process().
 *
 * @return 0 when initialization succeeds or the consumer is already running.
 * @return Non-zero ESP-IDF error code when the queue or worker task cannot
 *         be created.
 */
int semantic_consumer_init(void);

/**
 * @brief Validates and queues one Runtime State JSON payload.
 *
 * Expected contract:
 *
 * {
 *   "type": "runtime_state",
 *   "target": "runtime_state_indicator",
 *   "state": "thinking"
 * }
 *
 * The function validates the message contract and places the normalized state
 * in the local presentation queue. Visual processing is performed
 * asynchronously by the consumer worker task.
 *
 * @param payload Null-terminated JSON payload received from the network.
 *
 * @return 0 when the Runtime State is accepted and queued successfully.
 * @return Non-zero ESP-IDF error code when the payload is invalid,
 *         unsupported, the consumer is not initialized or the queue is full.
 */
int semantic_consumer_process(const char *payload);

#ifdef __cplusplus
}
#endif