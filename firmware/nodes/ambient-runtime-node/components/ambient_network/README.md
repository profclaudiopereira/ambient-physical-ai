# Ambient Network

## Ambient Physical AI

### Network Infrastructure for the Ambient Runtime

---

# Overview

The `ambient_network` component provides the network infrastructure required by the Ambient Runtime.

It encapsulates the ESP-IDF networking services used to initialize Wi-Fi Station mode, establish local network connectivity, track connection events and expose the current network state to the remainder of the application.

The component is designed as an infrastructure service.

Application modules do not interact directly with:

* `esp_wifi`;
* `esp_netif`;
* ESP-IDF network events;
* Wi-Fi event handlers;
* IP event handlers;
* NVS initialization.

Instead, the Ambient Runtime initializes the component once and retrieves a normalized snapshot representing the current network state.

---

# Purpose

The purpose of the `ambient_network` component is to isolate network initialization and connection-state management from the main application.

Current responsibilities include:

* initializing NVS;
* initializing `esp_netif`;
* creating the default ESP-IDF event loop;
* creating the Wi-Fi Station network interface;
* registering Wi-Fi and IP event handlers;
* configuring Wi-Fi Station mode;
* initiating network association;
* handling disconnection and reconnection;
* storing IPv4 network information;
* monitoring signal strength;
* exposing a normalized network status snapshot.

The component does not implement application protocols or semantic communication.

UDP message reception and Cognitive Runtime integration remain responsibilities of dedicated runtime components.

---

# Position within the Ambient Runtime

The network component occupies the infrastructure layer of the Ambient Runtime.

```text
Ambient Runtime Application
        │
        ├── ambient_console
        ├── semantic_event_receiver
        └── other runtime services
                    │
                    ▼
             ambient_network
                    │
                    ▼
           ESP-IDF Wi-Fi API
                    │
                    ▼
      ESP Wi-Fi Remote / ESP-Hosted
                    │
                    ▼
               ESP32-C6
                    │
                    ▼
          Local Wi-Fi Network
```

On the M5Stack Tab5, the ESP32-P4 executes the Ambient Runtime application while the ESP32-C6 provides the wireless interface.

The network component uses the standard ESP-IDF Wi-Fi API exposed to the ESP32-P4 environment.

The lower-level communication between the ESP32-P4 and ESP32-C6 remains outside the application-facing API of this component.

---

# Responsibilities

## NVS Initialization

Initialize the ESP-IDF non-volatile storage service required by the Wi-Fi subsystem.

If the NVS partition reports an incompatible version or insufficient free pages, the component erases and reinitializes the partition before continuing.

---

## Network Stack Initialization

Initialize the ESP-IDF network interface subsystem and default event loop.

The implementation tolerates an already initialized network stack by accepting the corresponding ESP-IDF invalid-state result where appropriate.

---

## Wi-Fi Station Interface

Create and configure the default Wi-Fi Station network interface.

The current implementation operates exclusively in:

```text
WIFI_MODE_STA
```

Access Point mode is not implemented by this component.

---

## Wi-Fi Configuration

Configure the Station interface using deployment-provided credentials.

The source currently expects the following placeholders to be replaced or supplied for the target environment:

```c
#define AMBIENT_WIFI_SSID "<YOUR_WIFI_SSID>"
#define AMBIENT_WIFI_PASS "<YOUR_WIFI_PASSWORD>"
```

Production deployments should avoid committing real credentials to the repository.

A future configuration mechanism may use:

* Kconfig;
* NVS;
* provisioning;
* deployment-specific configuration files.

These mechanisms are not part of the current validated component scope.

---

## Connection Management

Initiate the Wi-Fi connection when the Station interface starts.

The component reacts to ESP-IDF Wi-Fi events and updates its internal state according to the current association status.

---

## Minimal Reconnection

Request a new connection after a Wi-Fi disconnection event.

The current policy is intentionally minimal and is intended to improve runtime continuity in the controlled demonstration environment.

The component does not implement:

* exponential backoff;
* connection attempt limits;
* multi-network failover;
* captive portal handling;
* provisioning fallback.

---

## Network Address Management

Maintain the network addresses associated with the Station interface.

The normalized network snapshot includes:

* IPv4 address;
* network mask;
* default gateway.

When the connection is lost, these values are reset to:

```text
0.0.0.0
```

---

## RSSI Monitoring

Update the current received signal strength indicator when the Station interface is associated with an access point.

The RSSI value is exposed in decibel-milliwatts:

