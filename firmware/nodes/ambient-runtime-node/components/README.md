# Ambient Runtime Components

## Ambient Physical AI

### Modular ESP-IDF Components for the Ambient Runtime

---

# Overview

The Ambient Runtime is organized as a collection of reusable ESP-IDF components.

Each component encapsulates a single engineering responsibility and exposes a stable public interface through the standard ESP-IDF component structure.

This organization keeps application logic independent from hardware-specific implementation while simplifying maintenance, testing and future evolution.

The components collectively provide the platform services, infrastructure, device interfaces and runtime services required by the Ambient Runtime Node.

---

# Component Architecture

The component hierarchy reflects the software architecture of the Ambient Runtime.

```text
                    Ambient Runtime

                           │

        ┌──────────────────┼──────────────────┐
        │                  │                  │

        ▼                  ▼                  ▼

Platform Layer     Infrastructure Layer   Runtime Layer

tab5_platform      ambient_network        ambient_console
pahub                                      semantic_event_receiver
                                           oled_context_presenter

        │
        ▼

Device Layer

env_iv
dlight
oled_sh1107
```

Each layer exposes services to higher application layers while hiding implementation details.

This separation minimizes coupling between platform hardware, communication infrastructure and application-level functionality.

---

# Repository Organization

All reusable modules are located under the standard ESP-IDF component directory.

```text
components/
│
├── README.md
│
├── ambient_console/
├── ambient_network/
├── dlight/
├── env_iv/
├── oled_context_presenter/
├── oled_sh1107/
├── pahub/
├── semantic_event_receiver/
└── tab5_platform/
```

Each component contains its own source code, public headers and build configuration.

Whenever justified by the complexity of the component, an individual `README.md` documents its responsibilities, public interfaces and implementation decisions.

The present document provides only the architectural overview of the component collection.

Implementation details remain within the corresponding component documentation.

---

# Component Overview

| Component | Responsibility | Status |
|-----------|----------------|--------|
| `ambient_console` | Runtime status visualization on the Tab5 display | Stable |
| `ambient_network` | Network infrastructure and connectivity management | Stable |
| `semantic_event_receiver` | Reception, validation and normalization of semantic messages | Stable |
| `oled_context_presenter` | Presentation of semantic context on the Mini OLED display | Stable |
| `tab5_platform` | Hardware abstraction for the M5Stack Tab5 platform | Stable |
| `pahub` | External I²C multiplexer services | Stable |
| `env_iv` | Environmental sensor interface | Stable |
| `dlight` | Ambient light sensor interface | Stable |
| `oled_sh1107` | SH1107 OLED display driver | Stable |

---

# Component Responsibilities

The Ambient Runtime is intentionally divided into components with explicit engineering responsibilities.

## Platform Layer

The Platform Layer abstracts hardware features that are specific to the M5Stack Tab5 platform.

Current responsibilities include:

- platform initialization;
- display initialization;
- internal and external I²C buses;
- platform-specific hardware configuration;
- backlight control;
- framebuffer services.

The remainder of the application remains independent from board-specific implementation.

---

## Infrastructure Layer

Infrastructure components provide services shared by the entire runtime.

These include:

- network initialization;
- Wi-Fi connectivity;
- communication infrastructure;
- external I²C multiplexing.

Infrastructure components provide services to higher layers without exposing implementation details.

---

## Runtime Layer

Runtime components implement the operational behavior of the Ambient Runtime.

Their responsibilities include:

- runtime status visualization;
- reception of semantic information;
- normalization of received context;
- contextual presentation on the auxiliary display.

These modules consume normalized runtime data rather than directly interacting with hardware.

---

## Device Layer

Device components provide interfaces to individual peripherals.

Current device interfaces include:

- environmental sensing;
- ambient light measurement;
- auxiliary OLED display services.

Drivers intentionally remain independent from application logic and expose only hardware-specific functionality.

---

# Engineering Principles

All Ambient Runtime components follow the same engineering principles.

## Single Responsibility

Each component owns one clearly defined engineering responsibility.

Responsibilities are intentionally isolated to simplify maintenance and future evolution.

---

## Hardware Encapsulation

Low-level hardware access remains confined to dedicated components.

Higher application layers never manipulate hardware peripherals directly.

---

## Stable Public Interfaces

Every component exposes a small, well-defined public API through its `include/` directory.

Internal implementation details remain private to the component.

---

## ESP-IDF Native Organization

All modules follow the standard ESP-IDF component architecture.

This allows each component to be developed, tested and maintained independently while remaining fully integrated into the application build system.

---

# Documentation Organization

Component documentation follows the same modular organization adopted throughout the Ambient Physical AI project.

The purpose of this document is to introduce the component architecture.

Detailed implementation, public APIs and engineering decisions are documented within the individual README files of each component.

---

# Component Summary

The following sections summarize the purpose of each component and indicate where implementation details can be found.

The goal is to provide an architectural overview without duplicating the documentation maintained by each individual component.

