# Ambient Runtime Application

## Ambient Physical AI

### Application Orchestration and Runtime Lifecycle

---

# Overview

The `main` directory contains the application entry point of the Ambient Runtime Node.

Its implementation is responsible for coordinating the initialization and periodic execution of the ESP-IDF components that form the Ambient Runtime.

The application entry point does not implement device drivers, network protocols, display-controller logic or semantic business rules.

Instead, it orchestrates the validated components responsible for those functions and integrates their outputs into the operational state of the embedded runtime.

The application is implemented in:

```text
firmware/nodes/ambient-runtime-node/main/main.cpp
```

The ESP-IDF entry point is:

```c
extern "C" void app_main(void);
```

---

# Purpose

The purpose of the `main` application is to coordinate the complete lifecycle of the Ambient Runtime.

Its current responsibilities include:

* initializing the M5Stack Tab5 platform;
* initializing the primary Runtime Console;
* enabling the LCD backlight after local display initialization;
* starting the network infrastructure;
* starting the Semantic Event Receiver;
* obtaining the external PORT A I²C bus;
* managing PaHub channel selection;
* initializing the Mini OLED;
* acquiring environmental sensor measurements;
* acquiring ambient-light measurements;
* retrieving network and semantic receiver status;
* rendering semantic context on the Mini OLED;
* assembling the normalized Runtime Console snapshot;
* rendering the Runtime Console;
* emitting periodic operational diagnostics.

The application acts as the integration boundary between independent runtime components.

---

# Position within the Ambient Runtime

The `main` application occupies the orchestration layer of the Ambient Runtime.

```text
                    Ambient Runtime Application

                              main.cpp
                                 │
          ┌──────────────────────┼──────────────────────┐
          │                      │                      │
          ▼                      ▼                      ▼
  Platform Services       Runtime Services        Device Services
          │                      │                      │
          ▼                      ▼                      ▼
   tab5_platform          ambient_network              pahub
                          semantic_event_receiver      env_iv
                          ambient_console              dlight
                          oled_context_presenter       oled_sh1107
```

The application determines when each component is initialized and how their public interfaces are combined during runtime execution.

It does not replace the responsibility of any component.

---

# Architectural Role

The `main.cpp` implementation serves as the **Application Orchestrator** of the Ambient Runtime.

Its role can be summarized as:

```text
Initialize
        │
        ▼
Coordinate
        │
        ▼
Acquire
        │
        ▼
Normalize
        │
        ▼
Present
        │
        ▼
Monitor
```

Each stage uses component APIs rather than accessing hardware or protocols directly.

---

# Application Responsibilities

## Component Initialization

The application initializes the components required by the validated runtime baseline.

The initialization sequence includes:

* `tab5_platform`;
* `ambient_console`;
* LCD backlight;
* `ambient_network`;
* `semantic_event_receiver`;
* `oled_sh1107`;
* initial `oled_context_presenter` rendering.

Initialization details remain inside the corresponding components.

---

## Runtime Coordination

The application coordinates the periodic use of the external peripherals connected through the PaHub.

Before accessing a downstream device, it selects the corresponding PaHub channel.

The current validated channel mapping is:

| PaHub channel | Device    |
| ------------: | --------- |
|           `0` | ENV-IV    |
|           `1` | Mini OLED |
|           `2` | DLight    |

Channel ownership remains in the application layer.

The individual drivers do not know that the devices are connected through an I²C multiplexer.

---

## Environmental Acquisition

The application coordinates the acquisition of:

* SHT40 temperature;
* SHT40 relative humidity;
* BMP280 temperature;
* BMP280 atmospheric pressure;
* DLight illumination.

The sensor components perform the actual device communication and physical-unit conversion.

The application only coordinates their execution and records whether each acquisition succeeded.

---

## Semantic Context Presentation

The application retrieves the latest normalized Ambient Context from the `semantic_event_receiver`.

It then forwards that snapshot to the `oled_context_presenter`.

