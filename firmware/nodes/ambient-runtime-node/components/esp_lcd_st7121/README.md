# ESP LCD ST7121 Driver

## Ambient Physical AI

### Native ESP-IDF MIPI-DSI LCD Panel Driver

---

# Overview

The `esp_lcd_st7121` component provides the native ESP-IDF panel driver used by the Ambient Runtime to control the integrated LCD display of the M5Stack Tab5.

The component extends Espressif's `esp_lcd` framework by implementing the vendor-specific support required by LCD panels based on the ST7121 controller.

Rather than implementing a complete display subsystem, this component specializes the generic ESP-IDF MIPI-DPI panel by providing the initialization sequence and panel-specific behavior required by the target hardware.

---

# Purpose

The purpose of this component is to encapsulate all ST7121-specific display functionality behind the standard ESP-IDF LCD Panel API.

Current responsibilities include:

- creating the ST7121 LCD panel;
- configuring the MIPI-DSI interface;
- providing vendor-specific initialization;
- performing hardware and software reset;
- controlling display orientation;
- controlling color inversion;
- managing display visibility;
- supporting panel gap configuration.

The component intentionally remains independent from framebuffer management and application rendering.

---

# Position within the Ambient Runtime

The ST7121 driver belongs to the platform layer of the Ambient Runtime.

```text
Ambient Runtime
        │
        ▼
ambient_console
        │
        ▼
tab5_platform
        │
        ▼
esp_lcd_st7121
        │
        ▼
ESP-IDF esp_lcd
        │
        ▼
MIPI-DSI Interface
        │
        ▼
ST7121 LCD Controller
        │
        ▼
Tab5 LCD Panel
```

Higher-level runtime components interact only with the standard ESP-IDF LCD panel interface.

They remain completely independent from ST7121-specific implementation details.

---

# Responsibilities

The `esp_lcd_st7121` component is responsible exclusively for panel-level hardware control.

## Panel Creation

Create an LCD panel compatible with the ESP-IDF `esp_lcd_panel_t` abstraction.

The component allocates its internal state, validates configuration parameters and instantiates the underlying MIPI-DPI panel before extending its functionality with ST7121-specific operations. :contentReference[oaicite:0]{index=0}

---

## Vendor-Specific Initialization

Provide the initialization sequence required by LCD panels using the ST7121 controller.

The driver supports:

- the default initialization sequence supplied by the component;
- custom initialization sequences supplied through the vendor configuration structure.

This allows different panel manufacturers to use the same driver while preserving panel-specific initialization requirements. 

---

## MIPI Panel Integration

Integrate the ST7121-specific implementation with the ESP-IDF MIPI-DPI panel driver.

The component extends the standard panel implementation while preserving compatibility with the ESP-IDF LCD framework.

---

## Panel Control

Provide panel-level operations required during runtime.

Current operations include:

- hardware reset;
- software reset;
- display enable;
- display disable;
- display mirroring;
- color inversion;
- panel gap configuration.

These operations are implemented through the standard `esp_lcd_panel_t` interface.

---

## Vendor Configuration

Support vendor-specific configuration through the `st7121_vendor_config_t` structure.

The configuration includes:

- optional initialization commands;
- MIPI-DSI bus configuration;
- MIPI-DPI panel configuration.

This separation allows the driver implementation to remain generic while adapting to different hardware configurations. :contentReference[oaicite:2]{index=2}

---

# Driver Architecture

The component extends the ESP-IDF LCD framework rather than replacing it.

```text
Ambient Runtime
        │
        ▼
tab5_platform
        │
        ▼
esp_lcd_st7121
        │
        ▼
ESP-IDF esp_lcd_panel_t
        │
        ▼
ESP-IDF MIPI-DPI Panel
        │
        ▼
MIPI-DSI Hardware
        │
        ▼
ST7121 Controller
```

The driver wraps the generic ESP-IDF panel implementation and overrides selected operations with ST7121-specific behavior while preserving the standard programming interface. :contentReference[oaicite:3]{index=3}

---

# Supported Platform

The component is intended for ESP-IDF targets supporting the MIPI-DSI peripheral.

Compilation is conditioned by:

```text
SOC_MIPI_DSI_SUPPORTED
```

Platforms without MIPI-DSI support do not build this component. 

---

# Public API

The public interface intentionally exposes a single panel creation function.

```c
esp_err_t esp_lcd_new_panel_st7121(
    const esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_dev_config_t *panel_dev_config,
    esp_lcd_panel_handle_t *ret_panel
);
```

