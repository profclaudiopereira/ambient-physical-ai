# Tab5 Platform

## Ambient Physical AI

### Hardware Platform Abstraction Layer for the M5Stack Tab5

---

# Overview

The `tab5_platform` component implements the hardware abstraction layer (HAL) for the M5Stack Tab5 used by the Ambient Runtime.

It centralizes all board-specific initialization required before higher-level runtime components become operational.

Rather than implementing application logic, the component prepares the embedded platform by configuring communication buses, powering onboard peripherals, initializing the display subsystem and exposing reusable platform services to the remainder of the runtime.

This component is intentionally the only module that contains detailed knowledge of the Tab5 hardware architecture. 

---

# Purpose

The purpose of the `tab5_platform` component is to isolate hardware-specific initialization from the rest of the Ambient Runtime.

Current responsibilities include:

- initializing the internal I²C bus;
- initializing the external PORT A I²C bus;
- configuring both PI4IOE I/O expanders;
- enabling the onboard ESP32-C6 wireless coprocessor;
- resetting the LCD and touch controller;
- configuring LCD backlight control;
- acquiring the MIPI DSI power domain;
- creating the MIPI DSI communication bus;
- creating the LCD panel interface;
- initializing the ST7121 display driver;
- exposing platform services used by higher-level runtime components.

The component intentionally performs no application-specific processing.

---

# Position within the Ambient Runtime

The Tab5 Platform forms the hardware foundation of the Ambient Runtime.

```text
Ambient Runtime
        │
        ├──────────────┐
        │              │
        ▼              ▼
ambient_console   semantic_event_receiver
        │              │
        └──────┬───────┘
               ▼
        tab5_platform
               │
    ┌──────────┼──────────┐
    │          │          │
    ▼          ▼          ▼
Display     I²C Buses   Platform Control
               │
               ▼
          Tab5 Hardware
```

All higher-level runtime components access the underlying hardware through services provided by this component.

This architecture prevents hardware-specific code from spreading throughout the application.

---

# Responsibilities

The `tab5_platform` component owns all board-level initialization required by the Ambient Runtime.

---

## Platform Initialization

Initialize every hardware resource required before the application begins normal execution.

Initialization is performed once during system startup through:

```c
esp_err_t tab5_platform_init(void);
```

Repeated initialization requests safely return without reinitializing the platform. 

---

## Internal I²C Infrastructure

Create and configure the internal I²C master bus used by onboard peripherals.

The current implementation configures:

- I²C Port 0;
- GPIO31 (SDA);
- GPIO32 (SCL);
- internal pull-up resistors.

This bus is primarily used by the onboard platform devices, including the PI4IOE expanders. :contentReference[oaicite:2]{index=2}

---

## External PORT A I²C

Create an independent I²C master bus dedicated to the external PORT A connector.

Current configuration:

- I²C Port 1;
- GPIO53 (SDA);
- GPIO54 (SCL).

This bus is exposed through the public API for use by external peripherals connected to the Ambient Runtime platform. 

---

## Platform Control Devices

Configure the two onboard PI4IOE I/O expanders responsible for platform control.

Current responsibilities include:

- hardware reset sequencing;
- GPIO direction configuration;
- pull-up configuration;
- output state configuration;
- peripheral enable signals.

These devices provide low-level control over several onboard resources required during platform initialization. :contentReference[oaicite:4]{index=4}

---

## Display Platform Services

Prepare all hardware resources required by the integrated LCD.

Responsibilities include:

- display reset;
- touch reset;
- backlight initialization;
- MIPI DSI infrastructure;
- LCD panel creation.

Rendering itself is delegated to higher-level display components.

---

## Hardware Service Provider

Expose reusable platform services required by other runtime modules.

Current services include:

- internal I²C access;
- PORT A I²C access;
- LCD drawing operations;
- display clearing;
- backlight control.

Higher-level modules remain independent from board-specific initialization.

---

# Platform Architecture

The component occupies the lowest software layer of the Ambient Runtime.

