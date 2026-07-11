# Ambient Runtime Node

## Ambient Physical AI

The **Ambient Runtime Node** is the physical environment execution node of the **Ambient Physical AI** architecture.

This implementation runs on the **M5Stack Tab5**, based on the **ESP32-P4**, and is responsible for:

- acquiring environmental data;
- monitoring the local I²C device network;
- presenting the official Ambient Runtime Console;
- controlling auxiliary displays;
- representing the operational state of the physical environment;
- serving as the future network endpoint for integration with the remaining Ambient Physical AI nodes.

The current implementation replaces the original PoE-P4 host while preserving the architectural responsibility of the Ambient Runtime.

---

# Current Milestone

The current milestone establishes a stable local Ambient Runtime baseline on the Tab5.

Validated capabilities include:

- ESP32-P4 boot;
- 32 MB PSRAM;
- ST7121 display;
- MIPI DSI;
- framebuffer rendering;
- LCD backlight control;
- internal I²C bus;
- PORT A I²C bus;
- PI4IOE1 initialization;
- PaHub channel selection;
- ENV-IV environmental sensing;
- DLight ambient light sensing;
- Mini OLED SH1107;
- periodic sensor acquisition;
- text-oriented Ambient Runtime Console;
- runtime health visualization.

Wi-Fi integration remains intentionally documented as pending.

---

# Architectural Role

The Ambient Runtime Node is not the Cognitive Runtime.

Its responsibility is to interact with and represent the physical environment.

```text
Presence Layer
        │
        ▼
Identity Layer
        │
        ▼
Cognitive Runtime
        │
        ▼
Ambient Runtime
        │
        ▼
Physical Environment
```

The Ambient Runtime receives future semantic decisions and converts them into physical or visual environmental changes.

In the current milestone, the node operates in local standalone mode.

---

# Hardware Architecture

```text
M5Stack Tab5
│
├── ESP32-P4
│   ├── Ambient Runtime application
│   ├── Ambient Runtime Console
│   ├── MIPI DSI display
│   ├── PSRAM framebuffer
│   └── PORT A I²C controller
│
├── Internal I²C
│   ├── PI4IOE1
│   ├── LCD control
│   └── Touch controller
│
└── PORT A I²C
    └── PaHub
        ├── Channel 0 → ENV-IV
        ├── Channel 1 → Mini OLED SH1107
        ├── Channel 2 → DLight
        └── Channels 3–5 → Available for future devices
```

---

# Hardware Inventory

| Device | Function | Interface | Status |
|---|---|---|---|
| M5Stack Tab5 | Main Ambient Runtime platform | ESP32-P4 | Validated |
| ST7121 display | Primary Runtime Console | MIPI DSI | Validated |
| PI4IOE1 | Internal GPIO expander | Internal I²C | Validated |
| PaHub | I²C multiplexer | PORT A I²C | Validated |
| ENV-IV | Temperature, humidity and pressure | PaHub CH0 | Validated |
| Mini OLED SH1107 | Auxiliary distributed display | PaHub CH1 | Validated |
| DLight | Ambient light measurement | PaHub CH2 | Validated |
| ESP32-C6 | Wi-Fi coprocessor | SDIO | Pending integration |

---

# Current I²C Topology

```text
ESP32-P4
    │
    ▼
PORT A I²C
SDA = GPIO53
SCL = GPIO54
    │
    ▼
PaHub
Address = 0x70
    │
    ├── CH0
    │   └── ENV-IV
    │       ├── SHT40
    │       └── BMP280
    │
    ├── CH1
    │   └── Mini OLED SH1107
    │
    └── CH2
        └── DLight
```

---

# PaHub Channel Allocation

| Channel | Device | Responsibility |
|---|---|---|
| CH0 | ENV-IV | Temperature, humidity and pressure |
| CH1 | Mini OLED | Auxiliary local status display |
| CH2 | DLight | Ambient light measurement |
| CH3 | Reserved | Future I²C device |
| CH4 | Reserved | Future I²C device |
| CH5 | Reserved | Future I²C device |

Channel allocation is centralized in `main/main.cpp`.

```cpp
static constexpr uint8_t PAHUB_CHANNEL_ENV_IV = 0;
static constexpr uint8_t PAHUB_CHANNEL_OLED   = 1;
static constexpr uint8_t PAHUB_CHANNEL_DLIGHT = 2;
```

---

# Ambient Runtime Console

The Tab5 display is the official:

```text
Ambient Runtime Console
```

The console is intentionally implemented as a clean engineering interface.

It is not:

- a graphical dashboard;
- an LVGL demonstration;
- an animated user interface;
- a general-purpose tablet application.

The console prioritizes:

- readability;
- system status;
- engineering clarity;
- demonstration reliability;
- direct visualization of real hardware data.

---

# Console Layout

```text
AMBIENT PHYSICAL AI
AMBIENT RUNTIME CONSOLE

SYSTEM
WI-FI ............ PENDING
COGNITIVE ......... WAITING
MODE .............. STANDALONE

ENVIRONMENT
TEMPERATURE ....... xx.x C
HUMIDITY .......... xx.x %
PRESSURE .......... xxxx.x HPA
LIGHT ............. xxxx.x LX

I2C NETWORK
PAHUB ............. OK
ENV-IV ............ OK
DLIGHT ............ OK
MINI OLED ......... OK

STATUS
LOCAL READY
WI-FI INTEGRATION PENDING
```

`LOCAL READY` means that the local hardware, sensors, display and I²C network are operational.

`WI-FI INTEGRATION PENDING` means that network integration is not yet part of the validated baseline.

---

# Runtime Data Flow

```text
ENV-IV
    │
    ├── Temperature
    ├── Humidity
    └── Pressure
            │
            ▼
DLight
    │
    └── Ambient light
            │
            ▼
Ambient Runtime State
            │
            ├── Hardware health
            ├── Sensor values
            ├── I²C state
            └── Network state
                    │
                    ▼
Ambient Runtime Console
```

---

# Runtime Loop

```text
Boot
 │
 ▼
Initialize Tab5 platform
 │
 ▼
Initialize Ambient Console
 │
 ▼
Initialize Mini OLED
 │
 ▼
Runtime loop
 │
 ├── Select ENV-IV channel
 ├── Read SHT40
 ├── Read BMP280
 ├── Select DLight channel
 ├── Read lux
 ├── Revalidate Mini OLED channel
 ├── Build console state
 ├── Render console
 ├── Log system health
 └── Wait 3 seconds
```

Current update interval:

```cpp
static constexpr TickType_t RUNTIME_UPDATE_INTERVAL =
    pdMS_TO_TICKS(3000);
```

---

# Sensor Responsibilities

## ENV-IV

### SHT40

Provides:

- temperature;
- relative humidity.

Example:

```text
SHT40 Temp: 29.76 C  Hum: 73.04 %
```

### BMP280

Provides:

- atmospheric pressure;
- secondary temperature measurement.

Example:

```text
BMP280 Temp: 30.25 C  Press: 1013.24 hPa
```

The console currently uses the SHT40 value as its primary environmental temperature.

## DLight

The DLight unit provides ambient light measurement in lux.

```text
DLight Lux: 67.50 lx
```

## Mini OLED SH1107

The Mini OLED is an auxiliary distributed display, not the primary console.

Current baseline:

- initialization validated;
- communication through PaHub validated;
- test content validated.

Future content may include:

```text
Ambient Runtime

LOCAL READY

29.8 C

412 lx
```

The final dynamic OLED content remains a non-blocking improvement.

---

# Project Structure

```text
ambient-runtime-node/
│
├── CMakeLists.txt
├── README.md
├── sdkconfig.defaults
│
├── main/
│   ├── CMakeLists.txt
│   └── main.cpp
│
└── components/
    ├── README.md
    ├── ambient_console/
    ├── dlight/
    ├── env_iv/
    ├── esp_lcd_st7121/
    ├── oled_sh1107/
    ├── pahub/
    └── tab5_platform/
```

---

# Component Responsibilities

| Component | Responsibility |
|---|---|
| `tab5_platform` | Tab5 initialization, display, backlight and I²C buses |
| `esp_lcd_st7121` | ST7121 panel driver |
| `ambient_console` | Framebuffer, font rendering and Runtime Console |
| `pahub` | I²C multiplexer channel selection |
| `env_iv` | SHT40 and BMP280 measurements |
| `dlight` | Ambient light measurement |
| `oled_sh1107` | Mini OLED initialization and rendering |

Detailed component documentation is available in `components/README.md`.

---

# Platform Initialization

The `tab5_platform` component isolates board-specific code.

Responsibilities:

- internal I²C initialization;
- PORT A I²C initialization;
- PI4IOE1 initialization;
- LCD and touch reset;
- MIPI DSI PHY LDO configuration;
- MIPI DSI bus creation;
- DBI panel I/O creation;
- ST7121 panel creation;
- framebuffer drawing;
- LCD backlight control.

---

# Framebuffer

Resolution:

```text
720 × 1280
```

Pixel format:

```text
RGB565
```

Memory requirement:

```text
720 × 1280 × 2 bytes
= 1,843,200 bytes
```

The framebuffer is allocated in PSRAM:

```cpp
heap_caps_malloc(
    frame_size,
    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
);
```

---

# Text Rendering

The console uses a lightweight 8×8 bitmap font.

The current implementation provides:

- pixel rendering;
- character rendering;
- text rendering;
- configurable integer scaling;
- horizontal separators;
- formatted numeric values.

This avoids introducing LVGL into the current baseline and reduces dependency complexity and competition risk.

---

# Build Requirements

Validated environment:

```text
ESP-IDF 5.4.2
Target: ESP32-P4
Flash: 16 MB
PSRAM: 32 MB
```

---

# Build

```cmd
cd C:\Users\profc\projetos\ambient-physical-ai\firmware\nodes\ambient-runtime-node
idf.py set-target esp32p4
idf.py reconfigure
idf.py build
```

---

# Flash

```cmd
idf.py flash monitor
```

Exit the monitor with:

```text
Ctrl + ]
```

---

# Expected Boot Sequence

```text
Ambient Physical AI - Ambient Runtime Console
Tab5 platform init based on H020 baseline
init internal I2C bus SDA=31 SCL=32
init PORT A I2C bus SDA=53 SCL=54
init PI4IOE1 at I2C address 0x43
Acquire MIPI DSI PHY LDO
Create MIPI DSI bus
Create DBI panel IO
Configure ST7121 DPI panel
Create ST7121 panel
Tab5 platform init OK
Ambient Runtime Console init
Console framebuffer allocated: 1843200 bytes
Mini OLED initialized
SHT40 Temp: ...
BMP280 Temp: ...
DLight Lux: ...
Ambient Runtime alive
```

---

# Expected Runtime Behavior

After boot:

1. the Tab5 display becomes active;
2. the Ambient Runtime Console is rendered;
3. temperature, humidity and pressure are read;
4. ambient light is read;
5. PaHub devices are checked;
6. the console is updated every three seconds;
7. the serial log reports the current hardware state.

Example:

```text
Ambient Runtime alive |
Platform=OK |
I2C=OK |
ENV-IV=OK |
DLight=OK |
OLED=OK |
Wi-Fi=PENDING
```

---

# Definition of Local Ready

The console reports `LOCAL READY` when all local mandatory devices are operational:

```text
PaHub
+
ENV-IV
+
DLight
+
Mini OLED
```

```cpp
const bool hardware_ready =
    data->pahub_ok &&
    data->env_iv_ok &&
    data->dlight_ok &&
    data->mini_oled_ok;
```

---

# Wi-Fi Status

Wi-Fi integration is currently:

```text
PENDING
```

The Tab5 uses:

```text
ESP32-P4
    │
    ▼
ESP-Hosted
    │
    ▼
SDIO
    │
    ▼
ESP32-C6
    │
    ▼
Wi-Fi network
```

A dedicated integration laboratory used:

- `espressif/esp_hosted` 1.4.0;
- `espressif/esp_wifi_remote` 0.8.5;
- SDIO 4-bit at 40 MHz;
- GPIO8 through GPIO13;
- GPIO15 as ESP32-C6 reset.

The custom project reached:

```text
ESP-Hosted host initialized
        │
        ▼
Remote Wi-Fi channels registered
        │
        ▼
ESP32-C6 reset executed
        │
        ▼
SDIO initialization attempted
        │
        ▼
sdmmc_init_ocr timeout
```

Observed error:

```text
sdmmc_init_ocr:
send_op_cond returned 0x107
```

The official M5Tab5 UserDemo successfully created `M5Tab5-UserDemo-WiFi` on the same hardware, validating the ESP32-C6 hardware, firmware, physical SDIO connection and official ESP-Hosted stack.

The Wi-Fi laboratory was intentionally removed from the stable Runtime build.

---

# Wi-Fi Pending Mission

```text
TAB5_WIFI_REMOTE_PENDING_001
```

Objective:

```text
Connect the Ambient Runtime Node to the local Wi-Fi network
and obtain:

- IPv4 address
- network mask
- default gateway
- RSSI
```