Responsibilities include:

- validating the supplied configuration;
- creating the underlying MIPI-DPI panel;
- allocating the internal ST7121 driver structure;
- replacing selected panel operations with ST7121 implementations;
- returning a standard `esp_lcd_panel_handle_t`.

The returned handle is fully compatible with the ESP-IDF LCD panel framework. 

---

# Dependencies

The component depends on the following ESP-IDF modules:

- `esp_lcd`;
- `driver`.

Its ESP-IDF component registration is:

```cmake
idf_component_register(
    SRCS "esp_lcd_st7121.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd driver
)
```

The component also depends on the ESP-IDF MIPI-DSI infrastructure and is intended to be used through the platform abstraction layer rather than directly by application code.

---

# Panel Initialization

The `esp_lcd_st7121` component is responsible for creating and initializing an ST7121-compatible LCD panel using the ESP-IDF LCD framework.

Initialization begins by validating the supplied configuration before allocating the internal driver context.

Once created, the component instantiates the standard ESP-IDF MIPI-DPI panel and extends it with ST7121-specific functionality. 

---

# MIPI-DSI and MIPI-DPI Integration

The driver operates as a bridge between the Ambient Runtime platform layer and the ESP-IDF LCD framework.

```text
tab5_platform
        │
        ▼
esp_lcd_st7121
        │
        ▼
ESP-IDF MIPI-DPI Panel
        │
        ▼
MIPI-DSI Bus
        │
        ▼
LCD Controller
```

The component does not implement a custom rendering engine.

Instead, it extends the standard ESP-IDF implementation with vendor-specific initialization and panel control. :contentReference[oaicite:1]{index=1}

---

# Initialization Command Sequence

Every ST7121 panel requires a vendor-specific initialization sequence before becoming operational.

The component supports two initialization modes.

## Default Initialization

When no external initialization table is supplied, the driver executes its internal validated initialization sequence.

This sequence contains the commands required to configure the controller before enabling normal display operation. :contentReference[oaicite:2]{index=2}

---

## Custom Initialization

The driver also accepts a vendor-defined initialization table through the configuration structure.

```text
st7121_vendor_config_t
```

This mechanism allows manufacturers or platform implementations to provide panel-specific command sequences without modifying the driver itself. 

---

# Panel Operation Overrides

The ST7121 driver extends the generic ESP-IDF LCD panel by replacing selected operations with controller-specific implementations.

The driver overrides:

- panel initialization;
- panel deletion;
- panel reset;
- display mirroring;
- color inversion;
- display enable and disable;
- gap configuration.

All remaining functionality continues to be provided by the underlying ESP-IDF MIPI-DPI implementation. :contentReference[oaicite:4]{index=4}

---

# Reset Operations

The component supports both hardware and software reset.

## Hardware Reset

When a reset GPIO is available, the driver performs a hardware reset by controlling the panel reset line according to the configured reset polarity.

---

## Software Reset

If no reset GPIO is configured, the driver performs a software reset using the standard LCD reset command.

The reset strategy is selected automatically according to the supplied panel configuration. :contentReference[oaicite:5]{index=5}

---

# Display Mirroring

The driver supports independent horizontal and vertical mirroring.

Mirror operations are implemented by updating the controller's MADCTL register through the standard ESP-IDF LCD interface.

This functionality is useful when adapting the display orientation to different mechanical installations without modifying application rendering. :contentReference[oaicite:6]{index=6}

---

# Color Inversion

The component supports runtime color inversion.

The implementation uses the standard LCD controller commands to switch between:

- normal display mode;
- inverted display mode.

This operation affects only panel output and does not modify the framebuffer contents. :contentReference[oaicite:7]{index=7}

---

# Display Power Control

The driver exposes operations to control panel visibility.

Current supported operations include:

- Display ON;
- Display OFF.

These operations are implemented using the standard LCD display control commands while preserving compatibility with the ESP-IDF panel interface. :contentReference[oaicite:8]{index=8}

---

# Gap Configuration

The component supports configurable horizontal and vertical display offsets.

Gap values are stored internally and forwarded to the underlying MIPI-DPI implementation when supported.

This mechanism allows platform-specific display alignment without modifying application rendering code. :contentReference[oaicite:9]{index=9}

---

# Runtime Behavior

Once initialization is complete, the driver operates transparently beneath the Ambient Runtime.

