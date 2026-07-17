# Presence Layer --- Presence Node

## Ambient Physical AI

The **Presence Layer** is the first perception node of the Ambient
Physical AI architecture.

Its responsibility is simple:

``` text
Detect Human Presence
        ↓
Generate presence_event
        ↓
Notify Identity Layer
```

Current implementation:

``` text
AtomS3 Lite
+
HLK-LD2410C Radar
+
ESP-IDF
+
Wi-Fi
+
UDP Broadcast
```

Status:

``` text
Presence Node V2
OFFICIAL BASELINE
VALIDATED
```

------------------------------------------------------------------------

# Hardware

  Component   Description
  ----------- ---------------------------------
  MCU         M5Stack AtomS3 Lite (ESP32-S3)
  Sensor      HLK-LD2410C 24 GHz mmWave Radar

UART wiring:

``` text
GPIO1 (TX) → Radar RX
GPIO2 (RX) ← Radar TX
GND ↔ GND
```

------------------------------------------------------------------------

# Repository Structure

``` text
presence-node-v1/
├── components/
│   └── ld2410/
├── main/
│   ├── main.cpp
│   └── CMakeLists.txt
├── README.md
└── CMakeLists.txt
```

------------------------------------------------------------------------

# Features

Validated:

``` text
HLK-LD2410C communication .... PASS
Radar frame parser ........... PASS
Presence State Machine ....... PASS
Wi-Fi Station ................ PASS
DHCP ......................... PASS
UDP Broadcast ................ PASS
Presence → Identity .......... PASS
```

------------------------------------------------------------------------

# Building

From the node directory:

``` bash
idf.py set-target esp32s3
idf.py build
```

------------------------------------------------------------------------

# Flash

``` bash
idf.py -p COMx flash
```

------------------------------------------------------------------------

# Monitor

``` bash
idf.py -p COMx monitor
```

or

``` bash
idf.py -p COMx flash monitor
```

Exit monitor:

``` text
Ctrl + ]
```

------------------------------------------------------------------------

# Expected Boot

``` text
LD2410 initialized
Wi-Fi connected
Network ready
UDP broadcast configured
```

Expected runtime:

``` text
PRESENT
...
NOT_PRESENT
...
PRESENT
```

------------------------------------------------------------------------

# UDP Event

``` json
{
  "type":"presence_event",
  "state":"PRESENT",
  "distance_mm":3430,
  "source":"presence_node_v1"
}
```

------------------------------------------------------------------------

# Testing

1.  Flash the firmware.
2.  Connect the node to the same Wi-Fi network as the Identity Node.
3.  Open the serial monitor.
4.  Walk into the radar detection area.
5.  Verify:
    -   `PRESENT`
    -   UDP event transmitted.
6.  Leave the area.
7.  Verify:
    -   `NOT_PRESENT`

------------------------------------------------------------------------

# Engineering History

## V1

``` text
AtomS3 Lite
+
VL53L0X (ToF)
```

Validated:

-   I²C communication
-   Distance measurement
-   Threshold-based presence
-   Initial Presence → Identity integration

## V2 (Current)

``` text
AtomS3 Lite
+
HLK-LD2410C
```

Migration goals achieved:

-   Native human presence detection
-   Reusable radar component
-   Wi-Fi preserved
-   UDP preserved
-   Identity integration preserved

Only the sensing subsystem changed.

------------------------------------------------------------------------

# Documentation

Additional documentation should be placed under `docs/`, for example:

``` text
docs/
├── architecture/
├── hardware/
├── discoveries/
├── migration/
└── testing/
```

The README intentionally remains concise and serves as the entry point
for this node.

------------------------------------------------------------------------

# Related Components

``` text
firmware/nodes/identity-node/
firmware/nodes/ambient-runtime-node/
runtime/cognitive/stackflow/
```
