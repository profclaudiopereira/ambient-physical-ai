# Presence → Identity Integration V1

## Ambient Physical AI

### Status

Validated on Real Hardware

---

# Purpose

This document records the first successful hardware integration between the Presence Layer and the Identity Layer of the Ambient Physical AI project.

The integration validates the following journey:

```text
Person approaches
↓
Presence Node detects presence
↓
UDP Broadcast
↓
Identity Node receives event
↓
M5Dial displays prompt
↓
User taps NFC card
↓
Identity Package is generated
```

---

# Architectural Significance

This milestone proves that Ambient Physical AI is no longer a set of isolated validated nodes.

The system now has its first validated inter-layer communication path:

```text
Presence Layer
↓
Identity Layer
```

This establishes the foundation for the next integration step:

```text
Presence
↓
Identity
↓
Cognitive Runtime
```

---

# Integration Scope

The goal was to integrate both nodes without changing their internal architectures.

No feature rollback was allowed.

No refactoring was performed.

The integration added only the communication layer required for:

```text
Presence Node
→
Identity Node
```

---

# Preserved Baselines

## Presence Node V1

Hardware:

```text
AtomS3 Lite
+
Unit Mini ToF-90 (VL53L0X)
```

Preserved features:

```text
VL53L0X initialization
I2C GPIO2 / GPIO1
Distance measurement
Serial distance output
Presence threshold logic
PRESENT / NOT_PRESENT state machine
FreeRTOS structure
```

---

## Identity Node V1

Hardware:

```text
M5Dial
+
WS1850S NFC
```

Preserved features:

```text
UI Task
NFC Task
Identity Event Queue
Display
Touch
Encoder
Buzzer
WS1850S runtime
UID Mapping
Identity Package generation
Identity Visualization
NFC recovery state machine
FreeRTOS architecture
```

---

# Communication Model

Implemented communication:

```text
Presence Node
↓
Wi-Fi STA
↓
UDP Broadcast
↓
Identity Node
```

Network configuration used during validation:

```text
SSID      : <YOUR_WIFI_SSID>
Broadcast : 192.168.0.255
Port      : 3333
```

---

# Presence Node Behavior

The Presence Node maintains distance measurement and presence state logic.

When the state transitions from:

```text
NOT_PRESENT
↓
PRESENT
```

the node sends one UDP broadcast message.

This avoids continuous network flooding.

---

# UDP Payload

Example payload:

```json
{
  "type": "presence_event",
  "state": "PRESENT",
  "distance_mm": 597,
  "source": "presence_node_v1"
}
```

---

# Identity Node Behavior

The Identity Node listens for UDP packets on port:

```text
3333
```

When a valid presence event is received, the UI displays:

```text
Presence detected
Tap NFC card
```

for:

```text
5 seconds
```

After the timeout, the interface returns to the normal Identity Console flow.

The existing NFC flow remains unchanged.

---

# End-to-End Flow

## Step 1 — Presence Detection

```text
Person approaches ToF sensor
↓
Distance decreases below threshold
↓
Presence Node changes state to PRESENT
```

---

## Step 2 — UDP Transmission

```text
Presence Node connects to Wi-Fi
↓
UDP broadcast socket sends presence_event
↓
Packet sent to 192.168.0.255:3333
```

---

## Step 3 — Identity Node Reception

```text
M5Dial connected to Wi-Fi
↓
UDP listener receives packet
↓
Internal event generated:
EVENT_PRESENCE_RECEIVED
```

---

## Step 4 — User Prompt

```text
Identity UI displays:
Presence detected
Tap NFC card
```

---

## Step 5 — Identity Acquisition

```text
User taps NFC card
↓
WS1850S reads UID
↓
UID maps to profile
↓
Identity Package generated
```

---

# Validation Results

## Presence Node

Validated:

```text
Boot
VL53L0X initialization
Distance measurement
Serial output
Presence detection
Wi-Fi connection
UDP transmission
```

Example log:

```text
Distance: 597 mm
PRESENT

UDP presence_event sent:
{
  "type":"presence_event",
  "state":"PRESENT",
  "distance_mm":597,
  "source":"presence_node_v1"
}
```

---

## Identity Node

Validated:

```text
Boot
UI Task
NFC Task
Encoder
Touch
Display
Buzzer
Wi-Fi connection
UDP reception
Presence prompt
NFC UID acquisition
Profile mapping
Identity Package generation
```

Example log:

```text
UDP RX:
presence_event

Presence event received

NFC UID acquired

NFC mapped profile:
Student / learner

identity_package generated
```

---

# Issues Found and Resolved

## Issue 1 — UDP Sent Before Wi-Fi IP

Observed:

```text
Presence detected
UDP send failed
Wi-Fi IP obtained later
```

Cause:

```text
Presence event generated before DHCP completion.
```

Resolution:

```text
Deferred UDP transmission until IP acquisition.
```

---

## Issue 2 — ToF Regression During Intermediate Attempt

Observed:

```text
VL53L0X initialization instability
I2C NACK bursts
Distance read failures
```

Cause:

```text
Integration altered the validated boot sequence.
```

Resolution:

```text
Discard modified version.
Return to Git-restored baseline.
Apply network layer only.
```

Result:

```text
VL53L0X functionality restored.
```

---

## Issue 3 — Identity Node Build Failure

Observed:

```text
esp_wifi.h not found
```

Cause:

```text
Network components were not declared in main/CMakeLists.txt.
```

Resolution:

Added required ESP-IDF components:

```text
esp_wifi
esp_event
esp_netif
nvs_flash
```

to:

```text
firmware/nodes/identity-node/main/CMakeLists.txt
```

---

# Files Modified

```text
firmware/nodes/presence-node-v1/main/main.cpp

firmware/nodes/identity-node/main/main.cpp

firmware/nodes/identity-node/main/CMakeLists.txt
```

---

# Design Rules Confirmed

This integration confirmed the following engineering rules:

```text
Preserve validated baselines.
Add communication without altering internal node logic.
Avoid rollback to minimal firmware.
Avoid unnecessary refactoring.
Validate integration on real hardware.
```

---

# Current Architecture Status

```text
Presence Layer ............ VALIDATED
Identity Layer ............ VALIDATED
Presence → Identity ....... VALIDATED
Cognitive Layer ........... NEXT
Ambient Layer ............. ACTIVE DEVELOPMENT
Expression Layer .......... FUTURE INTEGRATION
```

---

# Next Step

The next architectural milestone is:

```text
Identity Package
↓
AX630C Cognitive Runtime
```

The intended flow is:

```text
Presence Event
↓
Identity Package
↓
Context Injection
↓
AX630C Inference
↓
Decision
```

---

# Final Result

The first real inter-layer communication path of Ambient Physical AI was successfully validated.

```text
Presence Layer
→
Identity Layer
```

Status:

```text
SUCCESSFULLY VALIDATED ON REAL HARDWARE
```
