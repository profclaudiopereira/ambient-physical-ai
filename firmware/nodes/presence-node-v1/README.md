# Presence Layer --- Presence Node V2

## Ambient Physical AI

### Distributed Human Presence Detection Node

The **Presence Layer** is the first physical perception layer of the
Ambient Physical AI ecosystem.

Its responsibility is to detect whether a person is present in the
environment and publish a semantic `presence_event` to the Identity
Layer.

The current official implementation uses:

``` text
M5Stack AtomS3 Lite
+
HLK-LD2410C
+
ESP-IDF
+
Wi-Fi
+
UDP Unicast
```

The node performs native human-presence detection using 24 GHz mmWave
radar, maintains a local presence state machine, connects to the local
Wi-Fi network and notifies the Identity Node whenever the state changes
from `NOT_PRESENT` to `PRESENT`.

------------------------------------------------------------------------

# Project Status

``` text
Presence Node V2
OFFICIAL BASELINE
VALIDATED
```

Current milestone:

``` text
PRESENCE_NODE_V2_MILESTONE_002
Presence → Identity UDP unicast integration validated
```

Validation date:

``` text
2026-07-17
```

Validated end-to-end path:

``` text
Human presence
      ↓
HLK-LD2410C
      ↓
Radar frame parser
      ↓
Presence state machine
      ↓
Wi-Fi Station
      ↓
UDP unicast :3333
      ↓
Identity Node
      ↓
NFC identification prompt
```

Result:

``` text
PASS
```

------------------------------------------------------------------------

## Repository Note

The current validated implementation described in this document corresponds to **Presence Node V2**, based on the HLK-LD2410C 24 GHz mmWave radar.

The repository directory intentionally retains the historical name `presence-node-v1` to preserve Git history, maintain stable repository references, and avoid unnecessary repository restructuring.

------------------------------------------------------------------------

# Role in the Ambient Physical AI Architecture

The Presence Layer does not identify the person and does not perform
cognitive processing.

Its scope is intentionally narrow:

``` text
Detect human presence
        ↓
Generate a semantic presence transition
        ↓
Notify the Identity Layer
```

Full system journey:

``` text
Human enters the environment
        ↓
Presence Node detects presence
        ↓
Presence Node sends presence_event
        ↓
Identity Node displays “Tap NFC card”
        ↓
Identity Node reads the NFC card
        ↓
Identity Package is generated
        ↓
Cognitive Runtime receives identity and context
```

------------------------------------------------------------------------

# Official Hardware

## Processing Unit

``` text
M5Stack AtomS3 Lite
ESP32-S3
```

## Presence Sensor

``` text
HLK-LD2410C
24 GHz FMCW mmWave radar
```

## UART Configuration

``` text
UART1
Baud rate: 256000
8 data bits
No parity
1 stop bit
No hardware flow control
```

Validated pin mapping:

  Signal                       AtomS3 Lite HLK-LD2410C
  --------- ------------------------------ -------------
  UART TX                            GPIO1 RX
  UART RX                            GPIO2 TX
  Ground                               GND GND
  Power       validated module power input VCC

UART lines must be crossed:

``` text
AtomS3 TX → LD2410C RX
AtomS3 RX ← LD2410C TX
GND        ↔ GND
```

> Verify the voltage requirements of the exact LD2410C carrier board
> before powering it.

------------------------------------------------------------------------

# Engineering Evolution

## Presence Node V1

The first validated implementation used:

``` text
AtomS3 Lite
+
Unit Mini ToF-90
+
VL53L0X
```

That baseline validated:

-   ESP-IDF bring-up on AtomS3 Lite;
-   I²C communication;
-   distance measurement;
-   threshold-based presence detection;
-   Wi-Fi Station mode;
-   semantic `presence_event`;
-   initial Presence → Identity integration.

The VL53L0X inferred presence indirectly:

``` text
Object distance
      ↓
Distance threshold
      ↓
Presence assumption
```

## Presence Node V2

The current implementation uses the LD2410C because it is designed for
human-presence detection:

``` text
Human movement
+
Stationary human micro-movement
      ↓
Native radar target state
      ↓
Presence
```

Only the sensing subsystem changed. The distributed semantic
architecture was preserved.