```text
semantic_event_receiver
        │
        ▼
ambient_context_snapshot_t
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

The application does not select user profiles, external services, personalized content or relevance rules.

Those responsibilities remain in the Cognitive Runtime.

---

## Runtime Console Assembly

The application collects information from multiple components and consolidates it into:

```c
ambient_console_data_t
```

The resulting snapshot includes:

* environmental measurements;
* Wi-Fi association status;
* network readiness;
* IPv4 information;
* RSSI;
* latest Semantic Event information;
* PaHub status;
* ENV-IV status;
* DLight status;
* Mini OLED status.

The complete snapshot is then supplied to:

```c
ambient_console_render(&console);
```

The Console remains independent from the origin of each value.

---

## Operational Monitoring

At the end of every runtime cycle, the application emits a consolidated status log.

The diagnostic output reports:

* platform status;
* external I²C status;
* ENV-IV status;
* DLight status;
* Mini OLED status;
* Wi-Fi state;
* IPv4 address;
* network mask;
* gateway;
* RSSI;
* network readiness.

This log provides a periodic operational heartbeat for engineering validation.

---

# Responsibilities Excluded from `main`

The application intentionally does not implement the internal behavior of the runtime components.

It does not directly perform:

* Tab5 board initialization;
* PI4IOE register configuration;
* ESP32-C6 power sequencing;
* Wi-Fi event processing;
* UDP socket management;
* JSON parsing;
* context TTL calculation;
* sensor-register access;
* sensor compensation algorithms;
* SH1107 command transmission;
* ST7121 panel initialization;
* framebuffer text rendering;
* profile selection;
* semantic reasoning;
* personalized-content generation.

These responsibilities remain encapsulated within dedicated components or the Cognitive Runtime.

---

# Application Component Dependencies

The application declares private dependencies on the following ESP-IDF components:

```text
tab5_platform
pahub
env_iv
oled_sh1107
oled_context_presenter
dlight
ambient_console
ambient_network
semantic_event_receiver
```

The corresponding ESP-IDF registration is:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        tab5_platform
        pahub
        env_iv
        oled_sh1107
        oled_context_presenter
        dlight
        ambient_console
        ambient_network
        semantic_event_receiver
)
```

Using `PRIV_REQUIRES` indicates that these dependencies belong to the internal implementation of the application component and are not exported as a public interface.

---

# Application Structure

The `main` directory contains the application-level ESP-IDF files.

```text
main/
│
├── README.md
├── main.cpp
├── CMakeLists.txt
└── idf_component.yml
```

| File                | Responsibility                                                       |
| ------------------- | -------------------------------------------------------------------- |
| `main.cpp`          | Application entry point, initialization sequence and runtime loop.   |
| `CMakeLists.txt`    | ESP-IDF application-component registration and private dependencies. |
| `idf_component.yml` | ESP-IDF Component Manager metadata and managed dependencies.         |
| `README.md`         | Application orchestration and lifecycle documentation.               |

---

# High-Level Application Flow

The application lifecycle is organized into two major phases.

```text
Application Start
        │
        ▼
Initialization Phase
        │
        ▼
Continuous Runtime Phase
```

The initialization phase prepares all mandatory platform and runtime services.

The continuous phase periodically acquires local data, retrieves distributed-runtime status and updates both presentation interfaces.

```text
app_main()
        │
        ├── Initialize Platform
        ├── Initialize Runtime Services
        ├── Initialize Mini OLED
        │
        ▼
while (true)
        │
        ├── Read ENV-IV
        ├── Read DLight
        ├── Update Mini OLED
        ├── Read Network Status
        ├── Read Semantic Receiver Status
        ├── Assemble Console Snapshot
        ├── Render Runtime Console
        ├── Emit Runtime Heartbeat
        └── Wait for Next Cycle
```

The current runtime update interval is:

```text
3000 ms
```

This interval is defined in the application as a FreeRTOS tick duration and controls the cadence of sensor acquisition, presentation updates and operational logging.

# Application Startup Sequence

The Ambient Runtime application follows a deterministic startup sequence.

The order is significant because later services depend on platform resources initialized by earlier stages.

```text
app_main()
        │
        ▼
Tab5 Platform
        │
        ▼
Runtime Console
        │
        ▼
LCD Backlight
        │
        ▼
Network Infrastructure
        │
        ▼
Semantic Event Receiver
        │
        ▼
PORT A I²C Bus
        │
        ▼
Mini OLED
        │
        ▼
Continuous Runtime Loop
```

Initialization failures in mandatory platform and runtime services are treated as fatal through:

```c
ESP_ERROR_CHECK(...);
```

The external PORT A I²C bus and Mini OLED are handled separately because their availability can be evaluated after the core services have started.

---

# Application Entry Point

The application starts in:

```c
extern "C" void app_main(void);
```

The function first emits the startup identification log:

```text
Ambient Physical AI - Ambient Runtime Console
```

It then initializes the core platform and runtime services.

---

# Core Initialization Order

