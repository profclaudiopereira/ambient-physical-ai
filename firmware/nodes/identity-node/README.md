# Identity Node V1

## Ambient Physical AI

### Identity Layer Runtime Node

The Identity Node V1 is responsible for detecting, identifying and contextualizing users within the Ambient Physical AI ecosystem.

It transforms physical identity interactions (NFC cards) into structured identity events that can be consumed by higher cognitive layers.

The current implementation is based on M5Dial V1.1 running ESP-IDF and FreeRTOS.

---

# Purpose

The Identity Node provides:

```text
Physical Identity
↓
Profile Resolution
↓
Context Association
↓
Identity Package
↓
Future Cognitive Runtime
```

The node is designed to act as the primary identity gateway of the Ambient Physical AI architecture.

---

# Project Status

Current status:

```text
Identity Node V1
OPERATIONAL BASELINE
```

Runtime architecture stabilized.

Core functionality validated.

Suitable for continued development.

---

# Hardware

## Main Device

```text
M5Dial V1.1
ESP32-S3
```

## Integrated Components

```text
Touch Display
Rotary Encoder
Buzzer
WS1850S NFC Controller
```

---

# Software Stack

## Framework

```text
ESP-IDF v5.4.2
```

## Runtime

```text
FreeRTOS
```

## Architecture

```text
UI Task
+
NFC Task
+
Identity Event Queue
```

---

# Architecture Overview

## UI Task

Responsible for:

```text
M5.update()
Display
Touch
Encoder
Buzzer
Context Selector
Profile Visualization
Identity Visualization
```

---

## NFC Task

Responsible for:

```text
WS1850S Initialization
NFC Polling
Card Detection
UID Acquisition
UID Mapping
Recovery Logic
```

---

## Identity Event Queue

Communication flow:

```text
NFC Task
↓
Identity Event Queue
↓
UI Task
```

---

# Identity Pipeline

```text
NFC Card
↓
UID
↓
Profile Mapping
↓
Identity Package
↓
Identity Visualization
```

---

# Supported Profiles

## Claudio

```text
UID:
8804DC32

Profile:
Claudio

Role:
owner
```

---

## Student

```text
UID:
88048667

Profile:
Student

Role:
learner
```

---

## Unknown

Any non-mapped UID is treated as:

```text
Unknown
visitor
```

---

# Context Selection

Supported contexts:

```text
Lab
Meeting
Classroom
Demo
```

The active context becomes part of the generated Identity Package.

---

# Identity Package Format

Example:

```json
{
  "type":"identity_package",
  "profile":{
    "id":"claudio",
    "name":"Claudio",
    "role":"owner"
  },
  "context":"Lab",
  "nfc":{
    "detected":true,
    "card_present":true,
    "uid":"8804DC32"
  },
  "source":"m5dial_identity_console_v1"
}
```

---

# Identity Visualization

The Identity Console V1 includes a visual identity presentation layer.

Upon successful card detection:

```text
UID
↓
Profile Mapping
↓
Full Screen Identity View
↓
Return to Main Screen
```

Displayed information:

```text
Profile Name
Role
Context
UID
```

Profiles:

```text
Claudio / owner
Student / learner
Unknown / visitor
```

---

# Validated Features

## Runtime

```text
Boot
FreeRTOS Startup
Task Scheduling
Event Queue
Recovery Logic
```

Status:

PASS

---

## User Interface

```text
Display
Touch
Encoder
Buzzer
Context Selection
Profile Selection
Identity Visualization
```

Status:

PASS

---

## NFC

```text
WS1850S Detection
Version Register Read
Card Detection
UID Reading
UID Mapping
Unknown UID Handling
Card Removal Detection
```

Status:

PASS

---

## Identity

```text
Identity Package Generation
Identity Visualization
Context Integration
Profile Mapping
```

Status:

PASS

---

# Known Issues

Occasional errors may still appear:

```text
ESP_ERR_INVALID_STATE

I2C transaction failed
```

Observed behavior:

```text
NFC polling
↓
I2C error
↓
Recovery
↓
WS1850S Reinitialization
↓
Normal operation resumes
```

Current classification:

```text
NON-BLOCKING
```

The issue is documented in:

```text
docs/notes/LAB_WS1850S_001_DRIVER_RELIABILITY.md
```

---

# Build

```bash
idf.py build
```

---

# Flash

```bash
idf.py flash
```

---

# Monitor

```bash
idf.py monitor
```

or

```bash
idf.py flash monitor
```

---

# Validation Procedure

## Basic Runtime

Verify:

```text
Display
Touch
Encoder
Buzzer
```

---

## NFC

Present:

```text
8804DC32
```

Expected:

```text
Claudio / owner
```

---

Present:

```text
88048667
```

Expected:

```text
Student / learner
```

---

Present an unknown card.

Expected:

```text
Unknown / visitor
```

---

Remove card.

Expected:

```text
NFC card stable: NO
```

---

# Future Work

## Identity Layer

Potential improvements:

```text
Profile Images
Avatar Rendering
NDEF Support
Extended Identity Registry
```

---

## Reliability

See:

```text
LAB_WS1850S_001_DRIVER_RELIABILITY.md
```

for ongoing NFC driver investigation.

---

## Presence Layer Integration

Future integration candidates:

```text
VL53L0X ToF
Presence Node
Context Triggering
Ambient Runtime Interaction
```

---

# Repository Position

```text
Ambient Physical AI
└── Identity Layer
    └── Identity Node V1
```

Current maturity:

```text
Operational Baseline
```

Ready for continued development and integration with future Ambient Physical AI cognitive and contextual layers.
