# Ambient Runtime Console

## Ambient Physical AI

### Runtime Status Console for the Ambient Runtime

---

# Overview

The `ambient_console` component implements the primary engineering interface of the Ambient Runtime.

Displayed on the M5Stack Tab5 LCD, the console provides a consolidated operational view of the embedded runtime by presenting information collected from multiple subsystems in a single interface.

Its primary purpose is to support engineering validation, runtime monitoring and system integration.

The component intentionally remains independent from hardware drivers, communication protocols and semantic processing.

Instead, it renders a normalized snapshot representing the current operational state of the Ambient Runtime.

---

# Purpose

The Ambient Runtime Console serves as the operational dashboard of the embedded runtime.

Current responsibilities include:

- displaying platform status;
- displaying network status;
- presenting environmental measurements;
- reporting hardware availability;
- exposing Cognitive Runtime connectivity;
- presenting the current operational state of the runtime.

The component is intended for engineering use during development, validation and system integration.

It is not designed as a user interface for end users.

---

# Position within the Ambient Runtime

The Ambient Runtime Console occupies the visualization layer of the runtime.

Rather than communicating directly with sensors or network services, it receives an already normalized runtime snapshot from the application.

```text
Application
        │
        ▼
ambient_console_data_t
        │
        ▼
ambient_console
        │
        ▼
tab5_platform
        │
        ▼
Tab5 LCD
```

This architecture separates runtime visualization from runtime operation.

---

# Responsibilities

The component is responsible exclusively for rendering runtime information.

Its current responsibilities include:

## Runtime Visualization

Render a complete operational view of the Ambient Runtime.

The console consolidates information originating from multiple runtime components without directly accessing them.

---

## Runtime Status

Present the current operational state of the runtime.

Current runtime states include:

- NETWORK READY
- LOCAL READY
- DEGRADED

These states are derived from the normalized runtime snapshot supplied by the application.

---

## Environmental Monitoring

Display measurements acquired from the environmental sensors.

Current validated measurements include:

- temperature;
- humidity;
- atmospheric pressure;
- ambient light.

The component performs no sensor acquisition.

---

## Network Monitoring

Display the current network status.

Current information includes:

- Wi-Fi connection status;
- IPv4 address;
- network mask;
- gateway;
- RSSI.

Network management remains outside this component.

---

## Hardware Monitoring

Present the operational status of hardware devices used by the Ambient Runtime.

Current hardware indicators include:

- PaHub;
- ENV-IV;
- DLight;
- Mini OLED.

Hardware validation is performed elsewhere.

The console only renders the resulting status.

---

## Cognitive Runtime Monitoring

Present the current operational status of the Cognitive Runtime integration.

Current information includes:

- connection state;
- most recently consumed Semantic Event;
- current Semantic Target.

The console intentionally presents only the latest operational snapshot.

It does not maintain event history or implement event processing.

---

# Component Architecture

The console follows a layered rendering architecture.

```text
Runtime Components
        │
        ▼
ambient_console_data_t
        │
        ▼
ambient_console
        │
        ▼
Framebuffer
        │
        ▼
tab5_platform
        │
        ▼
Tab5 LCD
```

The component remains completely independent from the origin of the information being displayed.

---

# Runtime Data Model

The Ambient Runtime Console operates exclusively on a normalized runtime structure.

```text
ambient_console_data_t
```

This structure represents the current operational snapshot of the runtime.

It aggregates information from multiple subsystems, including:

- environmental sensors;
- network infrastructure;
- hardware availability;
- runtime readiness;
- Cognitive Runtime connectivity.

By operating on a single normalized structure, the console remains completely decoupled from:

- sensor drivers;
- communication protocols;
- JSON payloads;
- UDP messages;
- semantic processing.

This separation significantly simplifies maintenance and future evolution.

---

# Public API

The public interface intentionally remains compact.

Current services include:

```c
esp_err_t ambient_console_init(void);
```

Initializes the console framebuffer and prepares the rendering infrastructure.

---

```c
esp_err_t ambient_console_render_static(void);
```

Renders the static console used during platform bring-up and display validation.

---

```c
esp_err_t ambient_console_render(
    const ambient_console_data_t *data
);
```

Renders the complete Runtime Console using the normalized runtime snapshot.

---

# Dependencies

The Ambient Runtime Console depends on:

- `tab5_platform`;
- framebuffer memory allocated in PSRAM;
- bitmap font resources.

The component intentionally avoids direct dependencies on:

- network services;
- sensor drivers;
- semantic communication;
- Cognitive Runtime protocols.

All required runtime information is supplied through the public data structure.

# Rendering Pipeline

The Ambient Runtime Console follows a deterministic rendering pipeline.

The application periodically constructs a normalized runtime snapshot and passes it to the console.

The console then generates the framebuffer and transfers it to the Tab5 display.

```text
Runtime Components
        │
        ▼
ambient_console_data_t
        │
        ▼
ambient_console_render()
        │
        ▼
Framebuffer
        │
        ▼
tab5_platform_draw_bitmap()
        │
        ▼
Tab5 LCD
```

The rendering process is entirely synchronous and stateless.

Each frame represents the current runtime state.

No rendering history is maintained.

---

# Runtime Integration

The Ambient Runtime Console integrates information originating from multiple runtime subsystems.

```text
Environmental Sensors
        │
        ▼
Runtime Snapshot

Network Services
        │
        ▼
Runtime Snapshot

Hardware Status
        │
        ▼
Runtime Snapshot

Cognitive Runtime
        │
        ▼
Runtime Snapshot

                │
                ▼

      ambient_console_data_t

                │
                ▼

       Ambient Runtime Console
```

The console never communicates directly with those subsystems.

Its only dependency is the normalized runtime snapshot supplied by the application.

---

# Display Organization

The Runtime Console is organized into logical sections.

Current implementation includes:

```text
Header

↓

System

↓

Environment

↓

I²C Network

↓

Runtime Status

↓

Cognitive Runtime
```

Each section presents a specific aspect of the Ambient Runtime without exposing implementation details of the underlying components.

---

# Engineering Principles

The component follows the engineering principles adopted throughout the Ambient Physical AI project.

## Separation of Responsibilities

The Ambient Runtime Console performs only runtime visualization.

It intentionally avoids:

- hardware access;
- sensor acquisition;
- network management;
- semantic message processing;
- communication protocols.

Those responsibilities belong to dedicated runtime components.

---

## Snapshot-Based Rendering

The console renders only the information contained in the current runtime snapshot.

This architecture provides several advantages:

- low coupling;
- deterministic rendering;
- simplified maintenance;
- reusable rendering code;
- independent runtime components.

The console never queries runtime modules directly.

---

## Stateless Visualization

The console does not maintain runtime history.

Each frame represents only the most recent operational state available at the time of rendering.

Likewise, the displayed Semantic Event and Semantic Target correspond only to the latest semantic information consumed by the Ambient Runtime.

Historical event management remains outside the scope of this component.

---

## Platform Independence

The console is implemented independently from the display controller.

Rendering is performed on a framebuffer before being transferred through the platform abstraction layer.

This approach isolates visualization logic from hardware-specific implementation.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| Framebuffer allocation | Validated |
| Static rendering | Validated |
| Dynamic runtime rendering | Validated |
| Environmental visualization | Validated |
| Network visualization | Validated |
| Hardware status visualization | Validated |
| Runtime status visualization | Validated |
| Cognitive Runtime visualization | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Related Components

The Ambient Runtime Console operates together with the following runtime components.

| Component | Relationship |
|-----------|--------------|
| `tab5_platform` | Transfers the framebuffer to the Tab5 LCD. |
| `ambient_network` | Supplies normalized network information. |
| `env_iv` | Supplies environmental measurements. |
| `dlight` | Supplies ambient light measurements. |
| `semantic_event_receiver` | Supplies the current Cognitive Runtime snapshot. |
| `pahub` | Supports external I²C peripherals whose status is displayed by the console. |
| `oled_context_presenter` | Independent presentation pipeline responsible for the Mini OLED. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Overview of all runtime components. |
| `firmware/nodes/ambient-runtime-node/main/README.md` | Runtime lifecycle and initialization sequence. |
| `components/tab5_platform/README.md` | Platform abstraction and framebuffer transfer. |
| `components/ambient_network/README.md` | Network infrastructure. |
| `components/semantic_event_receiver/README.md` | Semantic communication layer. |

---

# Conclusion

The `ambient_console` component provides the primary engineering interface of the Ambient Runtime.

By operating exclusively on a normalized runtime snapshot, it remains independent from hardware drivers, communication protocols and runtime services while presenting a comprehensive operational view of the embedded system.

This architecture allows the Runtime Console to evolve independently from the underlying runtime components and provides a stable visualization layer for engineering validation, integration and long-term maintenance.