The validated initialization order is:

```c
ESP_ERROR_CHECK(tab5_platform_init());
ESP_ERROR_CHECK(ambient_console_init());
ESP_ERROR_CHECK(tab5_platform_backlight_set(100));
ESP_ERROR_CHECK(ambient_network_init());
ESP_ERROR_CHECK(semantic_event_receiver_init());
```

Each call has a distinct responsibility.

---

## Tab5 Platform Initialization

```c
tab5_platform_init();
```

This operation prepares the board-level infrastructure required by the application.

The platform component is responsible for:

* internal board initialization;
* ESP32-C6 power enable;
* internal and external I²C buses;
* LCD and touch reset;
* backlight infrastructure;
* MIPI DSI initialization;
* ST7121 panel creation.

The application does not reproduce any of these procedures.

---

## Runtime Console Initialization

```c
ambient_console_init();
```

This operation allocates and prepares the framebuffer used by the primary Runtime Console.

The framebuffer is initialized before the LCD backlight is enabled.

This ordering prevents the application from intentionally enabling full display brightness before the console rendering infrastructure is available.

---

## LCD Backlight Enable

```c
tab5_platform_backlight_set(100);
```

After the platform and console have been initialized, the application sets the integrated LCD backlight to full brightness.

The application requests:

```text
100%
```

The PWM implementation remains inside `tab5_platform`.

---

## Network Initialization

```c
ambient_network_init();
```

This operation starts the network infrastructure.

The network connection itself is asynchronous.

A successful initialization call indicates that the networking subsystem was started; it does not guarantee that Wi-Fi association and usable IP configuration are already available.

The application later obtains the current state through:

```c
ambient_network_get_status();
```

---

## Semantic Receiver Initialization

```c
semantic_event_receiver_init();
```

This operation creates the background receiver task responsible for accepting Semantic Events and Ambient Context messages.

The receiver runs independently from the application loop.

The main application does not manage:

* UDP sockets;
* JSON parsing;
* message dispatch;
* context TTL evaluation.

It consumes only the thread-safe snapshots exposed by the receiver component.

---

# PORT A I²C Bus Acquisition

After the core services are initialized, the application obtains the external I²C bus through:

```c
i2c_master_bus_handle_t bus =
    tab5_platform_get_port_a_i2c_bus();
```

This handle is acquired once and reused throughout the complete runtime lifetime.

The application verifies the returned handle before accessing external peripherals.

```c
if (bus == nullptr) {
    ESP_LOGE(TAG, "PORT A I2C bus is not available");
    return;
}
```

If the bus is unavailable, the application terminates `app_main()` before entering the runtime loop.

This prevents external drivers from receiving an invalid bus handle.

---

# PaHub Channel Ownership

The application owns PaHub channel selection.

The channel mapping is defined locally as compile-time constants:

```c
static constexpr uint8_t PAHUB_CHANNEL_ENV_IV = 0;
static constexpr uint8_t PAHUB_CHANNEL_OLED   = 1;
static constexpr uint8_t PAHUB_CHANNEL_DLIGHT = 2;
```

The resulting topology is:

```text
PORT A I²C
        │
        ▼
PaHub
        │
        ├── Channel 0 ── ENV-IV
        ├── Channel 1 ── Mini OLED
        └── Channel 2 ── DLight
```

Before accessing any downstream device, the application calls:

```c
pahub_select_channel(bus, channel);
```

This responsibility intentionally remains outside the individual drivers.

Consequently:

* `env_iv` does not know that the ENV-IV is behind a multiplexer;
* `dlight` does not know that the DLight is behind a multiplexer;
* `oled_sh1107` does not know that the Mini OLED is behind a multiplexer.

This preserves reusable device drivers while keeping physical routing knowledge in the application layer.

---

# Mini OLED Initialization

The Mini OLED is initialized after the PORT A I²C bus becomes available.

The application initially assumes:

```c
bool mini_oled_ok = false;
```

The initialization sequence is:

```text
Select PaHub Channel 1
        │
        ▼
Initialize SH1107 Driver
        │
        ▼
Retrieve Initial Ambient Context
        │
        ▼
Render Initial Presentation
        │
        ▼
Record OLED Status
```

---

## Channel Selection

The application first selects the Mini OLED channel:

```c
esp_err_t ret =
    pahub_select_channel(bus, PAHUB_CHANNEL_OLED);
```

The SH1107 initialization is attempted only when channel selection succeeds.

---

## SH1107 Initialization

