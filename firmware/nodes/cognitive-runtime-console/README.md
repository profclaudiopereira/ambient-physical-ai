# Cognitive Runtime Console

## Ambient Physical AI

A dedicated touch-enabled graphical console for observing the operational state of the AX630C Cognitive Runtime.

---

## Overview

The Cognitive Runtime Console is an independent firmware node built for the M5Stack CoreS3 Lite.

It provides a multi-screen operator interface for:

- Cognitive Runtime services;
- active identity and context;
- cognitive state;
- AX630C health telemetry;
- runtime network information;
- recent semantic and integration events.

The console is an observability component. It does not execute inference, generate semantic events, dispatch actions, or replace the Ambient Runtime.

---

## Project Status

```text
GRAPHICAL UI                    IMPLEMENTED
TOUCH NAVIGATION                IMPLEMENTED
FIVE PERSISTENT VIEWS           IMPLEMENTED
RUNTIME STATUS MODEL            IMPLEMENTED
JSON CONTRACT                   IMPLEMENTED
PARTIAL JSON UPDATES            IMPLEMENTED
UART RECEIVER                   IMPLEMENTED
THREAD-SAFE LVGL UPDATES        IMPLEMENTED
DEVELOPMENT DATA PATH           VALIDATED
LIVE AX630C STATUS PUBLISHER     IMPLEMENTED
PHYSICAL UART INTEGRATION       VALIDATED
```

Current milestone:

```text
COGNITIVE_RUNTIME_CONSOLE_V1
Firmware architecture and graphical interface implemented
```

---

## Hardware

```text
M5Stack CoreS3 Lite
```

The firmware uses the official M5Stack CoreS3 BSP for:

- display initialization;
- touch controller initialization;
- LVGL startup;
- display locking;
- backlight control.

Dependency:

```yaml
espressif/m5stack_core_s3: ^3.0.0
```

---

## Technology Stack

```text
ESP-IDF
FreeRTOS
LVGL
M5Stack CoreS3 BSP
cJSON
ESP-IDF UART Driver
```

---

## Architecture

```text
AX630C Cognitive Runtime
        ↓
cognitive_runtime_console_notifier.py
        ↓
Newline-delimited JSON over UART
        ↓
runtime_uart
        ↓
runtime_status
        ↓
ui_manager
        ↓
Runtime | State | Health | Network | Log
```

Firmware organization:

```text
main/
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
    ├── screen_runtime.h
    ├── screen_state.c
    ├── screen_state.h
    ├── screen_health.c
    ├── screen_health.h
    ├── screen_network.c
    ├── screen_network.h
    ├── screen_log.c
    └── screen_log.h
```

---

## Design Principles

The implementation separates four concerns:

```text
Platform Initialization
Transport
Runtime Data Model
Presentation
```

Key constraints:

- screens do not communicate with the AX630C;
- the UART layer does not manipulate LVGL widgets;
- the runtime model does not depend on hardware or UI libraries;
- navigation is owned only by the UI Manager;
- all views consume the same immutable runtime snapshot;
- all LVGL updates occur under the BSP display lock;
- fixed-size buffers provide deterministic memory behavior.

---

## Graphical Interface

The interface uses a persistent shell:

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

The header and navigation bar remain present while the active view changes.

All views are created once during startup and are hidden or revealed without rebuilding their LVGL object trees.

---

## Views

### Runtime

Displays:

```text
Linux
StackFlow
MCP
Semantic
User
Context
Last RX
```

### State

Displays:

```text
Current Cognitive State
User
Context
Last Event
```

### Health

Displays:

```text
CPU
RAM
Temperature
Uptime
```

### Network

Displays:

```text
Network
IP
RSSI
Runtime Link
Heartbeat
```

### Log

Displays a bounded list of the five most recent runtime events.

---

## Touch Navigation

The bottom navigation bar contains five buttons:

```text
Run
State
Health
Net
Log
```

A touch event selects the corresponding view.

The active button receives a distinct visual background.

Navigation does not destroy or recreate widgets.

---

## Runtime Status Model

The shared model is defined by:

```text
main/runtime/runtime_status.h
```

It contains:

```text
Service availability
Identity and context
Cognitive state
Last semantic event
Last reception indicator
CPU utilization
RAM usage
Temperature
Uptime
Network state
IP address
RSSI
Runtime link
Heartbeat
Recent event list
```

The model is presentation-neutral and transport-independent.

A future UDP, USB, or other transport can reuse it without changing the screen modules.

---

## JSON Contract

The console accepts documents with:

```json
{
  "type": "runtime_status"
}
```

Complete structure:

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

### Partial Updates

Fields omitted from a valid document retain their previous values.

This allows both:

```text
Periodic full snapshots
```

and:

```text
Small event-driven updates
```

---

## UART Transport

Configuration:

```text
UART: UART1
Baud rate: 115200
Data bits: 8
Parity: none
Stop bits: 1
Flow control: disabled
Framing: one JSON document per line
```

The receiver accepts:

```text
LF
```

or:

```text
CRLF
```

Maximum JSON buffer:

```text
2048 bytes
```