---

## ambient_console

### Purpose

Implements the Runtime Console displayed on the primary Tab5 LCD.

The console consolidates operational information generated by the Ambient Runtime, providing a single interface for engineering validation and runtime monitoring.

Current information includes:

- environmental measurements;
- network status;
- platform status;
- runtime readiness;
- Cognitive Runtime status.

The console consumes normalized runtime structures and remains independent from sensor drivers and communication modules.

For implementation details, refer to:

```text
components/ambient_console/README.md
```

---

## ambient_network

### Purpose

Provides the networking infrastructure required by the Ambient Runtime.

Its responsibilities include:

- Wi-Fi initialization;
- connection management;
- network monitoring;
- runtime network status;
- public network snapshot.

The remainder of the application accesses only the public network interface exposed by this component.

For implementation details, refer to:

```text
components/ambient_network/README.md
```

---

## semantic_event_receiver

### Purpose

Acts as the communication gateway between the Cognitive Runtime and the Ambient Runtime.

Responsibilities include:

- semantic message reception;
- message validation;
- context normalization;
- runtime snapshot management.

The component isolates transport-specific details from the remainder of the application.

For implementation details, refer to:

```text
components/semantic_event_receiver/README.md
```

---

## oled_context_presenter

### Purpose

Transforms normalized semantic context into information suitable for presentation on the auxiliary Mini OLED display.

Rendering decisions are based exclusively on the normalized runtime snapshot.

The component remains independent from communication protocols and hardware drivers.

For implementation details, refer to:

```text
components/oled_context_presenter/README.md
```

---

## tab5_platform

### Purpose

Implements the hardware abstraction layer for the M5Stack Tab5 platform.

Responsibilities include:

- platform initialization;
- display initialization;
- backlight control;
- framebuffer services;
- internal I²C bus;
- external Port A I²C bus;
- board-specific hardware initialization.

Platform-specific implementation remains isolated from the remainder of the application.

For implementation details, refer to:

```text
components/tab5_platform/README.md
```

---

## pahub

### Purpose

Provides access to external Grove devices connected through the M5Stack PaHub I²C multiplexer.

Channel selection and multiplexing remain encapsulated within this component.

For implementation details, refer to:

```text
components/pahub/README.md
```

---

## env_iv

### Purpose

Provides access to the environmental sensors available in the M5Stack ENV-IV unit.

Current validated measurements include:

- temperature;
- humidity;
- atmospheric pressure.

For implementation details, refer to:

```text
components/env_iv/README.md
```

---

## dlight

### Purpose

Provides ambient light measurements used by the Ambient Runtime.

The component exposes a hardware-independent interface for retrieving illumination values.

For implementation details, refer to:

```text
components/dlight/README.md
```

---

## oled_sh1107

### Purpose

Implements the low-level driver for the SH1107 Mini OLED display.

The driver is intentionally limited to display services.

Presentation logic remains the responsibility of the `oled_context_presenter` component.

For implementation details, refer to:

```text
components/oled_sh1107/README.md
```

---

# Current Status

The current implementation includes the following validated components.

| Component | Status |
|-----------|--------|
| `ambient_console` | Validated |
| `ambient_network` | Validated |
| `semantic_event_receiver` | Validated |
| `oled_context_presenter` | Validated |
| `tab5_platform` | Validated |
| `pahub` | Validated |
| `env_iv` | Validated |
| `dlight` | Validated |
| `oled_sh1107` | Validated |

The current component set represents the baseline implementation of the Ambient Runtime.

---

# Future Components

Future functionality should be implemented as independent ESP-IDF components whenever appropriate.

Potential future additions include:

- MQTT integration;
- OTA update services;
- additional environmental sensors;
- new hardware interfaces;
- additional runtime services.

Future components should follow the same architectural principles described in this document.

---

# Repository Navigation

The recommended reading order for developers new to the Ambient Runtime is:

```text
README.md
        │
        ▼
components/README.md
        │
        ▼
main/README.md
        │
        ▼
Individual Component README files
```

This progression introduces the overall architecture before moving into implementation-specific documentation.

---

# Related Documentation

The Ambient Runtime documentation is organized into multiple levels.

| Document | Scope |
|----------|-------|
| `README.md` | Architectural overview of the Ambient Runtime. |
| `components/README.md` | Organization and responsibilities of the runtime components. |
| `main/README.md` | Application entry point and execution lifecycle. |
| Component `README.md` files | Detailed documentation for individual components. |

This layered organization minimizes duplicated information while improving maintainability.

---

# Conclusion

The Ambient Runtime component architecture follows the engineering principles adopted throughout the Ambient Physical AI project:

- modularity;
- explicit responsibilities;
- hardware abstraction;
- reusable software components;
- stable public interfaces;
- native ESP-IDF component organization.

By decomposing the runtime into independent components, the project improves maintainability, portability and long-term scalability while keeping implementation details isolated within their respective modules.