```text
Application
        │
        ▼
Runtime Components
        │
        ▼
tab5_platform
        │
 ┌──────┼───────────────┐
 │      │               │
 ▼      ▼               ▼
I²C   Display      Platform Control
 │      │               │
 ▼      ▼               ▼
Tab5 Hardware Resources
```

This separation isolates hardware-specific implementation from application logic while providing a consistent programming interface for the remainder of the runtime.

---

# Hardware Resources

The current implementation manages the following hardware resources.

| Resource | Purpose |
|----------|---------|
| Internal I²C Bus | Communication with onboard platform devices. |
| PORT A I²C Bus | Communication with external I²C peripherals. |
| PI4IOE1 | LCD, touch and platform control signals. |
| PI4IOE2 | ESP32-C6 power control and platform signals. |
| LCD Backlight | PWM brightness control. |
| MIPI DSI PHY | Display physical interface. |
| MIPI DSI Bus | LCD communication channel. |
| ST7121 Panel | Integrated LCD controller. |

The component owns the initialization and lifetime of these hardware resources. :contentReference[oaicite:5]{index=5}

---

# Public API

The component exposes the following platform services.

## Platform Initialization

```c
esp_err_t tab5_platform_init(void);
```

Initializes the complete Tab5 hardware platform.

---

## Backlight Control

```c
esp_err_t tab5_platform_backlight_set(
    int brightness_percent
);
```

Adjusts LCD brightness using PWM.

---

## Display Fill

```c
esp_err_t tab5_platform_fill(
    uint16_t color
);
```

Fills the entire LCD with a single RGB565 color.

---

## Bitmap Rendering

```c
esp_err_t tab5_platform_draw_bitmap(
    int x,
    int y,
    int w,
    int h,
    const uint16_t *pixels
);
```

Transfers an RGB565 bitmap to the integrated LCD.

---

## I²C Access

```c
i2c_master_bus_handle_t
tab5_platform_get_i2c_bus(void);
```

Returns the internal I²C bus.

---

```c
i2c_master_bus_handle_t
tab5_platform_get_port_a_i2c_bus(void);
```

Returns the external PORT A I²C bus. 

---

# Dependencies

The component depends on the following ESP-IDF modules:

- `driver`;
- `esp_lcd`;
- `esp_driver_gpio`;
- `esp_driver_ledc`;
- `esp_lcd_st7121`;
- `esp_hw_support`;
- `heap`.

Its ESP-IDF component registration is:

```cmake
idf_component_register(
    SRCS "tab5_platform.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        driver
        esp_lcd
        esp_driver_ledc
        esp_driver_gpio
        esp_lcd_st7121
        esp_hw_support
        heap
)
```

The component intentionally acts as the hardware abstraction layer for the Ambient Runtime, allowing higher-level modules to remain independent from board-specific implementation details.

---

# Platform Initialization

The `tab5_platform` component performs a deterministic initialization sequence that prepares every hardware resource required by the Ambient Runtime.

Initialization follows a strict order because several subsystems depend on previously initialized hardware resources.

```text
tab5_platform_init()
        │
        ▼
Internal I²C
        │
        ▼
PI4IOE2
        │
        ▼
ESP32-C6 Power
        │
        ▼
PORT A I²C
        │
        ▼
PI4IOE1
        │
        ▼
LCD / Touch Reset
        │
        ▼
Backlight
        │
        ▼
MIPI DSI
        │
        ▼
ST7121 Panel
        │
        ▼
Platform Ready
```

Each initialization step must complete successfully before the next one begins.

If any stage fails, the initialization process stops immediately and returns the corresponding ESP-IDF error code. 

---

# Internal I²C Bus

The first hardware resource initialized is the internal I²C master bus.

Current configuration:

| Parameter | Value |
|-----------|------:|
| Port | I²C0 |
| SDA | GPIO31 |
| SCL | GPIO32 |
| Internal Pull-up | Enabled |

