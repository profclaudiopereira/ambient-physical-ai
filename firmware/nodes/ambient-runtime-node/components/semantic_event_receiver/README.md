# Semantic Event Receiver

## Ambient Physical AI

### Semantic Context Gateway for the Ambient Runtime

---

# Overview

The `semantic_event_receiver` component provides the communication gateway between the Cognitive Runtime and the Ambient Runtime.

It receives semantic messages through UDP, validates their contents, normalizes accepted information and exposes thread-safe runtime snapshots to the remainder of the Ambient Runtime.

The component intentionally separates communication concerns from presentation logic.

It does not render information, control displays or execute business decisions.

Instead, it transforms external semantic messages into normalized runtime data structures that can be safely consumed by other runtime components. 

---

# Purpose

The purpose of the `semantic_event_receiver` component is to isolate all semantic communication from the rest of the Ambient Runtime.

Current responsibilities include:

- creating the UDP receiver task;
- listening for runtime messages;
- validating JSON payloads;
- identifying supported message types;
- normalizing semantic information;
- maintaining thread-safe runtime snapshots;
- tracking receiver operational status;
- monitoring context expiration.

The component intentionally avoids presentation logic and application-specific rendering.

---

# Position within the Ambient Runtime

The Semantic Event Receiver represents the communication boundary between the Cognitive Runtime and the embedded Ambient Runtime.

```text
Cognitive Runtime
        │
        ▼
UDP Messages
        │
        ▼
semantic_event_receiver
        │
        ├──────────────┐
        │              │
        ▼              ▼
Receiver Status   Ambient Context
        │              │
        └──────┬───────┘
               ▼
Ambient Runtime
        │
        ▼
oled_context_presenter
        │
        ▼
Mini OLED
```

The component converts external communication into normalized runtime information without exposing communication details to the remainder of the system.

---

# Responsibilities

The `semantic_event_receiver` component is responsible exclusively for semantic communication and context normalization.

## UDP Reception

Create and maintain a UDP listener on the configured runtime port.

The receiver continuously waits for incoming semantic messages and automatically restarts itself after communication failures. 

---

## Message Validation

Validate incoming JSON payloads before processing.

Messages with:

- invalid JSON;
- missing mandatory fields;
- unsupported message types;
- incomplete semantic information;

are ignored without affecting the remainder of the runtime. :contentReference[oaicite:2]{index=2}

---

## Semantic Message Dispatch

After successful validation, the component dispatches messages according to their declared type.

Current supported message types include:

```text
semantic_event

ambient_context
```

Additional message types may be introduced in future versions without changing the public API. 

---

## Context Normalization

Convert accepted Ambient Context messages into a normalized runtime structure.

The resulting snapshot contains only engineering data required by the embedded runtime.

Business decisions remain external to the component.

As documented by the public interface:

> The Tab5 does not choose APIs, profiles, relevance or business rules. It receives already normalized values and only decides the visual layout. :contentReference[oaicite:4]{index=4}

---

## Receiver Status Monitoring

Maintain an operational snapshot describing the receiver itself.

The status snapshot includes:

- initialization state;
- listening state;
- event reception status;
- received event counter;
- most recently accepted event;
- most recently accepted target.

This operational information remains intentionally separate from contextual content. :contentReference[oaicite:5]{index=5}

---

## Ambient Context Management

Maintain the most recently accepted Ambient Context.

The component stores:

- authentication state;
- profile identifier;
- global context;
- personalized context;
- sequence number;
- reception timestamp;
- time-to-live information.

The stored snapshot becomes the single source of contextual information for presentation components. 

---

# Message Flow

The communication pipeline follows a deterministic sequence.

```text
Cognitive Runtime
        │
        ▼
UDP Packet
        │
        ▼
JSON Validation
        │
        ▼
Message Type Detection
        │
        ▼
Context Normalization
        │
        ▼
Thread-Safe Snapshot
        │
        ▼
Ambient Runtime
```

The component completely encapsulates network communication, JSON parsing and message dispatch.

Higher-level runtime modules consume only normalized runtime structures.

---

# Supported Message Types

The current implementation recognizes two message categories.

## Semantic Events

Semantic Events notify the Ambient Runtime that an engineering event has occurred.

Current supported event handling includes:

- event registration;
- operational logging;
- runtime dispatch.