```text
dBm
```

---

# Component Architecture

```text
ambient_network_init()
        │
        ├── NVS initialization
        ├── esp_netif initialization
        ├── event loop initialization
        ├── Wi-Fi Station interface creation
        ├── event handler registration
        ├── Wi-Fi configuration
        └── Wi-Fi start
                │
                ▼
        ESP-IDF Event System
                │
        ┌───────┴────────┐
        │                │
        ▼                ▼
   WIFI_EVENT         IP_EVENT
        │                │
        ▼                ▼
Connection State    Address State
        │                │
        └───────┬────────┘
                ▼
    ambient_network_status_t
                │
                ▼
       Ambient Runtime
```

The event-driven implementation remains private to the component.

The remainder of the application consumes only the public status snapshot.

---

# Network State Model

The component distinguishes between Wi-Fi association and complete network readiness.

```text
NOT INITIALIZED
        │
        ▼
INITIALIZED
        │
        ▼
STATION STARTED
        │
        ▼
CONNECTED
        │
        ▼
NETWORK READY
```

## Initialized

The network component has completed its local initialization and started the Wi-Fi subsystem.

This state does not indicate association with an access point.

---

## Connected

The Station interface is associated with an access point.

At this stage:

```c
connected = true;
network_ready = false;
```

Association alone does not guarantee that usable IP configuration is available.

---

## Network Ready

The Station interface has usable IP configuration and the component marks the network as operational.

At this stage:

```c
connected = true;
network_ready = true;
```

The runtime can then expose network information to dependent components and diagnostic interfaces.

---

## Disconnected

When a disconnection event occurs:

```c
connected = false;
network_ready = false;
```

The component also clears:

* IPv4 address;
* network mask;
* gateway;
* RSSI.

A reconnection request is then issued.

---

# Network Status Snapshot

The public network state is represented by:

```c
typedef struct {
    bool initialized;
    bool connected;
    bool network_ready;

    char ipv4[AMBIENT_NETWORK_IPV4_LENGTH];
    char netmask[AMBIENT_NETWORK_IPV4_LENGTH];
    char gateway[AMBIENT_NETWORK_IPV4_LENGTH];

    int8_t rssi_dbm;
} ambient_network_status_t;
```

This structure is a normalized snapshot of the current network state.

It intentionally does not expose:

* ESP-IDF event identifiers;
* `esp_netif_t` handles;
* Wi-Fi driver internals;
* ESP-Hosted internals;
* raw access-point records.

---

# Public API

The public API consists of two functions.

## Initialization

```c
esp_err_t ambient_network_init(void);
```

Initializes the network infrastructure and starts Wi-Fi Station operation.

The connection process is asynchronous.

A successful return indicates that initialization and the Wi-Fi start request completed successfully. It does not guarantee that association or IP configuration has already completed.

Repeated initialization calls return successfully without reinitializing the component.

---

## Status Retrieval

```c
ambient_network_status_t
ambient_network_get_status(void);
```

Returns a snapshot of the current network state.

When the Station interface is connected, the function also attempts to update the RSSI value before returning the snapshot.

The returned structure is a value copy and does not expose the internal network-state storage.

---

# Dependencies

The component depends on the following ESP-IDF services:

* `esp_common`;
* `log`;
* `nvs_flash`;
* `esp_netif`;
* `esp_event`;
* `esp_wifi`.

Its ESP-IDF component registration is equivalent to:

```cmake
idf_component_register(
    SRCS
        "ambient_network.cpp"

    INCLUDE_DIRS
        "include"

    REQUIRES
        esp_common
        log
        nvs_flash
        esp_netif
        esp_event
        esp_wifi
)
```

The component does not directly depend on:

* `ambient_console`;
* `semantic_event_receiver`;
* sensor drivers;
* OLED components;
* application-level protocols.

# Initialization Sequence

The `ambient_network` component performs a deterministic initialization sequence that prepares the networking infrastructure before the Ambient Runtime enters its operational state.

The sequence is intentionally isolated from the remainder of the application.

```text
ambient_network_init()
        │
        ▼
Initialize NVS
        │
        ▼
Initialize esp_netif
        │
        ▼
Create Default Event Loop
        │
        ▼
Create Wi-Fi Station Interface
        │
        ▼
Register Wi-Fi Events
        │
        ▼
Register IP Events
        │
        ▼
Initialize Wi-Fi Driver
        │
        ▼
Configure Wi-Fi Station
        │
        ▼
Start Wi-Fi
        │
        ▼
Asynchronous Connection
```

