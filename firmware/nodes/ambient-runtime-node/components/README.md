# Components

## Ambient Runtime Node

This directory contains all reusable ESP-IDF components used by the Ambient Runtime Node running on the M5Stack Tab5 (ESP32-P4).

Each component encapsulates a single hardware peripheral or platform service, following ESP-IDF component architecture principles.

---

# Architecture

```
components/

├── ambient_console/
├── dlight/
├── env_iv/
├── oled_sh1107/
├── pahub/
└── tab5_platform/
```

Each component exposes a clean public API through its `include/` directory while hiding implementation details inside the component source.

---

# Component Overview

| Component | Purpose | Status |
|-----------|---------|--------|
| ambient_console | Graphical Ambient Runtime dashboard rendered on the Tab5 LCD | Stable |
| tab5_platform | Tab5 platform abstraction (display, backlight, framebuffer, Port A I²C) | Stable |
| pahub | M5Stack PaHub I²C multiplexer driver | Stable |
| env_iv | Driver for the ENV-IV environmental sensor unit (SHT40 + BMP280) | Stable |
| oled_sh1107 | Driver for the Mini OLED SH1107 display | Stable |
| dlight | Driver for the DLight ambient light sensor | Stable |

---

# Component Details

---

## ambient_console

### Responsibility

Implements the graphical Ambient Runtime Console displayed on the Tab5 LCD.

Current responsibilities include:

- framebuffer management
- bitmap rendering
- text rendering
- runtime dashboard
- hardware status visualization

Current information displayed:

- Temperature
- Humidity
- Pressure
- Ambient Light
- Hardware Status
- Runtime Status

The console is intentionally independent from the hardware drivers.

It only consumes an `ambient_console_data_t` structure provided by the application.

---

## tab5_platform

### Responsibility

Implements the hardware abstraction layer for the M5Stack Tab5.

Provides:

- LCD initialization
- MIPI DSI panel setup
- Framebuffer rendering
- Backlight control
- Port A I²C initialization
- PSRAM framebuffer allocation

The remainder of the project never interacts directly with LCD or BSP internals.

---

## pahub

### Responsibility

Driver for the M5Stack PaHub.

Provides channel selection for multiple Grove devices connected through a single I²C bus.

Current channel allocation:

| Channel | Device |
|----------|--------|
| CH0 | ENV-IV |
| CH1 | Mini OLED |
| CH2 | DLight |

---

## env_iv

### Responsibility

Driver for the M5Stack ENV-IV unit.

Supported sensors:

- SHT40
    - temperature
    - humidity

- BMP280
    - pressure
    - temperature

API returns typed measurement structures ready for higher application layers.

---

## oled_sh1107

### Responsibility

Driver for the Mini OLED display.

Current capabilities:

- initialization
- text rendering
- simple diagnostics

Currently used as a secondary diagnostic display.

---

## dlight

### Responsibility

Driver for the M5Stack DLight sensor.

Provides:

- ambient light measurement (lux)

Current API returns floating-point lux values.

---

# Design Principles

Every component follows the same engineering principles:

- single responsibility
- reusable
- hardware encapsulation
- public API through include/
- implementation hidden inside component
- ESP-IDF native component architecture

---

# Current Status

| Component | Status |
|-----------|--------|
| ambient_console | Stable |
| tab5_platform | Stable |
| pahub | Stable |
| env_iv | Stable |
| oled_sh1107 | Stable |
| dlight | Stable |

---

# Pending Components

The following components are planned for future development:

- Wi-Fi Remote (ESP-Hosted)
- MQTT
- Cognitive Runtime Client
- Semantic Event Client
- OTA Update
- Identity Client

These modules will be added as independent ESP-IDF components following the same architecture.

---

# Engineering Notes

Keeping each hardware feature inside an independent ESP-IDF component significantly improves:

- maintainability
- portability
- testability
- code reuse
- documentation quality
- long-term scalability

This organization also aligns with the modular architecture adopted by the Ambient Physical AI project.