Unsupported events are safely ignored after logging. :contentReference[oaicite:7]{index=7}

---

## Ambient Context

Ambient Context messages contain normalized information prepared by the Cognitive Runtime for presentation within the Ambient Runtime.

The component extracts:

- authentication state;
- profile identifier;
- global context;
- personalized context;
- sequence number;
- context lifetime.

The resulting information is stored as a normalized snapshot. 

---

# Data Model

The component exposes two independent runtime snapshots.

## Receiver Status

```text
semantic_event_receiver_status_t
```

This structure represents the operational state of the receiver itself.

It intentionally contains no presentation data. :contentReference[oaicite:9]{index=9}

---

## Ambient Context Snapshot

```text
ambient_context_snapshot_t
```

This structure represents the most recently accepted semantic context.

It contains only normalized information required by presentation components.

Neither JSON objects nor protocol-specific information are exposed outside this component. :contentReference[oaicite:10]{index=10}

---

# Public API

The component exposes three public services.

## Receiver Initialization

```c
esp_err_t semantic_event_receiver_init(void);
```

Initializes the receiver and creates the background UDP listener task.

Repeated initialization requests are safely ignored after successful initialization. 

---

## Receiver Status

```c
semantic_event_receiver_status_t
semantic_event_receiver_get_status(void);
```

Returns a thread-safe snapshot describing the operational state of the receiver.

---

## Ambient Context

```c
ambient_context_snapshot_t
semantic_event_receiver_get_ambient_context(void);
```

Returns a thread-safe copy of the most recently accepted Ambient Context.

The component automatically evaluates context expiration before returning the snapshot by comparing the locally measured elapsed time with the received `ttl_seconds` value. 

---

# Dependencies

The component depends on:

- `json`;
- `esp_timer`;
- `lwip`.

Its ESP-IDF component registration is:

```cmake
idf_component_register(
    SRCS "semantic_event_receiver.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        json
        esp_timer
        lwip
)
```

The component intentionally has no dependency on:

- `oled_sh1107`;
- `ambient_console`;
- sensor drivers;
- display controllers;
- presentation components.

Communication and presentation remain cleanly separated. :contentReference[oaicite:13]{index=13}

---

# JSON Processing

The `semantic_event_receiver` processes each received UDP payload as a JSON document.

The processing sequence begins by parsing the complete payload with `cJSON`.

```text
UDP Payload
        │
        ▼
cJSON_Parse()
        │
   ┌────┴────┐
   │         │
Invalid     Valid
   │         │
   ▼         ▼
Ignore   Read "type"
```

Invalid JSON is rejected immediately and does not modify the current receiver status or Ambient Context snapshot. :contentReference[oaicite:0]{index=0}

---

# Message Type Detection

Every accepted JSON payload must contain a string field named:

```text
type
```

The component currently recognizes:

```text
semantic_event
```

and:

```text
ambient_context
```

Messages without a valid `type` field are ignored.

Messages declaring unsupported types are logged and discarded without affecting the current runtime snapshots. :contentReference[oaicite:1]{index=1}

---

# Semantic Event Processing

A `semantic_event` message must contain:

```text
event_type
target
```

The internal parser copies these values into a temporary event structure.

A message is accepted only when both fields are present and non-empty.

```text
semantic_event
        │
        ▼
Read event_type
        │
        ▼
Read target
        │
        ▼
Validate Required Fields
        │
   ┌────┴────┐
   │         │
Invalid     Valid
   │         │
   ▼         ▼
Ignore   Register Event
```

Accepted events update the receiver operational status, including:

- `event_received`;
- `received_count`;
- `last_event_type`;
- `last_target`. 

---

# Event Target Filtering

After registration, Semantic Events may be evaluated according to their target.

The current implementation recognizes the target:

```text
ambient_runtime
```

Events directed to other targets are ignored after logging.

For the `ambient_runtime` target, the currently recognized event is:

```text
identity_authenticated
```

The event is acknowledged operationally, but no additional application action is currently implemented.

Other event types are logged as not implemented. :contentReference[oaicite:3]{index=3}

---

# Ambient Context Processing

An `ambient_context` message must contain two object fields:

```text
global
personal
```

If either field is missing or is not a JSON object, the message is rejected.

The parser then normalizes the message into:

```text
ambient_context_snapshot_t
```

