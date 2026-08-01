# SH1107 OLED Driver

## Ambient Physical AI

### Native ESP-IDF Driver for the M5Stack Unit OLED

---

# Overview

The `oled_sh1107` component provides the low-level driver for the M5Stack Unit OLED based on the SH1107 controller.

It is responsible exclusively for hardware communication with the display and exposes the rendering primitives required by higher-level runtime components.

The driver is implemented entirely using native ESP-IDF APIs and does not depend on Arduino, M5GFX, LovyanGFX or Arduino-ESP32.

Within the Ambient Runtime architecture, this component forms the lowest software layer responsible for visual output on the auxiliary OLED display.

---

# Purpose

The purpose of this component is to provide a reliable, reusable and hardware-independent interface for the SH1107 OLED controller.

Current responsibilities include:

- SH1107 controller initialization;
- I²C communication with the display;
- display configuration;
- framebuffer updates;
- page and column addressing;
- low-level rendering primitives.

The component intentionally does **not** implement application logic or context presentation.

---

# Position within the Ambient Runtime

The SH1107 driver occupies the lowest level of the Mini OLED presentation pipeline.

Higher-level runtime components remain independent from the display controller implementation.

```text
Ambient Runtime
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
SH1107 Controller
        │
        ▼
M5Stack Unit OLED
```

This separation allows presentation logic and hardware communication to evolve independently.

---

# Responsibilities

The `oled_sh1107` component is responsible only for hardware-level display services.

These responsibilities include:

## Display Initialization

Initialize the SH1107 controller using the validated configuration adopted by the Ambient Runtime.

---

## Display Configuration

Configure the operating parameters required by the OLED controller, including page addressing, column addressing and display operating mode.

---

## I²C Communication

Provide reliable communication between the ESP32-P4 and the SH1107 controller through the I²C bus.

All protocol-specific communication remains encapsulated within this component.

---

## Rendering Primitives

Expose low-level drawing operations used by higher-level runtime components.

Typical operations include:

- display initialization;
- display clearing;
- page updates;
- character rendering;
- string rendering;
- framebuffer transfer.

Rendering policies remain outside this component.

---

# Component Architecture

The driver follows a layered architecture.

```text
Application
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
driver/i2c_master
        │
        ▼
SH1107 Controller
        │
        ▼
Mini OLED
```

The component never interprets runtime information.

Its only responsibility is to execute display operations requested by higher application layers.

---

# Runtime Integration

The SH1107 driver participates in the Ambient Runtime through the Mini OLED presentation pipeline.

```text
Cognitive Runtime
        │
        ▼
semantic_event_receiver
        │
        ▼
ambient_context_snapshot
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
Mini OLED
```

The driver receives already formatted information from the `oled_context_presenter`.

It does not perform:

- semantic processing;
- user profile selection;
- message parsing;
- context interpretation;
- layout decisions.

Those responsibilities remain outside this component.

---

# Dependencies

The component depends on:

- ESP-IDF I²C Master Driver;
- SH1107 OLED controller;
- M5Stack Unit OLED hardware.

Within the Ambient Runtime, the driver is typically used together with:

- `oled_context_presenter`;
- `pahub`;
- `tab5_platform`.

---

# Public API

The public interface provides the services required for operating the display.

Typical responsibilities exposed through the public API include:

- display initialization;
- display clearing;
- text rendering;
- page updates;
- geometry validation support.

Higher-level runtime modules should access the display exclusively through this public interface.

---

# Build

The component is built automatically as part of the Ambient Runtime.

Typical build sequence:

```bash
cd firmware/nodes/ambient-runtime-node

idf.py build

idf.py flash

idf.py monitor
```

No additional configuration is required beyond the standard Ambient Runtime build process.

---

# Engineering Notes

The current implementation of the SH1107 driver follows the validated initialization sequence adopted for the M5Stack Unit OLED within the Ambient Runtime.

This implementation has been verified on the target hardware and represents the current engineering baseline for the project.

The driver should not be modified without validating the complete display initialization sequence on the physical device.

---

# Design Principles

The component was designed according to the engineering principles adopted throughout the Ambient Physical AI project.

## Native ESP-IDF Implementation

The driver is implemented exclusively using native ESP-IDF APIs.

It does not require:

- Arduino Core;
- Arduino-ESP32;
- M5GFX;
- LovyanGFX;
- external graphics frameworks.

This keeps the Ambient Runtime independent from Arduino-based software stacks.

---

## Separation of Responsibilities

The SH1107 driver performs only hardware-level operations.

Application behavior is intentionally delegated to higher runtime layers.

```text
Application Logic
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
Display Hardware
```

This separation minimizes coupling between application logic and display hardware.

---

## Hardware Encapsulation

The component encapsulates all controller-specific communication.

Higher application layers remain unaware of:

- initialization commands;
- page addressing;
- column addressing;
- controller configuration;
- I²C protocol details.

This simplifies maintenance and allows future hardware changes with minimal impact on the remainder of the runtime.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| SH1107 initialization | Validated |
| I²C communication | Validated |
| Display configuration | Validated |
| Character rendering | Validated |
| String rendering | Validated |
| Framebuffer updates | Validated |
| Runtime integration | Validated |

The driver is considered stable for the current Ambient Runtime baseline.

---

# Limitations

The `oled_sh1107` component intentionally remains limited to display services.

It does not implement:

- semantic message reception;
- JSON parsing;
- context management;
- user profile selection;
- rendering policies;
- screen layout decisions.

These responsibilities belong to higher-level runtime components.

---

# Engineering Investigation

During the integration of the Mini OLED into the Ambient Runtime, an engineering investigation was performed to validate the SH1107 initialization sequence and display geometry.

That investigation resulted in the current validated implementation adopted by the project.

The complete investigation, including:

- observed symptoms;
- root cause analysis;
- validation procedure;
- engineering decisions;
- integration details;

is documented separately in the following Engineering Technical Note:

```text
docs/notes/NOTE_002_SH1107_DISPLAY_GEOMETRY_VALIDATION.md
```

Maintaining the investigation as a Technical Note keeps this README focused on the permanent responsibilities of the component while preserving the complete engineering history.

---

# Related Components

The SH1107 driver operates together with the following Ambient Runtime components.

| Component | Relationship |
|-----------|--------------|
| `oled_context_presenter` | Generates the information presented on the Mini OLED. |
| `semantic_event_receiver` | Supplies normalized semantic context to the presenter. |
| `pahub` | Provides access to the external I²C channel used by the display. |
| `tab5_platform` | Supplies platform services required by the Ambient Runtime. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Overview of all runtime components. |
| `firmware/nodes/ambient-runtime-node/components/oled_context_presenter/README.md` | Context presentation layer. |
| `docs/notes/NOTE_002_SH1107_DISPLAY_GEOMETRY_VALIDATION.md` | Complete engineering investigation and validation of the SH1107 initialization sequence. |

---

# References

The implementation follows the engineering architecture adopted throughout the Ambient Physical AI project and has been validated as part of the Ambient Runtime integration.

Additional information regarding runtime architecture, platform services and component interactions can be found in the corresponding Ambient Runtime documentation.

---

# Conclusion

The `oled_sh1107` component provides the hardware abstraction required for the M5Stack Unit OLED used by the Ambient Runtime.

By restricting its responsibilities to low-level display services and delegating presentation logic to higher runtime layers, the component remains modular, reusable and easy to maintain.

The current implementation represents the validated baseline for the Ambient Runtime and serves as the reference driver for SH1107-based Mini OLED integration within the Ambient Physical AI ecosystem.