This bus provides communication with the onboard platform devices, particularly the PI4IOE expanders responsible for low-level hardware control. :contentReference[oaicite:1]{index=1}

---

# PI4IOE2 Initialization

After the internal I²C bus becomes available, the platform initializes the second PI4IOE I/O expander.

This device is responsible for controlling several board-level signals associated with the wireless subsystem.

Initialization includes:

- chip reset;
- I/O direction configuration;
- high-impedance configuration;
- pull-up configuration;
- interrupt mask configuration;
- output-state initialization;
- output verification.

The component validates the configured output state before continuing the platform startup sequence. :contentReference[oaicite:2]{index=2}

---

# ESP32-C6 Power Enable

One of the responsibilities of the PI4IOE2 initialization sequence is enabling the onboard ESP32-C6 wireless coprocessor.

The platform asserts the `WLAN_PWR_EN` control signal and verifies that the expected output state has been applied.

After power is enabled, the implementation waits briefly to allow the wireless subsystem to stabilize before continuing initialization.

```text
PI4IOE2
        │
        ▼
WLAN_PWR_EN
        │
        ▼
ESP32-C6 Power Enabled
        │
        ▼
Stabilization Delay
```

This sequence ensures that the wireless subsystem is powered before higher-level networking services begin initialization. :contentReference[oaicite:3]{index=3}

---

# PORT A I²C Bus

The platform creates a second independent I²C master bus dedicated to the external PORT A connector.

Current configuration:

| Parameter | Value |
|-----------|------:|
| Port | I²C1 |
| SDA | GPIO53 |
| SCL | GPIO54 |
| Internal Pull-up | Enabled |

This bus is intended for external peripherals used by the Ambient Runtime, such as the PaHub and its downstream I²C devices.

Separating the external bus from the internal platform bus prevents external peripherals from interfering with board-control devices. 

---

# PI4IOE1 Initialization

The platform next initializes the first PI4IOE I/O expander.

This device manages several board-control signals associated with the display and touch subsystem.

Initialization includes:

- chip reset;
- GPIO direction configuration;
- pull-up configuration;
- high-impedance configuration;
- default output configuration.

Among the configured output signals are:

- speaker enable;
- external 5 V enable;
- LCD reset;
- touch reset;
- camera reset.

The implementation applies the validated default output state before continuing platform initialization. :contentReference[oaicite:5]{index=5}

---

# LCD and Touch Reset

After both PI4IOE devices have been configured, the platform performs the LCD and touch reset sequence.

The implementation temporarily drives the LCD and touch reset signals low before restoring them to their operational state.

```text
PI4IOE1
        │
        ▼
LCD_RST = LOW
TP_RST  = LOW
        │
        ▼
Delay
        │
        ▼
LCD_RST = HIGH
TP_RST  = HIGH
        │
        ▼
Display Ready
```

The reset timing is handled entirely within the platform component.

Higher-level runtime modules never manipulate LCD reset signals directly. :contentReference[oaicite:6]{index=6}

---

# Initialization Characteristics

The platform initialization sequence follows several important engineering principles.

## Deterministic Startup

Hardware resources are initialized in a fixed order.

The sequence does not depend on application state or runtime conditions.

---

## Single Initialization

Platform initialization is performed only once.

Subsequent calls to:

```c
tab5_platform_init()
```

return immediately after confirming that the platform has already been initialized.

---

## Encapsulated Hardware Control

GPIO manipulation, I²C transactions, reset timing and board-control registers remain completely internal to the component.

Application modules never interact directly with:

- PI4IOE registers;
- WLAN power signals;
- LCD reset timing;
- touch reset timing.

---

## Early Failure Detection

Each initialization stage validates its result before allowing the startup sequence to continue.

This approach prevents partially initialized hardware from being exposed to higher-level runtime components.

---

# Initialization Outcome

After successful completion of this phase, the following hardware resources are available:

- internal I²C bus;
- external PORT A I²C bus;
- PI4IOE1 controller;
- PI4IOE2 controller;
- powered ESP32-C6 wireless subsystem;
- initialized board-control infrastructure;
- LCD and touch subsystem ready for display initialization.

