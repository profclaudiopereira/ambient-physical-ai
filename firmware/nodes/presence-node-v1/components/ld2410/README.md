# LD2410 Component

## Ambient Physical AI

### Reusable Human Presence Radar Driver

The **LD2410 component** provides the hardware abstraction layer used by the Presence Node to communicate with the HLK-LD2410C 24 GHz FMCW mmWave radar.

It encapsulates all low-level UART communication, protocol decoding and target-state extraction while exposing a simple and reusable API to the application layer.

The component was intentionally designed as an independent ESP-IDF component so it can be reused by future nodes without requiring modifications to application code.

---

# Current Status

```text
LD2410 Component

OFFICIAL BASELINE

VALIDATED
```

Current validated implementation:

```text
HLK-LD2410C
24 GHz FMCW mmWave Radar

ESP-IDF Component

UART Driver

Streaming Parser

Presence Target Decoder
```

---

# Purpose

The component is responsible exclusively for communicating with the LD2410 radar and translating its binary protocol into normalized application data.

Its responsibilities include:

- UART driver initialization;
- radar communication;
- streaming byte reception;
- protocol synchronization;
- frame validation;
- target-state decoding;
- moving-target information;
- stationary-target information;
- detection-distance extraction;
- driver resource management.

The component intentionally does **not** implement:

- presence state machine;
- semantic event generation;
- Wi-Fi communication;
- UDP transmission;
- Identity Layer integration;
- Cognitive Runtime interaction.

Those responsibilities belong to the Presence Node application.

---

# Component Architecture

```text
Presence Node
        │
        ▼
LD2410 Component
        │
        ├──────────────┐
        │              │
        ▼              ▼
UART Driver     Streaming Parser
        │              │
        └──────┬───────┘
               ▼
        Target Decoder
               │
               ▼
Normalized Target Data
```

The component isolates all radar-specific implementation details from the application layer.

The Presence Node never manipulates raw UART frames directly.

Instead, it consumes normalized target information returned by the component API.

---

# Design Principles

The component follows a layered architecture.

```text
Application
        │
Public Driver API
        │
Driver Implementation
        │
Streaming Parser
        │
UART Driver
        │
HLK-LD2410C Radar
```

This separation allows protocol implementation details to evolve without affecting the application.

Likewise, modifications to the parser remain transparent to software using the public API.

---

# Public API

The public interface is defined in:

```text
include/ld2410.h
```

The API intentionally exposes only the operations required by the Presence Node.

Current functions include:

```c
esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle
);

esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks
);

esp_err_t ld2410_delete(
    ld2410_handle_t handle
);

const char *ld2410_target_state_to_string(
    ld2410_target_state_t state
);
```

Opaque handles are used to preserve encapsulation and allow future internal changes without breaking application compatibility.

---

# Driver Lifecycle

The driver follows a simple lifecycle.

```text
Create Driver
        │
ld2410_new()
        │
        ▼
Initialize UART
        │
Initialize Parser
        │
        ▼
Read Target Reports
        │
ld2410_read()
        │
        ▼
Application Consumes Data
        │
        ▼
Release Resources
        │
ld2410_delete()
```

Each driver instance owns its UART configuration and parser context.

Resources are released automatically when the driver is destroyed.

---

# UART Communication Model

The component communicates with the radar using the ESP-IDF UART driver.

Validated baseline:

```text
UART1

256000 baud

8 data bits

No parity

1 stop bit

No hardware flow control
```

Incoming UART data is received incrementally in small chunks.

Rather than assuming frame alignment, every received byte is forwarded to the internal parser, allowing reliable operation even when UART reads split protocol frames across multiple transactions.

---

# Streaming Parser Architecture

Instead of processing complete UART packets, the component implements a streaming parser.

```text
UART Byte Stream
        │
        ▼
Header Synchronization
        │
        ▼
Frame Assembly
        │
        ▼
Frame Validation
        │
        ▼
Target Decoding
        │
        ▼
Normalized Target Data
```

The parser continuously searches for the protocol synchronization header.

Only after a complete frame has been assembled and validated is target information exposed to the application.

Invalid frames are discarded automatically, allowing rapid recovery from communication errors or corrupted data.