Oversized documents are discarded until the next newline so truncated JSON is never applied.

Pin configuration is centralized in:

```text
main/transport/runtime_uart_config.h
```

The final physical wiring and RX/TX orientation must be validated before closing the hardware integration milestone.

---

## Runtime Update Flow

```text
UART byte stream
      ↓
Line reconstruction
      ↓
JSON type validation
      ↓
Partial model update
      ↓
UI Manager publication
      ↓
BSP display lock
      ↓
Update all five views
      ↓
BSP display unlock
```

The UART task never accesses LVGL widgets directly.

---

## Local Fallback State

Before the first external packet, the console uses deterministic fallback values:

```text
User: Unknown
Context: Waiting
State: IDLE
Last Event: console_initialized
Last RX: LOCAL FALLBACK
Network: disconnected
Runtime Link: WAITING
IP: 0.0.0.0
```

The current development build also applies an internal test JSON packet to validate the complete parser-to-display path.

This development packet must be removed or disabled after live AX630C telemetry is integrated.

---

## Build

From the node directory:

```bat
cd firmware\nodes\cognitive-runtime-console
```

Configure the ESP-IDF environment, then run:

```bat
idf.py build
```

Flash:

```bat
idf.py -p COM_PORT flash
```

Monitor:

```bat
idf.py -p COM_PORT monitor
```

Combined:

```bat
idf.py -p COM_PORT flash monitor
```

Replace `COM_PORT` with the serial port assigned to the CoreS3 Lite.

---

## Expected Startup Log

Representative output:

```text
Starting Cognitive Runtime Console
Initializing complete console navigation
Runtime JSON contract validated
Five console views ready
Cognitive Runtime UART transport ready
Cognitive Runtime Console initialized
```

The UART task also reports its configured port, GPIO, and baud rate.

---

## AX630C Integration

The remaining Linux-side component should follow the StackFlow convention of specialized Python modules.

Implemented AX630C publisher:

```text
cognitive_runtime_console_notifier.py
```

Validated implementation:

- publishes complete `runtime_status` snapshots over `/dev/ttyS1`;
- configures the UART automatically for 115200 8N1 without flow control;
- uses newline-delimited UTF-8 JSON;
- uses only the Python standard library (no PySerial dependency);
- is integrated into `identity_udp_listener.py`;
- publishes identity, context, runtime state, health telemetry, network status and recent events.

The Runtime Console remains a pure observability component. Failures in the console publisher never interrupt the Cognitive Runtime semantic pipeline.

---

## Validation Checklist

### Firmware

- [x] BSP display initialization
- [x] Backlight enabled
- [x] LVGL interface created
- [x] Touch navigation operational
- [x] Five views available
- [x] Persistent screen lifecycle
- [x] Runtime status model
- [x] JSON type validation
- [x] Partial update behavior
- [x] UART driver initialization
- [x] FreeRTOS receiver task
- [x] LF and CRLF handling
- [x] Oversized document rejection
- [x] BSP display locking
- [x] Full development JSON path

### Final Integration

- [x] Validate physical UART wiring
- [x] Confirm final RX/TX GPIO orientation
- [x] Implement AX630C Python publisher
- [ ] Install publisher as a Linux service
- [x] Validate continuous live telemetry
- [ ] Validate heartbeat loss indication
- [ ] Remove or disable development JSON
- [ ] Capture final hardware evidence
- [ ] Record final integration logs

---

## Engineering Update (August 2026)

The AX630C integration has been completed and validated using the dedicated
`cognitive_runtime_console_notifier.py` publisher.

Validated configuration:

```text
Transport : UART
Linux Port: /dev/ttyS1
Console RX: GPIO18
Baud Rate : 115200
Format    : UTF-8 JSON + LF
```

The publisher is integrated with `identity_udp_listener.py` and automatically
publishes Cognitive Runtime state transitions, active identity, active context,
Linux health telemetry, network information and recent runtime events to the
CoreS3 Lite Runtime Console.

---

## Known Engineering Notes

### Development JSON

`ui_manager.c` currently contains a development runtime packet.

It is useful for validating the complete UI path but is not the final data source.

### UART Pin Comment Review

Before final integration, review the GPIO comments and configured RX/TX symbols in:

```text
main/transport/runtime_uart_config.h
```

The physical sender TX must connect to console RX, and both systems must share ground.

### Health Summary

The current Health summary displays `NOMINAL` as a static presentation label.

A future refinement may derive the summary from CPU, RAM, temperature, heartbeat, and service state thresholds.

---

## Documentation

Detailed architectural decisions are documented in:

```text
docs/architecture/COGNITIVE_RUNTIME_CONSOLE_TECHNICAL_ARCHITECTURE.md
```

---

## Current Status Statement

> The Cognitive Runtime Console firmware now includes a validated end-to-end integration with the AX630C Cognitive Runtime through the dedicated `cognitive_runtime_console_notifier.py` publisher. Live Runtime telemetry is transported over `/dev/ttyS1` using newline-delimited JSON, updating all five console views in real time while remaining architecturally independent from the semantic execution pipeline.
