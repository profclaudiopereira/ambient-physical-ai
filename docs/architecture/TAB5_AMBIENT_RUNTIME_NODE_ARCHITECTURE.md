# TAB5 Ambient Runtime Node Architecture

> **Ambient Physical AI**  
> **Distributed Cognitive Ecosystem Powered by StackFlow**

---

# Purpose

This document describes the internal architecture of the **Ambient Runtime Node** implemented on the **M5Stack Tab5 (ESP32-P4)**.

Its objective is to explain how the hardware resources, software components and communication layers interact to transform the Tab5 into the physical execution layer of the Ambient Physical AI ecosystem.

This document complements the project README and should be considered the architectural reference for this node.

---

# Architecture Diagram

![TAB5 Ambient Runtime Node Architecture](tab5_ambient_runtime_node_system_architecture.png)

---

# Overview

The Ambient Runtime Node is responsible for the physical interaction between the cognitive system and the surrounding environment.

Unlike the Cognitive Runtime (AX630C), which performs reasoning and contextual decision making, the Tab5 is responsible for interacting with physical devices connected to the environment.

Current responsibilities include:

- Environmental sensing
- Local visualization
- Physical feedback
- Wi-Fi communication
- Future distributed services

---

# Hardware Architecture

The Tab5 is centered around the ESP32-P4 processor.

Internally it exposes two independent I²C buses.

```text
System I²C
│
├── PI4IOE1
├── PI4IOE2
├── Display
├── Touch
└── Internal peripherals

Port A I²C
│
└── PaHub
    ├── ENV-IV
    ├── Mini OLED
    └── DLight
```

This separation isolates the board infrastructure from external sensors.

---

# ESP32-P4 Responsibilities

The ESP32-P4 executes the Ambient Runtime firmware.

Current responsibilities include:

- Board initialization
- Display initialization
- Port A I²C management
- PaHub management
- Sensor acquisition
- OLED rendering
- Runtime Console
- Wi-Fi Host
- Future MQTT integration

---

# ESP32-C6 Responsibilities

The Tab5 contains a dedicated ESP32-C6.

Unlike a conventional ESP32 module, the ESP32-C6 is **not directly controlled by user firmware**.

Instead, it operates as a dedicated wireless coprocessor.

Responsibilities include:

- Wi-Fi PHY
- Wi-Fi MAC
- 802.11 protocol stack
- SDIO communication with ESP32-P4

The ESP32-P4 communicates with the C6 through Espressif's **ESP-Hosted** architecture.

---

# ESP-Hosted Architecture

The communication between both processors follows the architecture below.

```text
Application

↓

ESP-IDF Wi-Fi API

↓

ESP Hosted

↓

SDIO

↓

ESP32-C6 Firmware

↓

Wi-Fi Network
```

The application never communicates directly with the ESP32-C6 firmware.

All communication is performed through the ESP Hosted middleware.

---

# Major Engineering Discovery

During the Ambient Runtime implementation, an important hardware behavior was identified.

Initially, the Wi-Fi subsystem appeared non-functional even after the ESP Hosted software components had been correctly integrated.

The investigation demonstrated that the issue was **not software related**.

The ESP32-C6 remained electrically powered off.

---

# PI4IOE2 Power Controller

The investigation revealed that the Tab5 powers the ESP32-C6 through a second I²C GPIO expander.

```text
ESP32-P4

↓

System I²C

↓

PI4IOE2 (0x44)

↓

Bit P0

↓

WLAN_PWR_EN

↓

ESP32-C6
```

Unless this signal is asserted, the wireless coprocessor never starts.

---

# Reference Implementation

The official M5Stack firmware initializes PI4IOE2 during board startup.

The GPIO expander configures several power rails, including:

- WLAN power
- USB 5V
- Battery charging
- QC charging

The relevant initialization writes the output register so that **P0 (WLAN_PWR_EN)** is driven HIGH.

Only after this configuration does the ESP32-C6 become operational.

---

# Ambient Runtime Implementation

The Ambient Runtime now reproduces this initialization.

During startup:

1. PI4IOE1 is initialized.
2. LCD and Touch reset lines are configured.
3. PI4IOE2 is initialized.
4. WLAN power is enabled.
5. ESP32-C6 powers up.
6. ESP Hosted initializes.
7. Wi-Fi becomes available.

This sequence reproduces the behavior implemented in the official M5Stack BSP while remaining independent from it.

---

# Current Peripheral Topology

```text
Port A

PaHub

├── Channel 0
│      ENV-IV
│      • SHT40
│      • BMP280
│
├── Channel 1
│      Mini OLED
│
└── Channel 2
       DLight
```

The Ambient Runtime periodically switches the active PaHub channel before accessing each device.

---

# Ambient Runtime Console

The Runtime Console aggregates the current environmental state.

Current information displayed includes:

- Temperature
- Humidity
- Pressure
- Ambient Light
- PaHub status
- ENV-IV status
- DLight status
- Mini OLED status
- Wi-Fi status
- Cognitive Runtime status

This console serves as the operational dashboard during development.

---

# Software Components

The current node is organized as independent reusable components.

```text
main/

↓

tab5_platform

↓

ambient_console

↓

ambient_network

↓

ENV-IV

↓

DLight

↓

OLED

↓

PaHub
```

This modular organization allows each subsystem to evolve independently.

---

# Current Status

Validated:

- ESP32-P4 initialization
- LCD initialization
- Port A I²C
- PaHub
- ENV-IV
- Mini OLED
- DLight
- Runtime Console
- PI4IOE2 Wi-Fi power initialization
- ESP Hosted integration
- ESP32-C6 power-up

Pending:

- Wi-Fi connection manager
- UDP services
- MQTT integration
- Ambient synchronization
- Cognitive Runtime communication

---

# Future Evolution

The Ambient Runtime Node will progressively evolve into the physical execution layer of Ambient Physical AI.

Future capabilities include:

- MQTT communication
- Distributed semantic events
- Ambient synchronization
- Multi-display support
- Expression devices
- Lighting orchestration
- Environmental adaptation
- Distributed runtime services

---

# Conclusion

The discovery of the **PI4IOE2-controlled WLAN power rail** represented a significant milestone in understanding the internal architecture of the Tab5.

Rather than being a software defect, the inability to use Wi-Fi was traced to the board's power management architecture.

By reproducing the official BSP initialization sequence, the Ambient Runtime now performs the complete hardware bring-up required to activate the ESP32-C6 coprocessor, establishing a solid foundation for future wireless networking capabilities.