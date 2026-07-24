# Cognitive Runtime Console — Technical Architecture Note

## Ambient Physical AI

**Document type:** Technical Architecture Note  
**Component:** Cognitive Runtime Console  
**Target hardware:** M5Stack CoreS3 Lite  
**Runtime counterpart:** AX630C Cognitive Runtime  
**Implementation language:** C / ESP-IDF / LVGL  
**Status:** Firmware architecture implemented; live AX630C telemetry publisher pending

---

## 1. Purpose

The Cognitive Runtime Console is a dedicated operator interface for observing the internal state of the Ambient Physical AI Cognitive Runtime.

Its purpose is not to execute cognitive inference, orchestrate semantic actions, or replace the Ambient Runtime. Instead, it provides a compact, touch-enabled graphical console that exposes selected operational, semantic, health, network, and event information from the AX630C-based Cognitive Runtime.

The console is the first node in the project to adopt a persistent multi-screen graphical architecture with touch navigation, a presentation-neutral runtime model, a transport-independent JSON contract, and synchronized LVGL updates.

---

## 2. Architectural Role

```text
Ambient Physical AI

Presence Node
      ↓
Identity Node
      ↓
AX630C Cognitive Runtime
      ├── Context Builder
      ├── Semantic Event Generator
      ├── Semantic Dispatcher
      ├── Destination Notifiers
      └── Runtime Status Publisher
                    ↓ UART / JSON
        Cognitive Runtime Console
```

The console is an observability node.

It receives status information from the Cognitive Runtime and renders it for a human operator. It does not participate in the semantic decision path and does not control the Ambient Runtime or Expression Layer.

---

## 3. Design Goals

The implementation follows the principles below:

- isolate hardware startup from application presentation;
- keep communication code independent from screen code;
- define a stable runtime status contract;
- support partial runtime updates;
- use deterministic fixed-size memory;
- create LVGL widget trees only once;
- switch views without reallocating screens;
- update graphical elements under the BSP display lock;
- allow future transports without redesigning the UI;
- preserve a safe local fallback state before the first external packet.

---

## 4. Software Architecture

```text
firmware/nodes/cognitive-runtime-console/
└── main/
    ├── cognitive_runtime_console.c
    ├── runtime/
    │   ├── runtime_status.c
    │   └── runtime_status.h
    ├── transport/
    │   ├── runtime_uart.c
    │   ├── runtime_uart.h
    │   └── runtime_uart_config.h
    └── ui/
        ├── ui_manager.c
        ├── ui_manager.h
        ├── screen_runtime.c
        ├── screen_state.c
        ├── screen_health.c
        ├── screen_network.c
        └── screen_log.c
```

The architecture is divided into four responsibilities:

```text
Application Initialization
        ↓
Transport
        ↓
Runtime Model
        ↓
UI Manager
        ↓
Presentation Screens
```

### 4.1 Application Initialization

`cognitive_runtime_console.c` owns only platform-level startup.

Responsibilities:

- start the official M5Stack CoreS3 BSP display stack;
- enable the display backlight;
- initialize the complete UI;
- start the UART transport task;
- report startup failures.

The entry point does not create individual screens and does not parse runtime data.

### 4.2 Runtime Model

`runtime_status.c` and `runtime_status.h` define the stable data contract shared by transports and presentation modules.

The model contains:

- Linux, StackFlow, MCP and Semantic service availability;
- active user and environmental context;
- cognitive state, last event and last reception indicator;
- CPU, RAM, temperature and uptime;
- network state, IP, RSSI, runtime link and heartbeat;
- a bounded recent-event list.

The model has no dependency on LVGL, BSP drivers, UART, Wi-Fi, or AX630C-specific APIs.

### 4.3 Transport Layer

`runtime_uart.c` owns the physical UART reception path.

Responsibilities:

- configure UART1;
- receive byte streams;
- reconstruct newline-delimited JSON documents;
- accept LF and CRLF senders;
- reject oversized documents safely;
- invoke the runtime parser;
- publish valid snapshots through the UI Manager.

The transport never manipulates LVGL widgets directly.

### 4.4 UI Manager

`ui_manager.c` owns:

- the persistent graphical shell;
- the header status indicator;
- the bottom navigation bar;
- view creation and lifecycle;
- touch navigation;
- synchronized model presentation;
- propagation of updates to all screens.

All screens are created once. Navigation changes only their hidden or visible state.

### 4.5 Screen Modules

Each screen owns only its internal widgets and presentation logic.

The screen modules do not:

- initialize hardware;
- receive UART data;
- parse JSON;
- control navigation;
- access Linux services;
- communicate with the AX630C.

---

## 5. Runtime Data Contract

The console accepts JSON documents whose top-level type is:

```json
{
  "type": "runtime_status"
}
```

Expected sections:

```json
{
  "type": "runtime_status",
  "services": {},
  "identity": {},
  "cognitive": {},
  "health": {},
  "network": {},
  "events": []
}
```

A representative complete document is:

```json
{
  "type": "runtime_status",
  "services": {
    "linux": true,
    "stackflow": true,
    "mcp": true,
    "semantic": true
  },
  "identity": {
    "user": "Claudio",
    "context": "Laboratory"
  },
  "cognitive": {
    "state": "READY",
    "last_event": "welcome_researcher",
    "last_rx": "UART"
  },
  "health": {
    "cpu_percent": 31,
    "ram_used_mb": 418,
    "ram_total_mb": 1024,
    "temperature_c": 50,
    "uptime": "02h 26m"
  },
  "network": {
    "connected": true,
    "heartbeat": true,
    "rssi_dbm": -47,
    "ip": "192.168.77.15",
    "runtime_link": "AX630C UART"
  },
  "events": [
    "Presence detected",
    "Identity authenticated",
    "Context built",
    "Semantic event generated",
    "Welcome dispatched"
  ]
}
```

### 5.1 Partial Updates

The parser supports partial documents.

When a valid field is absent, the previous value remains unchanged. This allows the AX630C to send:

- complete periodic snapshots;
- smaller event-driven updates;
- health-only updates;
- semantic-state updates;
- heartbeat updates.

### 5.2 Validation

The parser:

- rejects unrelated top-level document types;
- checks string, Boolean, numeric, object, and array types;
- bounds integer values before conversion;
- uses fixed-size buffers;
- truncates text safely through bounded copying;
- limits the event history to five entries.

This protects the console from malformed or unrelated packets and preserves deterministic memory behavior.

---

## 6. UART Transport

Current configuration:

```text
Peripheral: UART1
Baud rate: 115200 bit/s
Data format: 8N1
Flow control: disabled
Framing: one JSON document per line
Maximum JSON document: 2047 characters plus terminator
Driver RX buffer: 2048 bytes
```

The receiver accepts both:

```text
JSON + LF
```

and:

```text
JSON + CRLF
```

If a document exceeds the configured maximum size, the receiver discards the entire line and resumes at the next newline. A truncated JSON object is never forwarded to the parser.

### 6.1 Current Pin Mapping

The firmware centralizes the pin mapping in:

```text
main/transport/runtime_uart_config.h
```

The implementation must be revalidated against the final physical M-BUS wiring before the hardware integration is declared complete.

---

## 7. Graphical Architecture

The UI uses a persistent shell consisting of:

```text
┌────────────────────────────────────┐
│ Cognitive Runtime       ● READY    │
├────────────────────────────────────┤
│                                    │
│          Active View Area          │
│                                    │
├────────────────────────────────────┤
│ Run | State | Health | Net | Log   │
└────────────────────────────────────┘
```

The header remains visible across all views.

The bottom navigation bar provides direct touch access to five pages.

The active navigation button receives a distinct visual state.

The interface uses a dark, high-contrast visual language with semantic status colors for ready, degraded, offline, and active conditions.

---

## 8. Console Views

## 8.1 Runtime View

The Runtime view is the operational dashboard.

It displays:

- Linux;
- StackFlow;
- MCP;
- Semantic;
- active user;
- active context;
- last runtime reception.

The service fields use semantic colors to distinguish operational and failed states.

## 8.2 State View

The State view presents the current cognitive activity.

It displays:

- current cognitive state;
- active user;
- active context;
- most recent semantic event.

Long event names use bounded display behavior to remain inside the card.

## 8.3 Health View

The Health view displays AX630C platform telemetry:

- CPU utilization;
- RAM usage and total memory;
- temperature;
- uptime.

The current summary card presents the runtime as nominal while detailed values are updated independently.

## 8.4 Network View

The Network view displays:

- network availability;
- IP address;
- RSSI;
- runtime link description;
- heartbeat state.

It is intended to distinguish local console operation from a healthy connection to the Cognitive Runtime.

## 8.5 Log View

The Log view presents a bounded history of the five most recent runtime events.

The list is optimized for demonstration and integration diagnosis rather than long-term logging.

---

## 9. Screen Lifecycle

All five screens are created during `ui_manager_init()`.