The normalization process clears the destination structure before populating it, preventing fields from a previous context from remaining in a newly accepted snapshot. :contentReference[oaicite:4]{index=4}

---

# Context Normalization

The normalized Ambient Context contains three categories of data.

## Context Metadata

The root message provides:

- authentication state;
- sequence number;
- time-to-live;
- profile identifier;
- local reception timestamp.

The component uses fallback values when optional numeric or Boolean fields are absent.

The default context lifetime is:

```text
900 seconds
```

A received TTL value of zero is also replaced by the same default. :contentReference[oaicite:5]{index=5}

---

## Global Context

The `global` object provides information intended for general presentation.

Current normalized fields include:

- availability;
- location;
- weather summary;
- temperature;
- UV index;
- UV label.

The receiver does not retrieve or calculate these values.

They must already be selected and normalized by the Cognitive Runtime. 

---

## Personal Context

The `personal` object provides information associated with the authenticated profile.

Current normalized fields include:

- availability;
- title;
- primary value;
- secondary value.

The receiver does not determine which personal content is relevant.

It only copies accepted values into the internal snapshot. 

---

# String Handling

All JSON strings are copied into fixed-size buffers defined by the public data model.

The helper routine:

```text
copy_json_string()
```

performs the following operations:

- validates the destination pointer;
- clears the destination;
- verifies that the JSON field is a string;
- copies the value using bounded formatting.

This approach prevents direct exposure of dynamically allocated JSON strings to the rest of the runtime. :contentReference[oaicite:8]{index=8}

---

# Fallback Handling

Optional Boolean and numeric values are read through dedicated helper functions.

```text
json_bool_or()
json_number_or()
```

These helpers return predefined fallback values when:

- a field is missing;
- the JSON type is incorrect;
- the value cannot be interpreted as the expected type.

This behavior allows the parser to handle incomplete optional metadata without exposing partially initialized values. :contentReference[oaicite:9]{index=9}

---

# Thread Safety

The receiver task and the main Ambient Runtime loop access shared state concurrently.

To protect this state, the component uses:

```text
portMUX_TYPE
```

with critical sections around all shared snapshot updates and reads.

Protected state includes:

- receiver operational status;
- most recent Semantic Event metadata;
- current Ambient Context snapshot;
- initialization state;
- listening state. :contentReference[oaicite:10]{index=10}

---

# Thread-Safe Snapshot Model

The component does not return direct pointers to its internal state.

Instead, every public getter returns a value copy.

```text
Internal Shared State
        │
        ▼
Critical Section
        │
        ▼
Copy Snapshot
        │
        ▼
Release Lock
        │
        ▼
Return Value
```

This model prevents callers from modifying the internal receiver state and minimizes the time spent inside critical sections. 

---

# Receiver Status Synchronization

The following functions modify operational state inside protected critical sections:

- listening-state updates;
- accepted event registration;
- initialization-state updates.

The public receiver-status getter also copies the complete status structure while holding the same lock. :contentReference[oaicite:12]{index=12}

---

# Ambient Context Synchronization

When a new Ambient Context is accepted, the component:

1. enters the critical section;
2. calculates the next reception count;
3. replaces the complete previous snapshot;
4. restores the incremented count;
5. exits the critical section.

This full-structure replacement ensures that consumers never observe a partially updated context. :contentReference[oaicite:13]{index=13}

---

# TTL Management

The component evaluates whether the current Ambient Context is still valid using:

- `received_at_ms`;
- `ttl_seconds`;
- monotonic local time.

The reception timestamp is captured when the context is parsed.

```text
Context Received
        │
        ▼
Store Monotonic Timestamp
        │
        ▼
Later Snapshot Request
        │
        ▼
Calculate Elapsed Time
        │
        ▼
Compare with TTL
        │
   ┌────┴────┐
   │         │
Within TTL  Expired
   │         │
   ▼         ▼
Fresh      Stale
```

The calculated `stale` value is applied to the returned copy rather than modifying the stored context. 

---

# Monotonic Time

The implementation uses:

```text
esp_timer_get_time()
```

to calculate elapsed time.

This provides monotonic local timing independent from:

- wall-clock time;
- timezone;
- NTP availability;
- date configuration.

This is appropriate for context-expiration evaluation in an embedded runtime. :contentReference[oaicite:15]{index=15}