```c
ret = oled_sh1107_init(bus);
```

This initializes the low-level Mini OLED driver.

The application does not send SH1107 commands directly.

---

## Initial Context Retrieval

After successful display initialization, the application retrieves the current normalized context:

```c
ambient_context_snapshot_t initial_context =
    semantic_event_receiver_get_ambient_context();
```

At this stage, the receiver may not yet have accepted any message.

The returned snapshot can therefore represent the initial state in which no Ambient Context has been received.

---

## Initial Presentation

The initial snapshot is supplied to:

```c
oled_context_presenter_render(&initial_context);
```

The presenter determines the correct display state.

For an empty initial snapshot, the current presentation logic renders the waiting state rather than requiring special handling inside `main.cpp`.

---

## Initialization Result

If all Mini OLED initialization stages succeed:

```c
mini_oled_ok = true;
```

and the application logs:

```text
Mini OLED initialized
```

If any stage fails, the application records the error but still proceeds to the runtime loop.

This differs from the fatal treatment used for core platform initialization.

The Mini OLED is therefore monitored as a runtime peripheral whose status may change during subsequent cycles.

---

# Continuous Runtime Loop

After initialization, the application enters an infinite loop:

```c
while (true) {
    ...
}
```

Each cycle coordinates:

1. local measurement structures;
2. PaHub channel selection;
3. ENV-IV acquisition;
4. DLight acquisition;
5. Mini OLED context presentation;
6. network-status retrieval;
7. Semantic Receiver status retrieval;
8. Runtime Console snapshot assembly;
9. Runtime Console rendering;
10. operational heartbeat logging;
11. delay until the next cycle.

---

# Runtime Cycle Initialization

At the beginning of each cycle, the application creates local measurement variables.

```c
env_iv_sht40_data_t sht40 = {};
env_iv_bmp280_data_t bmp280 = {};
float lux = 0.0f;
```

It also initializes the runtime health indicators:

```c
bool pahub_ok = true;
bool env_iv_ok = false;
bool dlight_ok = false;
```

The local measurement structures are cleared on every cycle.

Consequently, failed acquisitions do not unintentionally reuse values stored in the previous loop iteration.

---

# ENV-IV Acquisition

The application selects PaHub channel `0` before accessing the ENV-IV unit.

```text
Select Channel 0
        │
        ▼
Read SHT40
        │
        ▼
Read BMP280
        │
        ▼
Evaluate ENV-IV Status
```

The two sensor acquisitions are requested independently:

```c
sht_ret = env_iv_sht40_read(bus, &sht40);
bmp_ret = env_iv_bmp280_read(bus, &bmp280);
```

If PaHub channel selection fails:

* neither sensor is accessed;
* `pahub_ok` becomes `false`;
* both sensor results remain failure values.

The ENV-IV is considered operational only when both acquisitions succeed:

```c
env_iv_ok =
    (sht_ret == ESP_OK) &&
    (bmp_ret == ESP_OK);
```

This means that a successful SHT40 reading combined with a failed BMP280 reading results in:

```text
ENV-IV = ERROR
```

for the consolidated Runtime Console status.

---

# Environmental Logging

Successful SHT40 acquisition produces temperature and humidity diagnostics.

Successful BMP280 acquisition produces temperature and atmospheric-pressure diagnostics.

Failures are logged independently.

This allows engineering diagnostics to distinguish:

* SHT40 failure;
* BMP280 failure;
* PaHub routing failure.

---

# DLight Acquisition

After the ENV-IV cycle, the application selects PaHub channel `2`.

```text
Select Channel 2
        │
        ▼
Read DLight
        │
        ▼
Evaluate DLight Status
```

The acquisition is performed through:

```c
dlight_read_lux(bus, &lux);
```

When successful:

```c
dlight_ok = true;
```

If channel selection fails:

* `pahub_ok` becomes `false`;
* the DLight driver is not called.

The resulting illumination value is later included in the Runtime Console snapshot.

---

# Mini OLED Runtime Update

The application selects PaHub channel `1` before every presentation attempt.

```text
Select Channel 1
        │
        ▼
Retrieve Ambient Context Snapshot
        │
        ▼
Render Through Presenter
        │
        ▼
Update Mini OLED Status
```

The current snapshot is retrieved through:

```c
ambient_context_snapshot_t context =
    semantic_event_receiver_get_ambient_context();
```

The application then calls:

```c
oled_context_presenter_render(&context);
```

The presenter receives only normalized data.

