# Atom Matrix RGB Node

> **Ambient Physical AI**  
> **Expression Layer Runtime Node**

The **Atom Matrix RGB Node** is a production firmware component of the Ambient Physical AI project.

It is responsible for receiving normalized Semantic Events produced by the Cognitive Runtime and translating them into deterministic visual expressions using the integrated 5 × 5 WS2812 RGB LED matrix of the M5Stack Atom Matrix.

This node belongs to the **Expression Layer**, whose responsibility is to physically express validated semantic states generated elsewhere in the distributed cognitive ecosystem.

The firmware performs no local reasoning or decision making. It exclusively consumes normalized semantic events and converts them into visual feedback according to the validated Expression Layer architecture.

---

# Mission

The Atom Matrix RGB Node provides a lightweight and deterministic visual expression service for the Ambient Physical AI ecosystem.

Its primary responsibilities are:

- receive Semantic Events over UDP;
- validate incoming semantic messages;
- process normalized semantic states;
- apply authenticated user profiles when available;
- convert semantic meaning into calibrated RGB lighting;
- drive the integrated 5 × 5 RGB LED matrix.

The node intentionally separates semantic processing from hardware control, preserving a modular architecture that simplifies maintenance, testing and future evolution.

---

# Current Status

| Component | Status |
|-----------|--------|
| Firmware | ✅ Validated |
| Wi-Fi Station | ✅ Validated |
| UDP Communication | ✅ Validated |
| Semantic Event Processing | ✅ Validated |
| Authenticated Profile Support | ✅ Validated |
| Platform Calibration | ✅ Validated |
| Integration with Cognitive Runtime | ✅ Ready |

Current implementation represents the validated production baseline for the Atom Matrix RGB Node.

---

# Hardware Platform

| Item | Description |
|------|-------------|
| Board | M5Stack Atom Matrix |
| MCU | ESP32-PICO-D4 |
| LED Matrix | Integrated 5 × 5 WS2812 |
| Total LEDs | 25 |
| LED Interface | RMT |
| LED GPIO | GPIO 27 |
| Framework | ESP-IDF v5.4.x |

The firmware controls the complete RGB matrix as a single ambient expression surface.

Individual pixel animations are intentionally outside the scope of the current implementation.

---

# Position Inside the Expression Layer

The Ambient Physical AI architecture separates cognition from physical expression.

The Cognitive Runtime is responsible for generating normalized semantic meaning.

Expression Nodes are responsible for translating that meaning into physical feedback appropriate for their respective hardware.

For the Atom Matrix RGB Node, this physical feedback consists of calibrated ambient lighting displayed on the integrated RGB matrix.

```text
                Ambient Physical AI

                 Cognitive Runtime
                        │
                        │
             Semantic Event (UDP)
                        │
                        ▼
             Atom Matrix RGB Node
                        │
                        ▼
           Ambient Visual Expression
```

The node does not generate semantic information.

It only consumes validated semantic events already produced by the Cognitive Runtime.

---

# Runtime Architecture

The firmware is organized as a sequence of specialized modules.

Each module owns a single engineering responsibility.

```text
                Wi-Fi Station
                     │
                     ▼
             Semantic Receiver
                     │
                     ▼
             Semantic Consumer
                     │
                     ▼
          Expression Processor
                     │
                     ▼
               RGB Effects
                     │
                     ▼
             RGB Controller
                     │
                     ▼
          5 × 5 WS2812 RGB Matrix
```

This separation minimizes coupling between networking, semantic processing, visual behavior and hardware control.

Each layer communicates only with its immediate neighbor, making the implementation easier to understand, validate and maintain.

---

# Startup Sequence

The firmware follows a deterministic initialization sequence.

```text
Application Startup
        │
        ▼
Initialize RGB Controller
        │
        ▼
Display Boot Expression
        │
        ▼
Initialize Wi-Fi Station
        │
        ▼
Wait for Network Connection
        │
        ▼
Display Idle Expression
        │
        ▼
Start Semantic Receiver
        │
        ▼
Wait for Semantic Events
```

The Semantic Receiver is intentionally started only after network connectivity has been established.

This guarantees that the node begins semantic processing only when it is capable of receiving UDP messages from the Cognitive Runtime.

If a critical initialization failure occurs, the firmware immediately switches to the validated **System Error** visual expression and aborts further startup.

---

# Software Modules

The Atom Matrix RGB Node is organized into independent software modules.

Each module owns a single engineering responsibility and exposes a well-defined interface to the next processing stage.

This organization minimizes coupling between networking, semantic interpretation, visual behavior and hardware control.