Expected future console state:

```text
SYSTEM

WI-FI ............ CONNECTED
IP ............... 192.168.x.x
MASK ............. 255.255.255.0
GATEWAY .......... 192.168.x.1
RSSI ............. -xx DBM
COGNITIVE ........ WAITING
MODE ............. STANDALONE
```

---

# Known Limitations

| Item | Status |
|---|---|
| Wi-Fi | Pending |
| Cognitive Runtime connection | Waiting / not implemented |
| MQTT | Out of scope |
| Semantic Events | Out of scope |
| Dynamic Mini OLED content | Partially implemented |
| Touch interface | Not used |
| Advanced graphics | Out of scope |

---

# Engineering Decisions

## Tab5 replaces PoE-P4

Previous host:

```text
PoE-P4
```

Current host:

```text
Tab5
```

The architectural responsibility remains unchanged.

## Text-oriented console

Reasons:

- deterministic rendering;
- easier debugging;
- lower complexity;
- reduced memory overhead;
- better readability;
- lower competition risk.

## Component-based organization

Benefits:

- modularity;
- testability;
- reuse;
- easier maintenance;
- clearer documentation;
- easier future replacement of devices.

## Local readiness separated from network readiness

The console differentiates:

```text
LOCAL READY
```

from:

```text
WI-FI INTEGRATION PENDING
```

---

# Current Validation Matrix

| Capability | Status |
|---|---|
| ESP32-P4 boot | PASS |
| Flash 16 MB | PASS |
| PSRAM 32 MB | PASS |
| MIPI DSI | PASS |
| ST7121 display | PASS |
| RGB565 framebuffer | PASS |
| LCD backlight | PASS |
| Internal I²C | PASS |
| PORT A I²C | PASS |
| PI4IOE1 | PASS |
| PaHub | PASS |
| ENV-IV SHT40 | PASS |
| ENV-IV BMP280 | PASS |
| DLight | PASS |
| Mini OLED SH1107 | PASS |
| Periodic runtime loop | PASS |
| Real sensor data on console | PASS |
| Local hardware health | PASS |
| Wi-Fi Station | PENDING |
| IPv4 address | PENDING |
| Netmask | PENDING |
| Gateway | PENDING |
| Cognitive Runtime connection | NOT IMPLEMENTED |
| MQTT | OUT OF SCOPE |
| Semantic Events | OUT OF SCOPE |

---

# Recommended Development Workflow

```text
Implement
    │
    ▼
Build
    │
    ▼
Flash
    │
    ▼
Validate logs
    │
    ▼
Validate display
    │
    ▼
Document
    │
    ▼
Commit
```

---

# Repository Hygiene

Before committing:

```cmd
git status
```

The repository should not track:

```text
build/
managed_components/
sdkconfig
sdkconfig.old
.vscode/
__pycache__/
```

Expected versioned files:

```text
README.md
sdkconfig.defaults
main/
components/
```

---

# Suggested Commit Scope

```text
feat(ambient-runtime): complete Tab5 local console baseline
```

Suggested description:

```text
- finalize Tab5 Ambient Runtime Console
- add periodic ENV-IV sensor acquisition
- add periodic DLight lux acquisition
- validate PaHub channel topology
- keep Mini OLED as auxiliary display
- display local hardware health
- report LOCAL READY state
- document Wi-Fi integration as pending
- restore stable build without ESP-Hosted runtime dependency
- update Ambient Runtime documentation
```

---

# Final Current State

```text
Tab5
    │
    ├── Display ............. OK
    ├── PSRAM ............... OK
    ├── PORT A I²C .......... OK
    ├── PaHub ............... OK
    ├── ENV-IV .............. OK
    ├── DLight .............. OK
    ├── Mini OLED ........... OK
    ├── Runtime Console ..... OK
    ├── Periodic Updates .... OK
    └── Wi-Fi ............... PENDING
```

The validated baseline is suitable for:

- local demonstration;
- environmental sensing;
- engineering inspection;
- future system integration;
- continued development of the Ambient Physical AI architecture.

The next blocking integration task is the dedicated Tab5 Wi-Fi Station mission.

---

# Ambient Physical AI

```text
Validated Local Runtime
        │
        ▼
Wi-Fi Integration
        │
        ▼
Cognitive Runtime Connection
        │
        ▼
Semantic Coordination
        │
        ▼
Final Competition Demonstration
```