The following responsibilities remain outside the application and presenter:

* profile rules;
* external API selection;
* relevance evaluation;
* personal-content generation.

Those decisions remain in the Cognitive Runtime.

---

# OLED Status Recovery

The `mini_oled_ok` flag is reevaluated during every cycle.

If a render operation succeeds:

```c
mini_oled_ok = true;
```

If rendering or channel selection fails:

```c
mini_oled_ok = false;
```

Therefore, a failure during startup does not permanently define the Mini OLED status.

A later successful runtime update can return the peripheral to an operational state.

---

# Runtime Status Retrieval

After the external-device operations, the application retrieves two independent runtime snapshots.

## Network Snapshot

```c
ambient_network_status_t network =
    ambient_network_get_status();
```

The snapshot contains:

* initialization state;
* association state;
* network readiness;
* IPv4 address;
* network mask;
* gateway;
* RSSI.

---

## Semantic Receiver Snapshot

```c
semantic_event_receiver_status_t semantic_receiver =
    semantic_event_receiver_get_status();
```

The snapshot contains:

* receiver initialization state;
* listening state;
* event reception state;
* received-event count;
* latest event type;
* latest target.

These snapshots are retrieved by value.

The application does not access the internal state of either component.

---

# Runtime Update Interval

After completing the presentation and monitoring operations, the application waits:

```c
vTaskDelay(RUNTIME_UPDATE_INTERVAL);
```

The interval is defined as:

```c
static constexpr TickType_t RUNTIME_UPDATE_INTERVAL =
    pdMS_TO_TICKS(3000);
```

The resulting nominal cycle period is:

```text
3 seconds
```

The actual interval between completed cycles also includes the time required for:

* PaHub switching;
* sensor conversion delays;
* sensor communication;
* OLED rendering;
* Console rendering;
* diagnostic logging.

The implementation does not use a strict periodic scheduler or compensate for processing time.



# Runtime Data Integration

The final stage of each runtime cycle consists of consolidating information produced by independent components into a single normalized application snapshot.

Rather than allowing the Runtime Console to communicate directly with every subsystem, the application assembles a unified data structure and forwards it for presentation.

This approach keeps the console independent from hardware interfaces, communication protocols and runtime services.


# Runtime Console Snapshot

The application creates an instance of:

```c
ambient_console_data_t console = {};
```

The structure is populated with information collected during the current execution cycle.

The consolidated snapshot includes:

* environmental measurements;
* ambient illumination;
* Wi-Fi status;
* IPv4 configuration;
* network readiness;
* RSSI;
* Cognitive Runtime communication status;
* latest semantic event information;
* external peripheral health indicators.

Only after the structure has been completely populated does the application request rendering.

---

# Environmental Information

The following values are copied into the Runtime Console snapshot:

```text
Temperature
Humidity
Atmospheric Pressure
Ambient Illumination
```

These values originate from the ENV-IV and DLight components.

The application performs no additional processing beyond transferring the validated measurements into the normalized console structure.

---

# Network Information

The Runtime Console receives the current networking state through the normalized snapshot.

The application transfers:

* Wi-Fi association state;
* network readiness;
* IPv4 address;
* network mask;
* default gateway;
* received signal strength (RSSI).

The Console therefore remains independent from the networking implementation.

All network-specific processing remains encapsulated inside the `ambient_network` component.

---

# Cognitive Runtime Information

The application also transfers the current status of the Semantic Event Receiver.

The Runtime Console receives:

* whether a semantic event has already been received;
* the latest event type;
* the latest target identifier.

If no event has yet been processed, the application explicitly supplies:

```text
none
```

for both textual fields.

This avoids requiring the Console to interpret an uninitialized semantic state.

---

# Runtime Health Indicators

The application continuously evaluates the operational status of the external peripherals.

The Runtime Console receives the following health indicators:

| Component | Indicator      |
| --------- | -------------- |
| PaHub     | `pahub_ok`     |
| ENV-IV    | `env_iv_ok`    |
| DLight    | `dlight_ok`    |
| Mini OLED | `mini_oled_ok` |

These indicators summarize the success or failure of the most recent acquisition cycle.

The Console does not inspect individual driver return codes.

---

# Runtime Console Rendering

After assembling the normalized snapshot, the application performs a single rendering request.

```c
ambient_console_render(&console);
```

This design separates:

```text
Data Acquisition
        │
        ▼
Data Normalization
        │
        ▼
Presentation
```

