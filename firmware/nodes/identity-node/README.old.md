# Identity Layer — Identity Node V1

## Ambient Physical AI

### Contextual Identity Gateway

The **Identity Node V1** is the identity gateway of the Ambient Physical AI ecosystem.

It receives a semantic presence trigger, prompts the user to present an NFC card, resolves the card UID to a local profile, combines that profile with the selected context and sends an `identity_package` to the Cognitive Runtime Node.

Official hardware:

```text
M5Stack M5Dial V1.1
+
WS1850S NFC reader
+
ESP-IDF
+
FreeRTOS
+
Wi-Fi
+
UDP
```

---

# Project Status

```text
Identity Node V1
OFFICIAL BASELINE
VALIDATED
```

Current milestone:

```text
IDENTITY_NODE_V1_MILESTONE_006
Presence-triggered NFC identity flow validated
```

Validation date:

```text
2026-07-17
```

Validated flow:

```text
Presence Node
192.168.77.19
      ↓ UDP unicast :3333
Identity Node
192.168.77.7
      ↓
Presence event validation
      ↓
“Presence detected”
“Tap NFC card”
      ↓
NFC UID
      ↓
Profile + Context
      ↓
Identity Package
      ↓ UDP :4444
AX630C Cognitive Runtime
```

Result:

```text
PASS
```

---

# Responsibilities

The Identity Node provides:

- reception of `presence_event`;
- user prompt for NFC identification;
- WS1850S NFC polling;
- UID acquisition;
- UID-to-profile mapping;
- profile visualization;
- context selection through the M5Dial encoder;
- touch and buzzer interaction;
- generation of the semantic `identity_package`;
- UDP transmission to the AX630C Cognitive Runtime.

The node does not perform LLM inference or ambient decision-making.

---

# Hardware

## Processing and Interface

```text
M5Stack M5Dial V1.1
ESP32-S3
Round display
Rotary encoder
Touch input
Buzzer
```

## NFC

```text
WS1850S
I²C address: 0x28
Validated VersionReg: 0x15
```

Validated UIDs:

| UID | Profile | Role |
|---|---|---|
| `8804DC32` | Claudio | owner |
| `88048667` | Student | learner |
| other UID | Unknown | visitor |

---

# FreeRTOS Architecture

The stabilized runtime separates responsibilities:

```text
UDP Listener Task
        ↓
Identity Event Queue
        ↓
UI Task
```

```text
NFC Task
        ↓
Identity Event Queue
        ↓
UI Task
```

Main ownership rules:

- **UI Task** owns `M5.update()`, display, touch, encoder and buzzer behavior;
- **NFC Task** owns WS1850S initialization, polling, UID reading and recovery;
- **UDP Listener Task** owns the Presence Layer receive socket;
- **Identity Event Queue** transfers semantic events safely to the UI;
- **I²C mutex** protects short shared-bus transactions.

This architecture avoids returning to a monolithic polling loop.

---

# Repository Structure

```text
firmware/nodes/identity-node/
├── CMakeLists.txt
├── README.md
├── main/
│   ├── CMakeLists.txt
│   └── main.cpp
├── components/
│   └── ws1850s/
├── managed_components/
└── sdkconfig
```

Generated directories such as `build/` must not be committed.

---

# Presence Event Listener

## Network Configuration

Validated Identity Node address:

```text
192.168.77.7
```

Listener:

```text
Bind address: 0.0.0.0
UDP port:    3333
```

The listener waits for Wi-Fi readiness before creating and binding the socket.

Validated startup sequence:

```text
UDP listener task started
UDP listener waiting for Wi-Fi...
Wi-Fi connected. IP: 192.168.77.7
Wi-Fi ready; creating UDP listener
UDP listener ready on 0.0.0.0:3333
UDP listener waiting for packet...
```

## Accepted Semantic Contract

```json
{
  "type": "presence_event",
  "state": "PRESENT",
  "distance_mm": 1950,
  "source": "presence_node_v1"
}
```

The current filter validates:

```text
type   = presence_event
state  = PRESENT
source = presence_node_v1
```

## Validated Reception

```text
UDP RX: 90 bytes from 192.168.77.19:55876
UDP payload: {"type":"presence_event","state":"PRESENT","distance_mm":1950,"source":"presence_node_v1"}
Valid presence_event received
Presence event received: show NFC prompt
```

---

# Presence Prompt

After a valid Presence Layer event, the UI displays:

```text
Presence detected
Tap NFC card
```

The prompt remains active temporarily and emits a short buzzer tone.

If no NFC card is presented during the prompt window, the UI returns to the main Identity Console.

---

# NFC Runtime

The NFC state machine includes:

```text
NFC_INIT
NFC_IDLE
NFC_POLL
NFC_READ_UID
NFC_CARD_PRESENT
NFC_CARD_REMOVED
NFC_ERROR
NFC_COOLDOWN
```

Validated behavior:

- WS1850S initialization;
- VersionReg validation;
- card polling;
- UID anti-collision read;
- card-present confirmation;
- card-removal confirmation;
- bounded retries;
- I²C recovery and polling backoff;
- mapping of known UIDs;
- unknown-profile fallback.

`ESP_ERR_NOT_FOUND` during UID acquisition is treated as card absence/removal rather than a critical hardware failure.

---

# Context Selection

Available contexts:

```text
Lab
Classroom
Demo
Meeting
```

The encoder selects the active context.

The selected context is combined with the NFC profile when generating the `identity_package`.

---

# Identity Package

Example:

```json
{
  "type": "identity_package",
  "profile": {
    "id": "claudio",
    "name": "Claudio",
    "role": "owner"
  },
  "context": "Lab",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "8804DC32"
  },
  "source": "m5dial_identity_console_v1"
}
```

Destination:

```text
AX630C Cognitive Runtime
IP: 192.168.77.15
UDP port: 4444
```

Validated profiles:

```text
Claudio / owner
Student / learner
Unknown / visitor
```

---

# Identity Visualization

After a UID is mapped, the display temporarily presents:

- profile initial;
- profile name;
- role;
- selected context;
- UID.

The UI then returns to the Identity Console.

---

# End-to-End Architecture

```text
Human enters the environment
        ↓
Presence Node V2
AtomS3 Lite + LD2410C
        ↓
presence_event
UDP :3333
        ↓
Identity Node V1
M5Dial + WS1850S
        ↓
NFC UID
        ↓
Profile resolution
        ↓
Context selection
        ↓
identity_package
UDP :4444
        ↓
AX630C Cognitive Runtime
```

---

# Build Requirements

Validated toolchain:

```text
ESP-IDF 5.4.2
Target: esp32s3
CMake
Ninja
Git
```

The M5Dial requires the ESP32-S3 target.

From the Identity Node directory:

```bash
cd firmware/nodes/identity-node
idf.py set-target esp32s3
idf.py build
```

For a clean build:

```bash
idf.py fullclean
idf.py set-target esp32s3
idf.py build
```

Confirm:

```text
CONFIG_IDF_TARGET="esp32s3"
```

Do not modify the M5Dial component GPIO definitions to work around a wrong build target. GPIOs 40, 41 and 42 are valid on the ESP32-S3.

---

# Flash and Monitor

```bash
idf.py -p COMx flash monitor
```

Exit:

```text
Ctrl + ]
```

---

# Expected Boot Evidence

```text
Knob encoder initialized
NFC state: INIT
NFC/WS1850S VersionReg: 0x15
NFC/WS1850S initialized at 0x28
NFC recovered: NFC ready
UDP listener waiting for Wi-Fi...
Wi-Fi connected. IP: 192.168.77.7
Wi-Fi ready; creating UDP listener
UDP listener ready on 0.0.0.0:3333
UDP listener waiting for packet...
```

---

# Validation Checklist

## Hardware and UI

```text
Display initialization .............. PASS
Touch input ......................... PASS
Encoder input ....................... PASS
Buzzer output ....................... PASS
```

## NFC

```text
WS1850S initialization .............. PASS
VersionReg 0x15 ..................... PASS
UID 8804DC32 ........................ PASS
UID 88048667 ........................ PASS
Known-profile mapping ............... PASS
Unknown-profile fallback ............ PASS
Recovery/backoff behavior ........... PASS
```

## Presence Reception

```text
Wi-Fi connection .................... PASS
Listener waits for Wi-Fi ............ PASS
Socket creation ..................... PASS
Bind 0.0.0.0:3333 ................... PASS
UDP unicast reception ............... PASS
Payload logging ..................... PASS
Semantic payload validation ......... PASS
UI prompt dispatch .................. PASS
```

## Cognitive Runtime Output

```text
Identity Package generation ......... PASS
UDP transmission to AX630C :4444 .... PASS
```

---

# Validated Integration Evidence

Presence Node:

```text
PRESENT
UDP presence_event sent: {"type":"presence_event","state":"PRESENT","distance_mm":1950,"source":"presence_node_v1"}
```

Identity Node:

```text
UDP RX: 90 bytes from 192.168.77.19:55876
UDP payload: {"type":"presence_event","state":"PRESENT","distance_mm":1950,"source":"presence_node_v1"}
Valid presence_event received
Presence event received: show NFC prompt
```

This evidence confirms:

```text
Presence detection ................. PASS
UDP delivery ....................... PASS
Identity listener .................. PASS
Semantic validation ................ PASS
UI event queue ..................... PASS
NFC prompt ......................... PASS
```

---

# NFC Identity Strategy

The final project strategy supports two identity-resolution paths:

```text
NFC card detected
      ↓
NDEF payload available?
   ├── YES → read NDEF/JSON profile data
   └── NO  → use UID Mapping
```

The current validated firmware baseline uses UID Mapping.

NDEF/JSON resolution remains a planned compatibility path and must not replace or break the validated UID fallback.

---

# Current Scope

Implemented:

- M5Dial UI runtime;
- FreeRTOS task separation;
- Identity Event Queue;
- encoder context selection;
- WS1850S NFC;
- UID mapping;
- identity visualization;
- Presence Layer UDP listener;
- Presence-triggered NFC prompt;
- Identity Package generation;
- AX630C UDP transmission.

Deferred or outside this milestone:

- full NDEF/JSON parsing;
- MQTT;
- direct StackFlow orchestration on the M5Dial;
- camera-based identity;
- LLM inference;
- redesign of the Identity Node architecture.

---

# Security and Configuration Note

Wi-Fi credentials and fixed device IP addresses are currently defined in source code for laboratory validation.

Before public release:

- remove private credentials from tracked files;
- use a configuration mechanism;
- document the required network parameters;
- preserve the semantic UDP contracts.

---

# Final Status

```text
IDENTITY NODE V1                        VALIDATED
WS1850S NFC RUNTIME                     VALIDATED
PRESENCE EVENT LISTENER                 VALIDATED
PRESENCE-TRIGGERED NFC PROMPT           VALIDATED
IDENTITY PACKAGE TO AX630C              VALIDATED
PRESENCE → IDENTITY INTEGRATION         VALIDATED
```