---

# Receiver Task

The UDP receiver operates in a dedicated FreeRTOS task named:

```text
semantic_receiver
```

The current task configuration uses:

```text
Stack size: 6144 bytes
Priority:   5
```

The task is created during `semantic_event_receiver_init()`. :contentReference[oaicite:16]{index=16}

---

# Socket Lifecycle

The receiver task manages the complete UDP socket lifecycle.

```text
Create Socket
        │
        ▼
Bind UDP Port
        │
        ▼
Set Listening State
        │
        ▼
Receive Messages
        │
        ▼
Dispatch JSON
        │
        ▼
Socket Error
        │
        ▼
Close Socket
        │
        ▼
Clear Listening State
        │
        ▼
Restart Receiver
```

The socket listens on all local interfaces through:

```text
INADDR_ANY
```

using UDP port:

```text
5555
```

The port is defined by:

```c
#define SEMANTIC_EVENT_RECEIVER_PORT 5555
``` 

---

# Receive Buffer

The current UDP receive buffer supports payloads up to:

```text
1023 bytes of message content
```

with one additional byte reserved for string termination.

The buffer size is defined internally as:

```c
#define UDP_RECEIVE_BUFFER_LENGTH 1024
```

Received payloads are terminated with `'\0'` before being passed to the JSON parser. :contentReference[oaicite:18]{index=18}

---

# Automatic Recovery

The receiver task includes automatic recovery for socket and bind failures.

Current recovery behavior includes:

- waiting two seconds after socket-creation failure;
- waiting two seconds after bind failure;
- restarting one second after a receive-loop failure.

This recovery mechanism keeps the receiver operational without requiring the main application to recreate the component. :contentReference[oaicite:19]{index=19}

---

# Engineering Principles

## Communication Encapsulation

UDP sockets, LwIP structures and JSON parsing remain private to the component.

Higher-level runtime modules never process transport-specific objects.

---

## Operational and Contextual Separation

Receiver monitoring data is stored separately from Ambient Context data.

This prevents communication health information from becoming coupled to presentation content. :contentReference[oaicite:20]{index=20}

---

## Normalized Data Contracts

The component exposes only fixed-size, typed C structures.

No `cJSON` objects, socket handles or dynamically allocated strings cross the public API boundary.

---

## Passive Embedded Runtime

The Ambient Runtime receives semantic information selected by the Cognitive Runtime.

It does not choose:

- external APIs;
- profiles;
- content relevance;
- personalization policies;
- business rules.

The embedded runtime consumes normalized values and presents them through dedicated components. :contentReference[oaicite:21]{index=21}

---

## Graceful Message Rejection

Malformed, incomplete and unsupported messages are rejected without replacing valid existing snapshots.

This preserves runtime continuity when invalid network traffic is received.

---

## Recoverable Receiver Operation

Socket failures do not permanently terminate semantic communication.

The receiver task recreates its socket and resumes listening after a controlled delay.

---

# JSON Processing

The `semantic_event_receiver` processes each received UDP payload as a JSON document.

The processing sequence begins by parsing the complete payload with `cJSON`.

```text
UDP Payload
        │
        ▼
cJSON_Parse()
        │
   ┌────┴────┐
   │         │
Invalid     Valid
   │         │
   ▼         ▼
Ignore   Read "type"
```

Invalid JSON is rejected immediately and does not modify the current receiver status or Ambient Context snapshot. :contentReference[oaicite:0]{index=0}

---

# Message Type Detection

Every accepted JSON payload must contain a string field named:

```text
type
```

The component currently recognizes:

```text
semantic_event
```

and:

```text
ambient_context
```

Messages without a valid `type` field are ignored.

Messages declaring unsupported types are logged and discarded without affecting the current runtime snapshots. :contentReference[oaicite:1]{index=1}

---

# Semantic Event Processing

A `semantic_event` message must contain:

```text
event_type
target
```

The internal parser copies these values into a temporary event structure.

A message is accepted only when both fields are present and non-empty.

```text
semantic_event
        │
        ▼
Read event_type
        │
        ▼
Read target
        │
        ▼
Validate Required Fields
        │
   ┌────┴────┐
   │         │
Invalid     Valid
   │         │
   ▼         ▼
Ignore   Register Event
```

Accepted events update the receiver operational status, including:

- `event_received`;
- `received_count`;
- `last_event_type`;
- `last_target`. 

---

# Event Target Filtering

After registration, Semantic Events may be evaluated according to their target.

The current implementation recognizes the target:

```text
ambient_runtime
```

Events directed to other targets are ignored after logging.

For the `ambient_runtime` target, the currently recognized event is:

```text
identity_authenticated
```

The event is acknowledged operationally, but no additional application action is currently implemented.

Other event types are logged as not implemented. :contentReference[oaicite:3]{index=3}

---

# Ambient Context Processing

An `ambient_context` message must contain two object fields:

```text
global
personal
```

If either field is missing or is not a JSON object, the message is rejected.

The parser then normalizes the message into:

```text
ambient_context_snapshot_t
```

The normalization process clears the destination structure before populating it, preventing fields from a previous context from remaining in a newly accepted snapshot. :contentReference[oaicite:4]{index=4}

---

# Context Normalization

The normalized Ambient Context contains three categories of data.

## Context Metadata

The root message provides:

- authentication state;
- sequence number;
- time-to-live;
- profile identifier;
- local reception timestamp.

The component uses fallback values when optional numeric or Boolean fields are absent.

The default context lifetime is:

```text
900 seconds
```

A received TTL value of zero is also replaced by the same default. :contentReference[oaicite:5]{index=5}

---

## Global Context

The `global` object provides information intended for general presentation.

Current normalized fields include:

- availability;
- location;
- weather summary;
- temperature;
- UV index;
- UV label.

The receiver does not retrieve or calculate these values.

They must already be selected and normalized by the Cognitive Runtime. 

---

## Personal Context

The `personal` object provides information associated with the authenticated profile.

Current normalized fields include:

- availability;
- title;
- primary value;
- secondary value.

The receiver does not determine which personal content is relevant.

It only copies accepted values into the internal snapshot. 

---

# String Handling

All JSON strings are copied into fixed-size buffers defined by the public data model.

The helper routine:

```text
copy_json_string()
```

performs the following operations:

- validates the destination pointer;
- clears the destination;
- verifies that the JSON field is a string;
- copies the value using bounded formatting.

This approach prevents direct exposure of dynamically allocated JSON strings to the rest of the runtime. :contentReference[oaicite:8]{index=8}

---

# Fallback Handling

Optional Boolean and numeric values are read through dedicated helper functions.

```text
json_bool_or()
json_number_or()
```

These helpers return predefined fallback values when:

- a field is missing;
- the JSON type is incorrect;
- the value cannot be interpreted as the expected type.

This behavior allows the parser to handle incomplete optional metadata without exposing partially initialized values. :contentReference[oaicite:9]{index=9}

---

# Thread Safety

The receiver task and the main Ambient Runtime loop access shared state concurrently.

To protect this state, the component uses:

```text
portMUX_TYPE
```

with critical sections around all shared snapshot updates and reads.

Protected state includes:

- receiver operational status;
- most recent Semantic Event metadata;
- current Ambient Context snapshot;
- initialization state;
- listening state. :contentReference[oaicite:10]{index=10}

---

# Thread-Safe Snapshot Model

The component does not return direct pointers to its internal state.

Instead, every public getter returns a value copy.

```text
Internal Shared State
        │
        ▼
Critical Section
        │
        ▼
Copy Snapshot
        │
        ▼
Release Lock
        │
        ▼
Return Value
```

This model prevents callers from modifying the internal receiver state and minimizes the time spent inside critical sections. 

---

# Receiver Status Synchronization

The following functions modify operational state inside protected critical sections:

- listening-state updates;
- accepted event registration;
- initialization-state updates.

The public receiver-status getter also copies the complete status structure while holding the same lock. :contentReference[oaicite:12]{index=12}

---

# Ambient Context Synchronization

When a new Ambient Context is accepted, the component:

1. enters the critical section;
2. calculates the next reception count;
3. replaces the complete previous snapshot;
4. restores the incremented count;
5. exits the critical section.

This full-structure replacement ensures that consumers never observe a partially updated context. :contentReference[oaicite:13]{index=13}

---

# TTL Management

The component evaluates whether the current Ambient Context is still valid using:

- `received_at_ms`;
- `ttl_seconds`;
- monotonic local time.

The reception timestamp is captured when the context is parsed.