The Runtime Console remains responsible only for visualization.

The application remains responsible only for orchestration.

---

# Operational Heartbeat

At the end of every cycle the application emits a consolidated operational log.

The heartbeat summarizes:

* platform availability;
* I²C infrastructure;
* environmental sensors;
* Mini OLED;
* Wi-Fi connection;
* IP configuration;
* RSSI;
* network readiness.

The resulting log provides an immediate overview of the operational state of the Ambient Runtime during engineering validation.

---

# Fault Tolerance

The application distinguishes between critical initialization failures and recoverable runtime failures.

## Critical Failures

The following services are considered mandatory:

* platform initialization;
* Runtime Console initialization;
* network infrastructure initialization;
* Semantic Event Receiver initialization.

Failures during these stages terminate the initialization sequence through `ESP_ERROR_CHECK()`.

---

## Recoverable Failures

Peripheral failures occurring during runtime do not terminate the application.

Examples include:

* unsuccessful ENV-IV acquisition;
* unsuccessful DLight acquisition;
* Mini OLED rendering failures;
* temporary PaHub communication failures.

Instead, the application:

* records the failure;
* updates the corresponding health indicator;
* continues executing the next runtime cycle.

This behavior improves the robustness of the Ambient Runtime while preserving continuous operation of unaffected subsystems.

---

# Engineering Principles

The current implementation follows several architectural principles.

## Separation of Responsibilities

Each runtime component remains responsible for its own implementation.

The application coordinates component execution but does not replicate internal functionality.

---

## Hardware Abstraction

Board-specific hardware details remain inside dedicated components.

The application never manipulates:

* GPIO expanders;
* display controllers;
* sensor registers;
* Wi-Fi hardware.

---

## Normalized Interfaces

Information exchanged between components is normalized through public data structures.

This minimizes coupling between:

* presentation;
* sensing;
* networking;
* semantic communication.

---

## Graceful Degradation

Failures affecting optional peripherals are isolated.

The Runtime Console continues operating even if one or more external devices become temporarily unavailable.

This behavior is particularly valuable during engineering validation and laboratory experimentation.

---

# Build

The application is built automatically as part of the Ambient Runtime firmware.

Typical development workflow:

```bash
cd firmware/nodes/ambient-runtime-node

idf.py build

idf.py flash monitor
```

A complete rebuild may be performed when application dependencies or platform configuration change.

```bash
idf.py fullclean

idf.py build

idf.py flash monitor
```

---

# Related Documentation

| Document                                                                           | Description                                       |
| ---------------------------------------------------------------------------------- | ------------------------------------------------- |
| `firmware/nodes/ambient-runtime-node/README.md`                                    | Ambient Runtime architectural overview.           |
| `firmware/nodes/ambient-runtime-node/components/README.md`                         | Runtime component organization.                   |
| `firmware/nodes/ambient-runtime-node/components/tab5_platform/README.md`           | Platform initialization and hardware abstraction. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md`         | Runtime Console implementation.                   |
| `firmware/nodes/ambient-runtime-node/components/ambient_network/README.md`         | Network infrastructure.                           |
| `firmware/nodes/ambient-runtime-node/components/semantic_event_receiver/README.md` | Ambient Context receiver.                         |
| `firmware/nodes/ambient-runtime-node/components/pahub/README.md`                   | External I²C multiplexer.                         |
| `firmware/nodes/ambient-runtime-node/components/env_iv/README.md`                  | Environmental sensing subsystem.                  |
| `firmware/nodes/ambient-runtime-node/components/dlight/README.md`                  | Ambient light sensor.                             |
| `firmware/nodes/ambient-runtime-node/components/oled_context_presenter/README.md`  | Mini OLED presentation layer.                     |
| `firmware/nodes/ambient-runtime-node/components/oled_sh1107/README.md`             | SH1107 display driver.                            |

---

# Conclusion

The `main` application serves as the orchestration layer of the Ambient Runtime.

Rather than implementing hardware drivers or runtime services directly, it coordinates the initialization, execution and integration of the validated components that compose the runtime.

Its primary responsibilities are to establish the application lifecycle, coordinate periodic data acquisition, assemble a normalized runtime snapshot and update the user-facing presentation layers.

This separation of concerns allows individual components to evolve independently while preserving a clear and maintainable application structure.

The resulting implementation provides a deterministic initialization sequence, continuous operational monitoring and a stable integration point between the Ambient Runtime, the underlying hardware platform and the Cognitive Runtime.