| Module | Responsibility |
|----------|----------------|
| `wifi_station` | Initializes the Wi-Fi Station interface and maintains network connectivity. |
| `semantic_receiver` | Receives UDP Semantic Events from the Cognitive Runtime. |
| `semantic_consumer` | Validates Semantic Event messages and extracts semantic information. |
| `expression_processor` | Converts semantic meaning into visual expression requests. |
| `rgb_effects` | Maps semantic states and authenticated profiles to calibrated RGB expressions. |
| `rgb_controller` | Controls the physical WS2812 RGB matrix. |

Each module communicates only through its public interface.

No module bypasses another layer.

---

# Semantic Communication

The Atom Matrix RGB Node receives normalized Semantic Events through UDP.

The current implementation listens on a dedicated UDP socket and forwards every received payload to the Semantic Consumer.

```text
UDP Socket
      │
      ▼
Semantic Receiver
      │
      ▼
Semantic Consumer
```

The node does not generate semantic information.

It only consumes semantic events produced elsewhere in the Ambient Physical AI ecosystem.

---

# Supported Semantic Message

The current implementation accepts normalized Semantic Events with the following general structure.

```json
{
  "type": "semantic_event",
  "event": "...",
  "target": "...",
  "payload": {
    "user_id": "..."
  }
}
```

The payload is intentionally extensible.

Only the authenticated user identifier is currently consumed by the firmware.

Future semantic attributes may be added without changing the event-processing interface.

---

# Target Validation

The Semantic Consumer validates every incoming message before forwarding it to the Expression Processor.

Current implementation accepts only the following targets:

```text
rgb_strip

expression_layer
```

Messages addressed to other targets are ignored.

This preserves compatibility with the validated Cognitive Runtime communication path while allowing generic Expression Layer events.

---

# Semantic Processing Pipeline

Incoming Semantic Events follow a deterministic processing pipeline.

```text
UDP Packet
      │
      ▼
Semantic Receiver
      │
      ▼
JSON Validation
      │
      ▼
Target Validation
      │
      ▼
Optional Payload Extraction
      │
      ▼
Expression Processor
      │
      ▼
RGB Effects
      │
      ▼
RGB Controller
```

Each stage performs one well-defined task before passing control to the next layer.

---

# Semantic Context

The Expression Processor receives both the semantic event name and an optional semantic context.

Current implementation defines:

```text
semantic_context
        │
        └── user_id
```

The context is optional.

Events that do not require profile information are processed normally without additional data.

This design preserves backward compatibility while allowing future semantic extensions without modifying the processing contract.

---

# Expression Model

The Expression Processor acts as the semantic dispatcher of the node.

It does not manipulate hardware directly.

Instead, it converts normalized semantic meaning into high-level visual expressions.

Current semantic states include:

| Semantic Event | Visual Expression |
|---------------|-------------------|
| `boot` | Boot indication |
| `idle` | Idle ambient lighting |
| `presence_detected` | Presence indication |
| `identity_authenticated` | Profile-aware lighting |
| `processing` | Processing indication |
| `system_error` | Error indication |

The Expression Processor never generates RGB values.

Its responsibility is limited to selecting the appropriate visual behavior according to the received semantic meaning.

---

# Authenticated User Profiles

The current implementation supports profile-aware ambient lighting.

When an `identity_authenticated` event contains an authenticated user identifier, the Expression Processor selects a dedicated visual profile.

Current mappings are:

| User Identifier | Selected Profile |
|-----------------|------------------|
| `claudio` | Claudio |
| `herminio` | Hermínio |
| `mariana` | Mariana |
| `student` | Mariana |
| `visitor` | Visitor |
| `guest` | Visitor |

Unknown or unsupported identifiers intentionally fall back to the Visitor profile.

This deterministic fallback prevents undefined visual behavior while preserving interoperability with future Cognitive Runtime extensions.

---

# RGB Expression Model

Visual behavior is intentionally organized into three abstraction levels.

```text
Primitive Colors
        │
        ▼
Semantic States
        │
        ▼
Authenticated Profiles
        │
        ▼
RGB Controller
```

This separation isolates semantic meaning from hardware implementation.

The Expression Processor selects semantic behavior.

The RGB Effects module determines the appropriate visual expression.

The RGB Controller applies that expression to the physical hardware.

---

# Platform Calibration

The Atom Matrix RGB Node preserves semantic compatibility with the other RGB Expression Nodes while applying hardware-specific visual calibration.

The semantic meaning remains identical across all supported hardware platforms.

Only the physical RGB values are adjusted to compensate for differences in LED characteristics and perceived color reproduction.

The calibrated values are centralized within the RGB Effects module, allowing future visual adjustments without modifying semantic processing or hardware control logic.

This design intentionally separates:

```text
Semantic Meaning

↓

Visual Calibration

↓

Hardware Output
```

As a result, future platform calibration can be performed without affecting the validated communication protocol or the semantic architecture.

---

# Repository Structure

The Atom Matrix RGB Node follows the standard repository organization adopted throughout the Ambient Physical AI project.