```text
Application Rendering
        │
        ▼
Framebuffer
        │
        ▼
tab5_platform
        │
        ▼
esp_lcd_st7121
        │
        ▼
ESP-IDF LCD Framework
        │
        ▼
LCD Panel
```

Application components never communicate directly with the ST7121 controller.

All interaction occurs through the standard ESP-IDF LCD panel abstraction, ensuring compatibility with the broader display framework.

---

# Runtime Integration

Within the Ambient Runtime, the `esp_lcd_st7121` component operates exclusively as a platform driver.

It does not perform application rendering, graphical layout or framebuffer management.

Instead, it provides the hardware interface used by the platform layer to transfer rendered images to the integrated LCD panel.

```text
Ambient Runtime
        │
        ▼
ambient_console
        │
        ▼
tab5_platform
        │
        ▼
esp_lcd_st7121
        │
        ▼
ESP-IDF LCD Framework
        │
        ▼
ST7121 LCD Controller
        │
        ▼
Integrated LCD Panel
```

The separation between rendering and panel control keeps the display subsystem modular and simplifies future hardware evolution.

---

# Engineering Principles

The component follows the engineering principles adopted throughout the Ambient Physical AI project.

## Native ESP-IDF Implementation

The driver is implemented entirely using the ESP-IDF LCD framework.

It does not depend on:

- Arduino;
- Arduino-ESP32;
- LovyanGFX;
- M5GFX.

This approach preserves compatibility with the native ESP-IDF graphics infrastructure while minimizing external dependencies. 

---

## Extension Rather Than Replacement

The component extends the generic ESP-IDF MIPI-DPI panel implementation instead of replacing it.

Only panel-specific operations are overridden.

All remaining functionality continues to be provided by the ESP-IDF LCD framework.

This design minimizes duplicated code and simplifies long-term maintenance. :contentReference[oaicite:1]{index=1}

---

## Hardware Encapsulation

Controller-specific implementation details remain fully encapsulated.

Application modules never manipulate:

- LCD controller commands;
- initialization sequences;
- reset timing;
- MADCTL configuration;
- MIPI-DSI transactions.

These responsibilities remain private to the driver.

---

## Platform Independence

The Ambient Runtime interacts only with the standard `esp_lcd_panel_t` interface.

This abstraction allows the platform layer to remain largely independent from the specific LCD controller implementation.

Future hardware revisions requiring a different panel controller should primarily impact the platform layer while preserving higher-level application code.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| ST7121 panel creation | Validated |
| Vendor-specific initialization | Validated |
| MIPI-DPI integration | Validated |
| Hardware reset | Validated |
| Software reset | Validated |
| Display mirroring | Validated |
| Color inversion | Validated |
| Display ON/OFF control | Validated |
| Gap configuration | Validated |
| ESP-IDF LCD integration | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Limitations

The `esp_lcd_st7121` component intentionally focuses on low-level LCD panel control.

It does not implement:

- framebuffer allocation;
- graphics rendering;
- font rendering;
- window management;
- widgets;
- user interface logic;
- animation;
- application layouts.

Those responsibilities belong to higher layers of the Ambient Runtime.

---

# Licensing and Provenance

This component is based on the ESP-IDF LCD framework and preserves the original licensing information provided by Espressif Systems.

The source files retain their SPDX license headers and copyright notices.

Any future modifications should preserve these licensing statements in accordance with the Apache License 2.0. 

---

# Related Components

The ST7121 driver operates together with the following Ambient Runtime components.

| Component | Relationship |
|-----------|--------------|
| `tab5_platform` | Creates and configures the LCD panel using this driver. |
| `ambient_console` | Renders the Runtime Console displayed on the integrated LCD. |
| `ambient_network` | Supplies network information displayed by the Runtime Console. |
| `env_iv` | Supplies environmental measurements displayed by the Runtime Console. |
| `dlight` | Supplies ambient light measurements displayed by the Runtime Console. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/components/tab5_platform/README.md` | Platform abstraction layer. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md` | Runtime Console. |

---

# Conclusion

The `esp_lcd_st7121` component provides the native ESP-IDF driver responsible for controlling the ST7121 LCD panel used by the Ambient Runtime.

By extending the standard ESP-IDF LCD framework rather than replacing it, the component encapsulates all controller-specific functionality while preserving compatibility with the native display architecture.

Its responsibilities are intentionally limited to panel creation, initialization and hardware control, allowing rendering, platform services and application logic to remain cleanly separated within the Ambient Physical AI architecture.