```text
Context Received
        │
        ▼
Store Monotonic Timestamp
        │
        ▼
Later Snapshot Request
        │
        ▼
Calculate Elapsed Time
        │
        ▼
Compare with TTL
        │
   ┌────┴────┐
   │         │
Within TTL  Expired
   │         │
   ▼         ▼
Fresh      Stale
```

The calculated `stale` value is applied to the returned copy rather than modifying the stored context. 

---

# Monotonic Time

The implementation uses:

```text
esp_timer_get_time()
```

to calculate elapsed time.

This provides monotonic local timing independent from:

- wall-clock time;
- timezone;
- NTP availability;
- date configuration.

This is appropriate for context-expiration evaluation in an embedded runtime. :contentReference[oaicite:15]{index=15}

---

# Receiver Task

The UDP receiver operates in a dedicated FreeRTOS task named:

```text
semantic_receiver
```

The current task configuration uses:

```text
Stack size: 6144 bytes
Priority:   5
```

The task is created during `semantic_event_receiver_init()`. :contentReference[oaicite:16]{index=16}

---

# Socket Lifecycle

The receiver task manages the complete UDP socket lifecycle.

```text
Create Socket
        │
        ▼
Bind UDP Port
        │
        ▼
Set Listening State
        │
        ▼
Receive Messages
        │
        ▼
Dispatch JSON
        │
        ▼
Socket Error
        │
        ▼
Close Socket
        │
        ▼
Clear Listening State
        │
        ▼
Restart Receiver
```

The socket listens on all local interfaces through:

```text
INADDR_ANY
```

using UDP port:

```text
5555
```

The port is defined by:

```c
#define SEMANTIC_EVENT_RECEIVER_PORT 5555
``` 

---

# Receive Buffer

The current UDP receive buffer supports payloads up to:

```text
1023 bytes of message content
```

with one additional byte reserved for string termination.

The buffer size is defined internally as:

```c
#define UDP_RECEIVE_BUFFER_LENGTH 1024
```

Received payloads are terminated with `'\0'` before being passed to the JSON parser. :contentReference[oaicite:18]{index=18}

---

# Automatic Recovery

The receiver task includes automatic recovery for socket and bind failures.

Current recovery behavior includes:

- waiting two seconds after socket-creation failure;
- waiting two seconds after bind failure;
- restarting one second after a receive-loop failure.

This recovery mechanism keeps the receiver operational without requiring the main application to recreate the component. :contentReference[oaicite:19]{index=19}

---

# Engineering Principles

## Communication Encapsulation

UDP sockets, LwIP structures and JSON parsing remain private to the component.

Higher-level runtime modules never process transport-specific objects.

---

## Operational and Contextual Separation

Receiver monitoring data is stored separately from Ambient Context data.

This prevents communication health information from becoming coupled to presentation content. :contentReference[oaicite:20]{index=20}

---

## Normalized Data Contracts

The component exposes only fixed-size, typed C structures.

No `cJSON` objects, socket handles or dynamically allocated strings cross the public API boundary.

---

## Passive Embedded Runtime

The Ambient Runtime receives semantic information selected by the Cognitive Runtime.

It does not choose:

- external APIs;
- profiles;
- content relevance;
- personalization policies;
- business rules.

The embedded runtime consumes normalized values and presents them through dedicated components. :contentReference[oaicite:21]{index=21}

---

## Graceful Message Rejection

Malformed, incomplete and unsupported messages are rejected without replacing valid existing snapshots.

This preserves runtime continuity when invalid network traffic is received.

---

## Recoverable Receiver Operation

Socket failures do not permanently terminate semantic communication.

The receiver task recreates its socket and resumes listening after a controlled delay.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability                   | Status    |
| ---------------------------- | --------- |
| UDP socket creation          | Validated |
| UDP bind on port `5555`      | Validated |
| Background receiver task     | Validated |
| JSON parsing                 | Validated |
| Message type dispatch        | Validated |
| Semantic Event reception     | Validated |
| Ambient Context reception    | Validated |
| Context normalization        | Validated |
| Thread-safe receiver status  | Validated |
| Thread-safe context snapshot | Validated |
| Context TTL evaluation       | Validated |
| Automatic receiver restart   | Validated |
| Ambient Runtime integration  | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Validation Indicators

The receiver exposes operational information through:

```text
semantic_event_receiver_status_t
```

Relevant validation fields include:

* `initialized`;
* `listening`;
* `event_received`;
* `received_count`;
* `last_event_type`;
* `last_target`.

These fields allow the Ambient Runtime Console and serial logs to confirm that semantic communication is operational without exposing sockets or parser internals.

---

# Expected Runtime Behavior

After successful initialization, the receiver should:

1. create the `semantic_receiver` FreeRTOS task;
2. create a UDP socket;
3. bind to port `5555`;
4. enter the listening state;
5. wait for incoming runtime messages;
6. parse supported JSON payloads;
7. update the corresponding thread-safe snapshot;
8. continue listening for subsequent messages.

Representative runtime logs include:

```text
Semantic event receiver initialized
Semantic receiver task started
Waiting for runtime messages on UDP port 5555
```

After receiving a Semantic Event:

```text
Semantic event consumed | Type=semantic_event Event=<event> Target=<target>
```

After receiving an Ambient Context:

```text
Ambient context consumed | Seq=<sequence> Profile=<profile> Global=<state> Personal=<state>
```

These logs provide engineering visibility into the communication path while the public API exposes normalized snapshots to other components.

---

# Limitations

The component intentionally implements only the semantic reception boundary required by the current Ambient Runtime.

It does not implement:

* message transmission;
* delivery acknowledgment;
* message retransmission;
* message ordering enforcement;
* duplicate detection;
* sender authentication;
* payload encryption;
* transport-level integrity verification;
* persistent event history;
* persistent context storage;
* Semantic Event execution beyond the currently recognized baseline;
* dynamic port configuration;
* multiple simultaneous receiver sockets.

These capabilities are outside the current validated scope.

---

# UDP Transport Characteristics

The current implementation uses UDP because it provides a lightweight communication mechanism suitable for the controlled local network used by the Ambient Physical AI integration.

UDP does not guarantee:

* delivery;
* ordering;
* uniqueness;
* connection state.

The receiver therefore treats every valid payload as an independent message.

Any stronger delivery semantics must be implemented by higher-level services or by a future transport layer.

---

# Message Size Limitation

The current receive buffer is:

```text
1024 bytes
```

One byte is reserved for null termination before JSON parsing.

Therefore, the practical maximum accepted payload is:

```text
1023 bytes
```

Messages larger than the receive buffer are not supported by the current implementation.

---

# Semantic Event Limitations

The component currently registers all valid Semantic Events in the operational status snapshot.

However, runtime-specific processing remains limited.

The current implementation:

* ignores events addressed to other targets;
* recognizes `ambient_runtime` as the local target;
* acknowledges `identity_authenticated`;
* logs other Ambient Runtime events as not implemented.

This behavior must not be interpreted as complete Semantic Event execution support.

---

# Ambient Context Limitations

The current Ambient Context contract supports:

* one profile identifier;
* one global context block;
* one personal context block;
* one sequence value;
* one TTL value.

The component stores only the most recently accepted context.

It does not maintain:

* previous contexts;
* context history;
* multiple simultaneous profiles;
* multiple personal content items;
* context prioritization;
* semantic merging.

Those responsibilities remain outside the embedded receiver.

---

# Error Handling

The receiver follows a fail-safe rejection strategy.

## Invalid JSON

Invalid JSON payloads are ignored.

The currently stored snapshots remain unchanged.

---

## Missing Message Type

Messages without a valid string field named `type` are rejected.

---

## Incomplete Semantic Events

Semantic Events without both `event_type` and `target` are ignored.

---

## Incomplete Ambient Context

Ambient Context messages are rejected when either:

```text
global
```

or:

```text
personal
```

is missing or is not a JSON object.

---

## Unsupported Message Types

Unsupported message types are logged and ignored.

The receiver remains operational and continues listening.

---

## Socket Failure

Socket creation, binding and reception failures cause the task to close the current socket and retry after a controlled delay.

---

# Security Considerations

The current implementation is designed for a controlled laboratory network.

It does not authenticate the UDP sender or verify cryptographic signatures.

The `authenticated` field inside the Ambient Context represents semantic information supplied by the Cognitive Runtime.

It is not transport-level authentication performed by the receiver.

Therefore, the current component should not be exposed directly to untrusted networks without an additional security layer.

---