The remaining display-specific initialization is described in the next section.

---

# Display Subsystem

The `tab5_platform` component is responsible for initializing and managing the complete display infrastructure used by the Ambient Runtime.

Its responsibilities extend from LCD backlight control to the creation of the MIPI-DSI communication channel and the initialization of the ST7121 display controller.

Rendering itself is performed by higher-level components after the platform has completed its initialization.

---

# LCD Backlight

The platform configures the LCD backlight using the ESP-IDF LEDC peripheral.

Current configuration includes:

| Parameter | Value |
|-----------|------:|
| GPIO | 22 |
| PWM Peripheral | LEDC |
| Timer Resolution | 12 bits |
| Frequency | 5 kHz |

Brightness is controlled through:

```c
esp_err_t tab5_platform_backlight_set(
    int brightness_percent
);
```

The API accepts values between:

```text
0% ... 100%
```

Internally, the percentage is converted into the corresponding PWM duty cycle.

The platform automatically clamps values outside the valid range before updating the PWM output. 

---

# MIPI DSI PHY

Before creating the display bus, the platform acquires the MIPI DSI physical layer power domain.

The implementation requests an LDO channel dedicated to the DSI PHY.

```text
Platform
        │
        ▼
Acquire LDO
        │
        ▼
Enable DSI PHY
```

Without this step, the display interface cannot operate correctly.

This hardware-specific operation remains fully encapsulated within the platform layer. :contentReference[oaicite:1]{index=1}

---

# MIPI DSI Bus

Once the PHY becomes available, the platform creates the MIPI DSI communication bus.

Current configuration includes:

| Parameter | Value |
|-----------|------:|
| Bus ID | 0 |
| Data Lanes | 2 |
| Bit Rate | 965 Mbps |
| PHY Clock | Default ESP-IDF |

The resulting bus becomes the communication channel used by the LCD panel driver. :contentReference[oaicite:2]{index=2}

---

# DBI Interface

After the DSI bus is available, the platform creates the Display Bus Interface (DBI).

Current configuration includes:

- virtual channel 0;
- 8-bit LCD commands;
- 8-bit LCD parameters.

The DBI interface provides the command path used during panel initialization and runtime control.

It is created once during platform initialization and remains owned by the platform layer. :contentReference[oaicite:3]{index=3}

---

# DPI Configuration

The platform prepares the video configuration required by the LCD panel.

Current validated configuration includes:

| Parameter | Value |
|-----------|------:|
| Resolution | 720 × 1280 |
| Pixel Format | RGB565 |
| Framebuffers | 1 |
| DMA2D | Enabled |

Timing parameters such as horizontal and vertical synchronization are configured during platform initialization before the panel is created.

These parameters are intentionally hidden from higher-level runtime modules. :contentReference[oaicite:4]{index=4}

---

# ST7121 Panel

The platform creates the integrated LCD panel through the dedicated ST7121 driver.