------------------------------------------------------------------------

# Repository Structure

The directory retains its historical name:

``` text
firmware/nodes/presence-node-v1/
```

Current structure:

``` text
firmware/nodes/presence-node-v1/
├── CMakeLists.txt
├── README.md
├── components/
│   └── ld2410/
│       ├── CMakeLists.txt
│       ├── ld2410.c
│       ├── ld2410_parser.c
│       ├── ld2410_internal.h
│       └── include/
│           └── ld2410.h
└── main/
    ├── CMakeLists.txt
    └── main.cpp
```

The folder was not renamed in order to preserve repository history and
avoid unnecessary structural changes.

------------------------------------------------------------------------

# Software Architecture

## Reusable LD2410 Component

``` text
components/ld2410/
```
The reusable LD2410 component is documented separately in:

```text
components/ld2410/README.md
```

This document describes only the role of the component within the Presence Layer. Detailed information about the public API, driver lifecycle, streaming parser architecture and protocol decoding is available in the component documentation.


Responsibilities:

-   UART initialization;
-   byte-stream reception;
-   normal-frame synchronization;
-   frame validation;
-   target-state decoding;
-   moving-target data;
-   stationary-target data;
-   detection-distance extraction;
-   public driver API.

Public interface:

``` c
esp_err_t ld2410_new(
    const ld2410_config_t *config,
    ld2410_handle_t *out_handle
);

esp_err_t ld2410_read(
    ld2410_handle_t handle,
    ld2410_target_data_t *out_data,
    TickType_t timeout_ticks
);

esp_err_t ld2410_delete(
    ld2410_handle_t handle
);
```

## Presence Runtime

``` text
main/main.cpp
```

Responsibilities:

-   initialize the radar;
-   translate LD2410C target states into presence;
-   maintain the local presence state;
-   initialize Wi-Fi;
-   acquire an IPv4 address through DHCP;
-   configure the UDP destination;
-   send a semantic `presence_event`;
-   retain a pending event when presence is detected before networking
    is ready.

------------------------------------------------------------------------

# Target-State Mapping

  LD2410C state                           Presence interpretation
  --------------------------------------- ---------------------------------
  `LD2410_TARGET_NONE`                    `NOT_PRESENT`
  `LD2410_TARGET_MOVING`                  `PRESENT`
  `LD2410_TARGET_STATIONARY`              `PRESENT`
  `LD2410_TARGET_MOVING_AND_STATIONARY`   `PRESENT`
  `LD2410_TARGET_UNKNOWN`                 preserve current presence state

An unknown or malformed reading does not create a false absence
transition.

------------------------------------------------------------------------

# Presence State Machine

The local state machine recognizes:

``` text
NOT_PRESENT → PRESENT
PRESENT → NOT_PRESENT
```

A UDP event is transmitted only when entering `PRESENT`.

``` text
Radar reports a human target
        ↓
presence_state = true
        ↓
PRESENT
        ↓
presence_event transmitted
```

When no target is detected:

``` text
Radar reports NONE
        ↓
presence_state = false
        ↓
NOT_PRESENT
```

Repeated radar frames while presence remains active do not generate
repeated events.

------------------------------------------------------------------------

# UDP Semantic Contract

## Validated Network Path

``` text
Presence Node: 192.168.77.19
Identity Node: 192.168.77.7
UDP port:     3333
Transport:    unicast
```

Firmware destination:

``` cpp
#define UDP_BROADCAST_IP "192.168.77.7"
#define UDP_PORT 3333
```

> The legacy macro name `UDP_BROADCAST_IP` remains in the current
> source, but the validated destination is now a unicast host address. A
> future editorial refactor may rename the macro without changing
> behavior.

## Payload

``` json
{
  "type": "presence_event",
  "state": "PRESENT",
  "distance_mm": 1950,
  "source": "presence_node_v1"
}
```

The legacy source identifier remains intentionally unchanged for
compatibility with the Identity Node.

## Distance Compatibility

The LD2410C reports `detection_distance_cm`. The runtime converts that
value to the existing contract:

``` text
detection_distance_cm × 10 = distance_mm
```

Example:

``` text
195 cm → 1950 mm
```

------------------------------------------------------------------------