They remain allocated for the lifetime of the application.

Navigation uses LVGL visibility flags:

```text
Requested view
      ↓
Reveal selected container
      ↓
Hide remaining containers
      ↓
Update active navigation style
```

Advantages:

- no repeated widget allocation;
- no screen destruction;
- immediate transitions;
- stable widget references;
- predictable memory usage;
- reduced fragmentation risk.

---

## 10. Thread Safety and LVGL Ownership

The UART receiver runs in a FreeRTOS task outside the LVGL execution context.

To make this safe, transport code does not update widgets directly.

Instead:

```text
UART Task
   ↓
runtime_status_apply_json()
   ↓
ui_manager_update_runtime_status()
   ↓
BSP display lock
   ↓
Update all screens
   ↓
BSP display unlock
```

The UI Manager copies the received snapshot into its own internal model before updating the views. The caller may therefore reuse its local model immediately after the function returns.

---

## 11. Local Fallback and Development Packet

Before external runtime telemetry is received, the model is initialized with deterministic fallback values such as:

```text
User: Unknown
Context: Waiting
State: IDLE
Last Event: console_initialized
Runtime Link: WAITING
IP: 0.0.0.0
```

The current firmware also contains an embedded development JSON packet used to validate:

```text
JSON document
    ↓
parser
    ↓
runtime model
    ↓
UI Manager
    ↓
all five screens
```

This development packet must be removed or disabled once the live AX630C publisher is integrated, so the production interface begins from the true local fallback state.

---

## 12. AX630C-Side Component

The missing Linux-side component should follow the existing StackFlow convention of specialized Python modules.

Recommended working name:

```text
cognitive_runtime_console_notifier.py
```

A second acceptable name is:

```text
runtime_console_notifier.py
```

Its responsibility will be different from semantic destination notifiers.

It will publish observability snapshots rather than semantic actions.

Expected responsibilities:

- inspect Linux and Cognitive Runtime service state;
- read active identity and context;
- expose current cognitive state and last semantic event;
- collect CPU, RAM, temperature and uptime;
- report network and heartbeat information;
- maintain a bounded recent-event list;
- serialize the data as `runtime_status`;
- transmit newline-delimited JSON over the AX630C UART;
- operate periodically and optionally react to significant runtime changes.

Proposed flow:

```text
AX630C Runtime Sources
        ↓
Cognitive Runtime Console Notifier
        ↓
runtime_status JSON
        ↓
UART
        ↓
CoreS3 Lite runtime_uart
        ↓
runtime_status parser
        ↓
UI Manager
        ↓
Five touch views
```

---

## 13. Current Validation Status

Implemented:

- CoreS3 BSP display initialization;
- touch-enabled LVGL shell;
- five persistent views;
- direct touch navigation;
- runtime status data model;
- JSON parsing;
- partial updates;
- input validation;
- fixed-size deterministic buffers;
- UART task and line framing;
- synchronized UI updates;
- development end-to-end JSON path;
- modular and documented source organization.

Pending final integration:

- implement the AX630C Python status publisher;
- connect and validate physical UART wiring;
- confirm final RX/TX pin orientation;
- remove or disable the embedded development packet;
- validate continuous live updates;
- define heartbeat timeout behavior;
- document the final Linux service deployment;
- capture final photographic evidence and validation logs.

---

## 14. Engineering Significance

This implementation establishes a reusable graphical-node pattern for the Ambient Physical AI repository.

The pattern can be summarized as:

```text
Transport-independent model
        +
Persistent multi-view UI
        +
Central UI ownership
        +
Thread-safe update gateway
        +
Explicit external data contract
```

Future graphical nodes can reuse these principles without copying the exact console implementation.

The Cognitive Runtime Console therefore serves both as an operational node and as an architectural reference for future touch-enabled interfaces in the project.

---

## 15. Repository Placement

This document should be stored in the existing architecture documentation area:

```text
docs/architecture/COGNITIVE_RUNTIME_CONSOLE_TECHNICAL_ARCHITECTURE.md
```

No new directory is required.

The firmware-specific usage and reproduction guide should remain with the source code:

```text
firmware/nodes/cognitive-runtime-console/README.md
```

---

## 16. Status Statement

> The Cognitive Runtime Console firmware has a complete modular graphical architecture, five touch-navigable views, a stable runtime status model, a validated JSON ingestion path, and an operational UART receiver. The remaining milestone is the implementation and validation of the AX630C-side Python publisher that will provide live Cognitive Runtime telemetry.
