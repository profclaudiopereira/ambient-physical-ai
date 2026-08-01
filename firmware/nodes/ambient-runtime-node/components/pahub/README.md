# PaHub Driver

## Ambient Physical AI

### Native ESP-IDF Driver for the M5Stack PaHub

---

# Overview

The `pahub` component provides a lightweight hardware abstraction for the M5Stack PaHub (Port Hub) used by the Ambient Runtime.

Its sole responsibility is to select the active channel of the PaHub through the I²C bus, allowing multiple Grove-compatible peripherals to share a single ESP32-P4 I²C interface.

The component intentionally exposes a minimal public API and remains independent from any specific peripheral connected to the PaHub.

---

# Purpose

The purpose of this component is to provide a reusable interface for controlling the M5Stack PaHub multiplexer.

Current responsibilities include

- selecting the active PaHub channel;
- validating channel numbers;
- transmitting channel selection commands over I²C;
- providing a simple hardware abstraction for higher-level runtime components.

The component performs no device initialization and contains no application-specific logic.

---

# Position within the Ambient Runtime

The PaHub operates as an infrastructure component shared by multiple peripherals connected to the external Grove bus.

```text
Ambient Runtime
        │
        ▼
Application Components
        │
        ▼
pahub
        │
        ▼
M5Stack PaHub
        │
        ├───────────────┐
        │               │
        ▼               ▼
ENV-IV         Mini OLED
```

The PaHub isolates channel selection from higher-level runtime components.

---

# Responsibilities

The `pahub` component is responsible only for channel selection.

Current responsibilities include

## Channel Selection

Activate one of the available PaHub channels through the I²C control interface.

---

## Parameter Validation

Validate the requested channel before transmitting commands to the hardware.

Invalid channel numbers are rejected before communication begins.

---

## I²C Communication

Transmit the channel selection command to the PaHub controller.

The component encapsulates all low-level communication required for channel switching.

---

## Channel Stabilization

After selecting a channel, the component waits briefly before returning control to the caller.

This delay allows the selected downstream device to become available before subsequent I²C transactions are performed.

---

# Component Architecture

The component occupies the infrastructure layer of the Ambient Runtime.

```text
Application
        │
        ▼
pahub
        │
        ▼
ESP-IDF I²C Master Driver
        │
        ▼
M5Stack PaHub
        │
        ├───────────────┐
        │               │
        ▼               ▼
External Grove Devices
```

The component remains completely independent from the devices connected to each channel.

---

# Public API

The public interface intentionally consists of a single service.

```c
esp_err_t pahub_select_channel(
    i2c_master_bus_handle_t bus,
    uint8_t channel
);
```

Responsibilities of this function include

- validating the requested channel;
- configuring temporary I²C access to the PaHub;
- selecting the requested channel;
- releasing the temporary device handle;
- returning the resulting ESP-IDF status code.

The component exposes no additional runtime state.

---

# Dependencies

The component depends only on

- ESP-IDF I²C Master Driver;
- FreeRTOS (timing delay);
- ESP-IDF logging services.

No additional runtime components are required.

---

# Build

The component is built automatically as part of the Ambient Runtime.

Typical build sequence

```bash
cd firmwarenodesambient-runtime-node

idf.py build

idf.py flash

idf.py monitor
```

No additional configuration is required.

---

# Current Status

The current implementation provides the following validated functionality.

 Capability  Status 
--------------------
 Channel validation  Validated 
 I²C communication  Validated 
 Channel selection  Validated 
 Temporary device management  Validated 
 Runtime integration  Validated 

The component is considered stable for the current Ambient Runtime baseline.

---

# Design Principles

The implementation follows the same engineering principles adopted throughout the Ambient Physical AI project.

## Single Responsibility

The component performs only channel selection.

It intentionally avoids implementing peripheral-specific behavior.

---

## Hardware Encapsulation

All PaHub-specific communication remains isolated within this component.

Higher-level modules never manipulate the PaHub directly.

---

## Reusability

The component can be reused by any runtime module requiring access to devices connected through the PaHub.

No assumptions are made regarding the connected peripherals.

---

# Related Components

The PaHub driver is commonly used together with

 Component  Relationship 
-------------------------
 `tab5_platform`  Provides the external I²C bus used by the PaHub. 
 `oled_sh1107`  Accesses the Mini OLED through a selected PaHub channel. 
 `env_iv`  Accesses the environmental sensor through a selected PaHub channel. 

---

# Related Documentation

 Document  Description 
-----------------------
 `firmwarenodesambient-runtime-nodeREADME.md`  Ambient Runtime architectural overview. 
 `firmwarenodesambient-runtime-nodecomponentsREADME.md`  Overview of all runtime components. 
 `firmwarenodesambient-runtime-nodecomponentstab5_platformREADME.md`  Platform abstraction and I²C services. 
 `firmwarenodesambient-runtime-nodecomponentsoled_sh1107README.md`  SH1107 display driver. 
 `firmwarenodesambient-runtime-nodecomponentsenv_ivREADME.md`  Environmental sensor interface. 

---

# Conclusion

The `pahub` component provides a minimal, reusable and hardware-independent interface for selecting channels on the M5Stack PaHub.

By restricting its responsibilities to channel selection and encapsulating all PaHub-specific communication, the component remains simple, maintainable and reusable across multiple Ambient Runtime peripherals while preserving the modular architecture of the project.