# Pending Event After Wi-Fi Startup

Presence can be detected before DHCP completes.

Expected sequence:

``` text
PRESENT
UDP send postponed: Wi-Fi not connected yet
...
Wi-Fi connected
...
UDP presence_event sent
```

The firmware stores the pending detection distance and sends the event
after the network becomes ready.

This behavior was validated.

------------------------------------------------------------------------

# Build Requirements

Validated toolchain:

``` text
ESP-IDF 5.4.2
Target: esp32s3
CMake
Ninja
Git
```
### Wi-Fi Configuration

Before building the firmware, configure the Wi-Fi credentials:

```cpp
#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASS "<YOUR_WIFI_PASSWORD>"
```

Replace the placeholder values with the credentials of your local Wi-Fi network.

From the node directory:

``` bash
cd firmware/nodes/presence-node-v1
idf.py set-target esp32s3
idf.py build
```

For a clean rebuild:

``` bash
idf.py fullclean
idf.py set-target esp32s3
idf.py build
```

------------------------------------------------------------------------

# Flash and Monitor

``` bash
idf.py -p COMx flash monitor
```

Exit the monitor:

``` text
Ctrl + ]
```

------------------------------------------------------------------------

# Expected Boot Log

``` text
PRESENCE_NODE_V2_MILESTONE_001
AtomS3 Lite + HLK-LD2410C
UART1: TX=GPIO1 RX=GPIO2 baud=256000
LD2410C initialized
Wi-Fi STA started
UDP ... configured
```

After DHCP:

``` text
Wi-Fi connected. IP      : 192.168.77.19
Netmask                 : 255.255.255.0
Gateway                 : 192.168.77.1
Network ready           : YES
```

------------------------------------------------------------------------

# Validated Runtime Evidence

Presence transition:

``` text
Radar: state=MOVING_AND_STATIONARY detection=195 cm ...
PRESENT
UDP presence_event sent: {"type":"presence_event","state":"PRESENT","distance_mm":1950,"source":"presence_node_v1"}
```

Matching Identity Node evidence:

``` text
UDP RX: 90 bytes from 192.168.77.19:55876
UDP payload: {"type":"presence_event","state":"PRESENT","distance_mm":1950,"source":"presence_node_v1"}
Valid presence_event received
Presence event received: show NFC prompt
```

------------------------------------------------------------------------

# Validation Checklist

## Radar

``` text
UART initialization ................ PASS
LD2410C frame parsing ............... PASS
MOVING detection .................... PASS
STATIONARY detection ................ PASS
MOVING_AND_STATIONARY detection ..... PASS
NONE detection ...................... PASS
```

## State Machine

``` text
NOT_PRESENT → PRESENT ............... PASS
PRESENT → NOT_PRESENT ............... PASS
Repeated-event suppression .......... PASS
UNKNOWN state preservation .......... PASS
```

## Network

``` text
Wi-Fi association ................... PASS
DHCP IPv4 acquisition ............... PASS
Pending event after DHCP ............ PASS
UDP unicast transmission ............ PASS
```

## Integration

``` text
Semantic payload generation ......... PASS
Identity Node reception ............. PASS
Payload validation .................. PASS
NFC prompt activation ............... PASS
```

------------------------------------------------------------------------

# Current Scope

Implemented:

-   native human-presence detection;
-   local presence state machine;
-   Wi-Fi Station connectivity;
-   DHCP;
-   UDP unicast event transmission;
-   pending-event retry;
-   semantic `presence_event`;
-   validated Presence → Identity trigger.

Not implemented in this node:

-   identity recognition;
-   NFC reading;
-   LLM inference;
-   MQTT;
-   StackFlow orchestration;
-   ambient actuation;
-   expression output.

------------------------------------------------------------------------

# Related Documentation

| Document | Description |
|----------|-------------|
| `components/ld2410/README.md` | LD2410 reusable radar driver architecture and public API. |

------------------------------------------------------------------------

# Final Status

``` text
PRESENCE NODE V2                         VALIDATED
LD2410C HUMAN-PRESENCE DETECTION         VALIDATED
UDP UNICAST TO IDENTITY NODE             VALIDATED
PRESENCE → IDENTITY INTEGRATION          VALIDATED
```