```text
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

The platform prepares the required vendor configuration structure before requesting panel creation.

After creation, the initialization sequence performs:

1. panel reset;
2. panel initialization;
3. display enable.

Only after these operations does the platform consider the display subsystem ready. 

---

# Framebuffer Management

The platform provides helper functions for transferring image data to the integrated LCD.

Two drawing services are currently available.

## Full-Screen Fill

```c
esp_err_t tab5_platform_fill(
    uint16_t color
);
```

This function:

- allocates a temporary framebuffer in PSRAM;
- fills every pixel with the specified RGB565 color;
- transfers the framebuffer to the LCD;
- releases the allocated memory.

This operation is primarily intended for platform validation and display testing. :contentReference[oaicite:6]{index=6}

---

## Bitmap Rendering

```c
esp_err_t tab5_platform_draw_bitmap(
    int x,
    int y,
    int w,
    int h,
    const uint16_t *pixels
);
```

This function transfers an application-provided RGB565 bitmap directly to the display.

The caller owns the framebuffer.

The platform validates:

- platform initialization;
- panel availability;
- bitmap pointer;
- image dimensions.

After validation, the request is forwarded to the ESP-IDF LCD framework. :contentReference[oaicite:7]{index=7}

---

# Display Services

The platform provides a minimal set of display-oriented services.

Current capabilities include:

- LCD initialization;
- backlight brightness control;
- full-screen color fill;
- partial bitmap rendering.

Higher-level rendering responsibilities remain outside this component.

The platform intentionally does not implement:

- graphics primitives;
- text rendering;
- fonts;
- widgets;
- user interface layouts;
- animation.

These responsibilities belong to higher layers such as `ambient_console` and `oled_context_presenter`.

---

# Memory Usage

Display operations make use of external PSRAM for framebuffer allocation.

Before creating the LCD panel, the platform reports available PSRAM capacity through diagnostic logging.

Using PSRAM allows large display buffers to be allocated without exhausting the internal memory required by the remainder of the Ambient Runtime. :contentReference[oaicite:8]{index=8}

---

# Display Lifecycle

The display subsystem follows the lifecycle below.

```text
Acquire DSI PHY
        │
        ▼
Create DSI Bus
        │
        ▼
Create DBI Interface
        │
        ▼
Configure DPI
        │
        ▼
Create ST7121 Panel
        │
        ▼
Reset Panel
        │
        ▼
Initialize Panel
        │
        ▼
Display ON
        │
        ▼
Runtime Rendering
```

After this sequence completes, higher-level runtime components may safely render graphical content through the services provided by the platform layer.

---

# Engineering Decisions

The `tab5_platform` component preserves several validated engineering decisions that are specific to the M5Stack Tab5 hardware architecture.

These decisions are intentionally centralized inside the platform layer so that the remainder of the Ambient Runtime can remain independent from board-level implementation details.

---

# Platform Ownership

The component owns the initialization and lifetime of the following resources:

- internal I²C bus;
- external PORT A I²C bus;
- PI4IOE1 handle;
- PI4IOE2 handle;
- MIPI DSI PHY power channel;
- MIPI DSI bus;
- DBI panel interface;
- ST7121 panel handle;
- LCD backlight PWM;
- platform initialization state.

These resources remain private to the component.

Higher-level runtime modules receive only the public service interfaces required for their operation. 

---

# WLAN Power Enable

A critical engineering requirement of the Tab5 platform is the explicit activation of the onboard ESP32-C6 wireless coprocessor power rail.

The enable signal is controlled through:

```text
PI4IOE2
```

at I²C address:

```text
0x44
```

The relevant output is:

```text
P0 = WLAN_PWR_EN
```

The validated initialization sequence writes:

```text
0x09
```

to the PI4IOE2 output register, enabling both:

- `WLAN_PWR_EN`;
- `USB5V_EN`.

The platform then reads the output register back and verifies that bit 0 is active before allowing initialization to continue. :contentReference[oaicite:1]{index=1}

---

# Wireless Subsystem Sequence

The validated wireless power sequence is:

```text
Internal I²C
        │
        ▼
PI4IOE2 Initialization
        │
        ▼
WLAN_PWR_EN = 1
        │
        ▼
Read-Back Verification
        │
        ▼
ESP32-C6 Powered
        │
        ▼
Stabilization Delay
        │
        ▼
Network Layer Initialization
```

The platform waits approximately:

```text
200 ms
```

after enabling the wireless power rail before proceeding.

This delay allows the ESP32-C6 subsystem to stabilize before the Ambient Runtime initializes higher-level networking services. :contentReference[oaicite:2]{index=2}

---

# Engineering Note Recommendation

The complete investigation that identified the `WLAN_PWR_EN` requirement should be maintained as a separate Engineering Technical Note.

Recommended location:

```text
docs/notes/NOTE_001_TAB5_WIFI_POWER_ENABLE.md
```

The component README should preserve only the permanent engineering decision and reference the detailed investigation.

---

# Internal and External I²C Separation

The Tab5 platform uses two independent I²C buses.

```text
Internal I²C Bus
        │
        ├── PI4IOE1
        └── PI4IOE2