# Design Decisions

## Single Communication Boundary

All Cognitive Runtime messages intended for the Ambient Runtime enter through one dedicated component.

This prevents UDP and JSON handling from spreading throughout the application.

---

## Separate Operational and Context Snapshots

The component maintains two independent public data models:

```text
semantic_event_receiver_status_t
```

for communication monitoring, and:

```text
ambient_context_snapshot_t
```

for contextual presentation.

This separation keeps diagnostic state independent from user-facing context.

---

## Value-Based Public API

Public getters return complete structure copies rather than internal pointers.

This protects internal state and simplifies ownership rules.

Callers do not need to:

* allocate memory;
* release memory;
* retain locks;
* manage parser objects.

---

## Fixed-Size Data Structures

The component uses fixed-size character buffers for all normalized strings.

This avoids dynamic allocation in the public data model and provides predictable memory use.

The trade-off is that incoming strings longer than the configured buffers are truncated.

---

## Monotonic TTL Evaluation

Context age is calculated from monotonic local time rather than wall-clock time.

This ensures that TTL evaluation remains operational even when:

* NTP is unavailable;
* the system date is unset;
* timezone configuration changes;
* wall-clock corrections occur.

---

## Complete Snapshot Replacement

When a valid Ambient Context is accepted, the previous context is replaced atomically.

This prevents consumers from observing a mixture of fields from different messages.

---

# Future Evolution

Potential future evolution may include:

* explicit message schema versioning;
* sequence-order validation;
* duplicate-message rejection;
* sender validation;
* authenticated transport;
* encrypted communication;
* configurable UDP port;
* extended Semantic Event processing;
* transport abstraction;
* message reception statistics;
* malformed-payload counters;
* optional persistent diagnostics.

These items are future work and are not part of the current validated implementation.

---

# Related Components

The Semantic Event Receiver operates together with the following Ambient Runtime components.

| Component                | Relationship                                                                                |
| ------------------------ | ------------------------------------------------------------------------------------------- |
| `ambient_network`        | Provides the network connectivity required for UDP reception.                               |
| `oled_context_presenter` | Consumes the normalized Ambient Context snapshot.                                           |
| `ambient_console`        | Displays receiver connectivity, latest event and latest target.                             |
| `oled_sh1107`            | Displays context after presentation formatting, without depending directly on the receiver. |
| `main`                   | Initializes the receiver and retrieves its snapshots during the runtime loop.               |

---

# Related Documentation

| Document                                                                          | Description                                    |
| --------------------------------------------------------------------------------- | ---------------------------------------------- |
| `firmware/nodes/ambient-runtime-node/README.md`                                   | Ambient Runtime architectural overview.        |
| `firmware/nodes/ambient-runtime-node/components/README.md`                        | Runtime component architecture.                |
| `firmware/nodes/ambient-runtime-node/main/README.md`                              | Application lifecycle and runtime integration. |
| `firmware/nodes/ambient-runtime-node/components/ambient_network/README.md`        | Network infrastructure.                        |
| `firmware/nodes/ambient-runtime-node/components/oled_context_presenter/README.md` | Mini OLED presentation layer.                  |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md`        | Runtime operational console.                   |

---

# Build

The component is built automatically as part of the Ambient Runtime.

Typical build and validation sequence:

```bash
cd firmware/nodes/ambient-runtime-node

idf.py build

idf.py flash monitor
```

After startup, verify that the receiver reports:

```text
Semantic event receiver initialized
Semantic receiver task started
Waiting for runtime messages on UDP port 5555
```

A valid Ambient Context message should produce an `Ambient context consumed` log and update the Mini OLED presentation.

---

# Conclusion

The `semantic_event_receiver` component provides the semantic communication boundary between the Cognitive Runtime and the Ambient Runtime.

It encapsulates UDP reception, JSON validation, message dispatch, context normalization, thread-safe state management and TTL evaluation behind a compact public API.

By exposing only normalized value-based snapshots, the component prevents transport details, parser objects and Cognitive Runtime contracts from leaking into presentation and application modules.

This architecture establishes a stable embedded gateway through which the Ambient Runtime receives semantic information while preserving a clear separation between:

```text
Communication
        │
        ▼
Normalization
        │
        ▼
Presentation
```

The current implementation represents the validated semantic reception baseline for the Ambient Runtime.