```text
firmware/
└── nodes/
    └── expression-node/
        └── atom-matrix-rgb-node/
            ├── CMakeLists.txt
            ├── README.md
            ├── sdkconfig
            ├── main/
            │   ├── atom_matrix_rgb_node.c
            │   ├── wifi_station.c
            │   ├── wifi_station.h
            │   ├── semantic_receiver.c
            │   ├── semantic_receiver.h
            │   ├── semantic_consumer.c
            │   ├── semantic_consumer.h
            │   ├── expression_processor.c
            │   ├── expression_processor.h
            │   ├── rgb_effects.c
            │   ├── rgb_effects.h
            │   ├── rgb_controller.c
            │   └── rgb_controller.h
            └── docs/
```

The firmware intentionally separates networking, semantic processing, visual behavior and hardware control into independent modules.

This organization improves maintainability while preserving a clear architectural boundary between software layers.

---

# Build Requirements

The current production firmware requires:

- ESP-IDF v5.4.x
- ESP32 toolchain
- Python environment configured by ESP-IDF
- M5Stack Atom Matrix
- USB connection for flashing and monitoring

The project is built using the standard ESP-IDF workflow.

---

# Build

```bash
idf.py build
```

---

# Flash

```bash
idf.py -p COMx flash
```

Replace `COMx` with the serial port assigned to the Atom Matrix board.

---

# Serial Monitor

```bash
idf.py monitor
```

The serial monitor provides diagnostic information including:

- firmware startup;
- RGB controller initialization;
- Wi-Fi connection status;
- IPv4 acquisition;
- Semantic Receiver startup;
- received Semantic Events;
- authenticated profile selection;
- error reporting.

These logs are intended to support engineering validation and troubleshooting.

---

# Validation Procedure

The validated engineering workflow is:

1. Build the firmware.
2. Flash the Atom Matrix.
3. Verify successful startup.
4. Confirm Wi-Fi connectivity.
5. Verify IPv4 acquisition.
6. Confirm Semantic Receiver initialization.
7. Send normalized Semantic Events from the Cognitive Runtime.
8. Verify that the expected visual expression is displayed.
9. Validate authenticated profile selection.
10. Confirm deterministic fallback behavior for unsupported profiles.

Successful completion of this sequence confirms correct integration of the node within the Expression Layer.

---

# Engineering Characteristics

Current implementation intentionally follows several engineering principles.

## Single Responsibility

Each software module owns a single responsibility.

Networking, semantic interpretation, visual mapping and hardware control remain isolated.

---

## Deterministic Processing

Every Semantic Event produces a deterministic visual response.

Unknown events are rejected.

Unknown user profiles fall back to the validated Visitor profile.

---

## Platform Independence

Semantic meaning is independent of hardware.

Visual calibration is platform-specific.

This allows different RGB devices to express the same semantic intent while compensating for hardware-specific optical characteristics.

---

## Layered Architecture

The firmware maintains a strict processing hierarchy.

```text
Semantic Communication
        ↓
Semantic Interpretation
        ↓
Visual Mapping
        ↓
Hardware Control
```

No module bypasses another layer.

This architecture simplifies maintenance, testing and future evolution.

---

# Related Documentation

Additional engineering information is maintained separately from this README.

Recommended companion documents include:

| Document | Purpose |
|----------|---------|
| `docs/notes/NOTE_ATOM_MATRIX_RGB_NODE_PROFILE_LIGHTING` | Platform calibration methodology, engineering decisions and profile-aware ambient lighting implementation. |
| Expression Layer documentation | Overall Expression Layer architecture. |
| Cognitive Runtime documentation | Semantic Event generation and communication contracts. |

This README intentionally focuses on the operational behavior of the node.

Detailed engineering rationale and implementation history are maintained in dedicated technical notes.

---

# Future Work

The following items are planned for future evolution and are **not part of the current validated implementation**.

Potential future improvements include:

- smooth fade transitions;
- breathing effects;
- low-frequency ambient pulsing;
- profile-specific animation timing;
- configurable brightness profiles;
- additional authenticated user profiles;
- future semantic attributes beyond `user_id`.

These enhancements are expected to be implemented within the existing layered architecture without modifying the validated communication contract.

---

# Final Status

Current implementation status:

| Component | Status |
|-----------|--------|
| Firmware | ✅ Validated |
| Modular Architecture | ✅ Implemented |
| Wi-Fi Communication | ✅ Validated |
| UDP Semantic Reception | ✅ Validated |
| Semantic Processing | ✅ Validated |
| Authenticated Profiles | ✅ Validated |
| Platform Calibration | ✅ Implemented |
| Expression Layer Integration | ✅ Ready |

The Atom Matrix RGB Node is considered a validated production component of the Ambient Physical AI Expression Layer.

---

# License

This firmware is part of the **Ambient Physical AI** project.

It is distributed under the repository license.

For licensing information, refer to the license defined at the repository root.