External PORT A I²C Bus
        │
        └── PaHub
                ├── ENV-IV
                ├── Mini OLED
                └── DLight
```

This separation is intentional.

The internal bus is reserved for board-control devices.

The PORT A bus is dedicated to external Ambient Runtime peripherals.

This arrangement reduces coupling between platform-control traffic and external device communication. 

---

# Display Abstraction

The platform component exposes framebuffer-oriented display services rather than controller-specific operations.

Higher-level components do not interact directly with:

- MIPI DSI configuration;
- DBI commands;
- ST7121 initialization;
- panel reset operations;
- synchronization timings;
- LDO configuration.

Instead, they use:

```c
tab5_platform_draw_bitmap(...)
```

or:

```c
tab5_platform_fill(...)
```

This preserves a stable boundary between rendering logic and display hardware. 

---

# Backlight Policy

The platform initializes the backlight with brightness set to:

```text
0%
```

The application enables the required brightness only after the platform and console initialization complete.

This prevents partially initialized display contents from being presented during startup. :contentReference[oaicite:5]{index=5}

---

# Input Validation

Public display services validate their arguments before interacting with the panel.

Current checks include:

- platform initialized;
- panel handle available;
- non-null bitmap pointer;
- positive bitmap width;
- positive bitmap height.

The backlight service also clamps the requested brightness to the valid range:

```text
0% to 100%
```

These checks prevent invalid application requests from reaching lower-level hardware operations. :contentReference[oaicite:6]{index=6}

---

# Error Handling

The platform initialization sequence uses ESP-IDF error propagation macros to stop immediately when a required hardware operation fails.

Typical behavior:

```text
Initialize Resource
        │
   ┌────┴────┐
   │         │
Success     Failure
   │         │
   ▼         ▼
Continue   Return Error
```

The component does not continue into later initialization stages after a critical hardware failure.

This prevents the application from treating a partially initialized platform as operational. :contentReference[oaicite:7]{index=7}

---

# Initialization Idempotency

The platform maintains an internal initialization flag.

After successful initialization:

```c
platform_initialized = true;
```

Subsequent calls to:

```c
tab5_platform_init()
```

return successfully without recreating hardware resources.

This prevents duplicated buses, panel handles or PWM configuration. :contentReference[oaicite:8]{index=8}

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| Internal I²C initialization | Validated |
| PI4IOE1 initialization | Validated |
| PI4IOE2 initialization | Validated |
| ESP32-C6 power enable | Validated |
| WLAN power-state verification | Validated |
| PORT A I²C initialization | Validated |
| LCD reset | Validated |
| Touch reset | Validated |
| Backlight PWM initialization | Validated |
| Backlight brightness control | Validated |
| MIPI DSI PHY power | Validated |
| MIPI DSI bus creation | Validated |
| DBI interface creation | Validated |
| ST7121 panel creation | Validated |
| Full-screen fill | Validated |
| Bitmap rendering | Validated |
| Ambient Runtime integration | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Limitations

The `tab5_platform` component intentionally focuses on the hardware resources currently required by the Ambient Runtime.

It does not provide a complete Tab5 Board Support Package.

The current implementation does not expose:

- touch input events;
- audio services;
- camera services;
- speaker control;
- battery monitoring;
- USB host services;
- SD card services;
- power-management policies;
- display rotation policies;
- panel deinitialization;
- platform shutdown;
- dynamic hardware discovery.

These capabilities remain outside the current validated scope.

---

# Resource Lifetime

The platform does not currently implement resource deinitialization.

After successful initialization, the following resources remain active for the complete runtime lifetime:

- I²C buses;
- PI4IOE device handles;
- MIPI DSI resources;
- panel handle;
- backlight PWM;
- DSI PHY LDO channel.

This design is appropriate for the current continuously running embedded application.

A future runtime requiring controlled shutdown or restart would need explicit deinitialization support.

---

# Memory Considerations

The integrated display requires large RGB565 framebuffers.

For a complete `720 × 1280` frame:

```text
720 × 1280 × 2 bytes
```

which corresponds to:

```text
1,843,200 bytes
```

Framebuffer allocation therefore depends on external PSRAM.

The platform reports:

- available PSRAM;
- largest available PSRAM block;

during display initialization to support engineering validation. :contentReference[oaicite:9]{index=9}

---

# Temporary Frame Allocation

The function:

```c
tab5_platform_fill(...)
```

allocates a complete temporary framebuffer, transfers it and releases it.

This behavior is acceptable for diagnostic and platform validation use.

For continuous runtime rendering, higher-level components should reuse their own persistent framebuffer whenever possible.

The `ambient_console` follows this approach by maintaining its framebuffer in PSRAM and passing it to:

```c
tab5_platform_draw_bitmap(...)
```

---

# Build

The component is built automatically as part of the Ambient Runtime.

Typical build sequence:

```bash
cd firmware/nodes/ambient-runtime-node