After initialization, all subsequent state transitions are driven by the ESP-IDF event system.

---

# Event Processing

The component internally handles two categories of events.

## Wi-Fi Events

The Wi-Fi event handler is responsible for:

- Station startup;
- access point association;
- disconnection detection;
- automatic reconnection.

These events update the internal connection state but do not directly expose ESP-IDF events to the remainder of the application.

---

## IP Events

The IP event handler updates the operational network state once a valid IPv4 configuration becomes available.

Current information maintained includes:

- IPv4 address;
- network mask;
- default gateway;
- network readiness.

Only after IP configuration is available does the component consider the network operational.

---

# Engineering Principles

The implementation follows the architectural principles adopted throughout the Ambient Physical AI project.

## Event Encapsulation

ESP-IDF event processing remains completely internal to the component.

Application modules never register Wi-Fi or IP event handlers directly.

Instead, they consume only the normalized network snapshot.

---

## Snapshot-Based Design

The component exposes a single immutable snapshot representing the current network state.

```text
ESP-IDF Events
        │
        ▼
Internal Network State
        │
        ▼
ambient_network_status_t
        │
        ▼
Application
```

This approach reduces coupling between the networking stack and application logic.

---

## Deterministic Initialization

Network initialization always follows the same sequence.

Repeated initialization requests are safely ignored after successful initialization.

This behavior simplifies application startup and prevents duplicated network infrastructure.

---

## Infrastructure Isolation

The component is responsible exclusively for network infrastructure.

It intentionally does not implement:

- UDP communication;
- MQTT;
- HTTP;
- WebSocket;
- semantic communication;
- application protocols.

These responsibilities belong to higher-level runtime components.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| NVS initialization | Validated |
| Network stack initialization | Validated |
| Wi-Fi Station initialization | Validated |
| Event loop registration | Validated |
| Wi-Fi event processing | Validated |
| IP event processing | Validated |
| Automatic reconnection | Validated |
| RSSI monitoring | Validated |
| Network status snapshot | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Configuration

The current implementation uses compile-time placeholders for the Wi-Fi credentials.

```c
#define AMBIENT_WIFI_SSID "<YOUR_WIFI_SSID>"
#define AMBIENT_WIFI_PASS "<YOUR_WIFI_PASSWORD>"
```

These placeholders are intended to be replaced by deployment-specific values before building the firmware.

The component has been intentionally designed so that credential management remains independent from the networking architecture.

Future versions may adopt alternative configuration mechanisms such as:

- Kconfig;
- NVS storage;
- Wi-Fi provisioning;
- secure deployment workflows.

These mechanisms are outside the scope of the current validated implementation.

---

# Design Decisions

Several implementation decisions contribute to the robustness of the Ambient Runtime networking infrastructure.

## Separation Between Connection and Readiness

The component distinguishes between:

```text
Connected
```

and

```text
Network Ready
```

A successful Wi-Fi association does not necessarily imply that the runtime has a usable network configuration.

Only after valid IP configuration becomes available does the component report the network as ready.

---

## Automatic Recovery

When a Wi-Fi disconnection occurs, the component automatically requests a new connection.

This strategy improves runtime continuity while keeping the implementation intentionally simple.

---

## Runtime Transparency

All runtime modules obtain network information from the same normalized snapshot.

No component needs to understand ESP-IDF networking internals.

This significantly simplifies maintenance and future evolution.

---

# Related Components

The network infrastructure supports several Ambient Runtime modules.

| Component | Relationship |
|-----------|--------------|
| `ambient_console` | Displays the current network status. |
| `semantic_event_receiver` | Depends on network connectivity to receive Semantic Events and Ambient Context messages. |
| `tab5_platform` | Provides the underlying execution platform for the networking stack. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md` | Runtime Console architecture. |
| `firmware/nodes/ambient-runtime-node/components/semantic_event_receiver/README.md` | Semantic communication layer. |
| `firmware/nodes/ambient-runtime-node/main/README.md` | Application initialization and runtime lifecycle. |

---

# Conclusion

The `ambient_network` component provides the networking infrastructure required by the Ambient Runtime.

By encapsulating ESP-IDF networking services, maintaining a normalized network status snapshot and isolating event-driven behavior from application modules, the component offers a clean, reusable and maintainable interface for embedded network management.

Its current implementation serves as the validated networking baseline for the Ambient Runtime and establishes a stable foundation upon which higher-level communication services operate.