---

# Target Data Model

The component exposes decoded radar observations through a normalized data structure defined in the public interface.

```text
ld2410_target_data_t
```

Each successful radar report contains:

- target state;
- moving target distance;
- moving target energy;
- stationary target distance;
- stationary target energy;
- overall detection distance.

This abstraction hides all protocol-specific details from the application layer.

The Presence Node consumes only normalized target information and never accesses raw protocol frames.

---

# Target State Mapping

The driver normalizes the radar protocol into a canonical enumeration.

| Radar State | Description |
|-------------|-------------|
| `LD2410_TARGET_NONE` | No target detected |
| `LD2410_TARGET_MOVING` | Moving human detected |
| `LD2410_TARGET_STATIONARY` | Stationary human detected |
| `LD2410_TARGET_MOVING_AND_STATIONARY` | Simultaneous moving and stationary targets |
| `LD2410_TARGET_UNKNOWN` | Undefined or unsupported protocol value |

This stable abstraction allows higher software layers to remain independent from future protocol revisions.

---

# Parser Validation

Before any target information is returned, every received frame passes a complete validation process.

Validation includes:

- frame header verification;
- payload length verification;
- protocol marker validation;
- frame footer verification;
- complete frame decoding.

Only validated protocol frames produce application data.

Malformed or incomplete frames are silently discarded.

This strategy prevents corrupted UART traffic from propagating invalid information to the Presence Node.

---

# Error Handling

The component follows the standard ESP-IDF error reporting model.

Typical return values include:

```text
ESP_OK

ESP_ERR_INVALID_ARG

ESP_ERR_TIMEOUT

ESP_ERR_NO_MEM

ESP-IDF UART error codes
```

Communication failures are reported immediately to the caller.

Partially initialized resources are automatically released whenever initialization cannot be completed successfully.

This fail-fast behavior keeps the driver in a consistent state.

---

# Integration with the Presence Node

The LD2410 component is consumed by the Presence Node runtime.

Typical execution flow:

```text
Presence Runtime
        │
ld2410_new()
        │
        ▼
Initialize Radar Driver
        │
        ▼
ld2410_read()
        │
        ▼
Receive Target Data
        │
        ▼
Presence State Machine
        │
        ▼
Semantic presence_event
```

The component is intentionally unaware of the semantic meaning assigned to target states.

Its responsibility ends after successfully decoding radar observations.

---

# Repository Structure

```text
components/
└── ld2410/
    ├── CMakeLists.txt
    ├── ld2410.c
    ├── ld2410_parser.c
    ├── ld2410_internal.h
    └── include/
        └── ld2410.h
```

Component organization follows the standard ESP-IDF layout.

The implementation is divided into:

- public API;
- driver implementation;
- parser implementation;
- private definitions.

This separation improves maintainability while preserving a stable public interface.

---

# Current Limitations

Current implementation supports:

- normal data frames;
- continuous UART streaming;
- one radar instance per driver handle;
- normalized target decoding.

Configuration mode, firmware management and advanced radar configuration commands are intentionally outside the scope of this component.

Those capabilities may be added in future versions without changing the current public API.

---

# Future Evolution

Future development may include support for:

- radar configuration commands;
- engineering mode;
- firmware version queries;
- configurable detection parameters;
- multiple radar instances;
- additional protocol messages.

The current architecture already isolates these future capabilities from the application layer, minimizing compatibility impacts.

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `../../README.md` | Presence Layer architecture and runtime overview. |
| `../../main/main.cpp` | Presence Node application runtime. |

---

# README REVIEW SUMMARY

## Purpose

Document the reusable LD2410 radar component independently from the Presence Node application.

## Technical Content

- Public API
- Driver architecture
- UART communication model
- Streaming parser
- Target data model
- Error handling
- Integration boundaries

## Engineering Assessment

The component follows a clean layered architecture separating:

- public interface;
- driver lifecycle;
- streaming parser;
- protocol decoding;
- application integration.

The implementation presents a high level of modularity and encapsulation, making it suitable for reuse by future projects requiring communication with the HLK-LD2410C radar.

---

**Component Status**

```text
LD2410 COMPONENT

OFFICIAL BASELINE

VALIDATED
```