idf.py build

idf.py flash monitor
```

The project must target:

```text
esp32p4
```

A clean rebuild may be performed when platform configuration, panel drivers or low-level hardware components change:

```bash
idf.py fullclean
idf.py build
idf.py flash monitor
```

---

# Validation

After flashing, verify the following startup sequence through the serial monitor:

```text
Tab5 platform init based on H020 baseline
init internal I2C bus
init PI4IOE2
ESP32-C6 Wi-Fi power enabled
init PORT A I2C bus
init PI4IOE1
reset LCD_RST/TP_RST
init LCD backlight PWM
Acquire MIPI DSI PHY LDO
Create MIPI DSI bus
Create DBI panel IO
Configure ST7121 DPI panel
Create ST7121 panel
Tab5 platform init OK
```

The exact log formatting may vary, but all major stages must complete without an error result. :contentReference[oaicite:10]{index=10}

---

# Related Components

The Tab5 Platform supports the following Ambient Runtime components.

| Component | Relationship |
|-----------|--------------|
| `ambient_console` | Uses platform bitmap transfer services to render the primary Runtime Console. |
| `ambient_network` | Depends on the powered ESP32-C6 wireless subsystem. |
| `esp_lcd_st7121` | Provides the ST7121 panel implementation used by the platform. |
| `pahub` | Uses the PORT A I²C bus exposed by the platform. |
| `env_iv` | Uses the external I²C infrastructure through the PaHub. |
| `dlight` | Uses the external I²C infrastructure through the PaHub. |
| `oled_sh1107` | Uses the external I²C infrastructure through the PaHub. |
| `oled_context_presenter` | Indirectly depends on platform-provided external I²C access. |
| `semantic_event_receiver` | Runs on the platform but has no direct hardware dependency on this component. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/main/README.md` | Application lifecycle and platform initialization order. |
| `firmware/nodes/ambient-runtime-node/components/esp_lcd_st7121/README.md` | ST7121 panel driver. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md` | Primary Runtime Console. |
| `firmware/nodes/ambient-runtime-node/components/pahub/README.md` | External I²C multiplexer. |
| `docs/notes/NOTE_001_TAB5_WIFI_POWER_ENABLE.md` | Recommended technical note documenting the ESP32-C6 power-enable investigation. |

---

# Conclusion

The `tab5_platform` component provides the hardware foundation of the Ambient Runtime.

It encapsulates board-specific initialization, power control, communication buses and display infrastructure behind a compact platform API.

By centralizing knowledge of the M5Stack Tab5 hardware architecture, the component allows the remainder of the Ambient Runtime to operate without direct dependencies on:

- PI4IOE registers;
- ESP32-C6 power sequencing;
- MIPI DSI configuration;
- LCD controller initialization;
- board-specific GPIO assignments.

This separation establishes a stable hardware abstraction layer for the current Ambient Runtime baseline and provides a maintainable foundation for future platform evolution.