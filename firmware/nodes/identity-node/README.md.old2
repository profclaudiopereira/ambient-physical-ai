# Identity Node V1

## Ambient Physical AI

### Identity Layer Runtime Node

The Identity Node is responsible for authenticating users in the Ambient Physical AI ecosystem.

It receives presence notifications from the Presence Node, requests user identification through NFC, associates the detected card with a local user profile, generates an Identity Package and sends it to the Cognitive Runtime.

---

# Purpose

The Identity Node provides:

```text
Presence Event
        ↓
NFC Authentication
        ↓
Profile Identification
        ↓
Active Context
        ↓
Identity Package
        ↓
Cognitive Runtime
```

The node is intentionally focused on identity management and user context.

---

# Current Status

```text
Identity Node V1
VALIDATED
```

Current milestone:

```text
IDENTITY_NODE_V1_MILESTONE_009

• Presence Integration
• NFC Authentication
• Profile Mapping
• Context Selection
• Identity Package
• UDP Communication
• Circular UI V1
```

---

# Hardware Platform

```text
M5Stack M5Dial V1.1
```

Main peripherals:

- ESP32-S3
- Circular Touch Display
- Rotary Encoder
- Speaker
- WS1850S NFC Reader

---

# Software Architecture

Main components:

- FreeRTOS Tasks
- Identity Event Queue
- NFC Manager
- Profile Manager
- Context Manager
- UDP Communication
- User Interface

---

# Identity Flow

```text
Presence Node
        │
        ▼
Presence Event
        │
        ▼
Show NFC Prompt
        │
        ▼
Read NFC Card
        │
        ▼
UID Mapping
        │
        ▼
Profile Selection
        │
        ▼
Generate Identity Package
        │
        ▼
Send UDP
        │
        ▼
AX630C Cognitive Runtime
```

---

# Supported Profiles

Current built-in profiles:

```text
Unknown
Claudio
Student
```

Unknown cards are handled safely as visitor profiles.

---

# Available Contexts

Official Version 1 contexts:

```text
Lab
Research
Classroom
Demo
Meeting
```

Contexts are selected locally using the rotary encoder.

The current implementation separates:

- Protocol identifier (`protocol_name`)
- Display name (`display_name`)

This allows future UI localization without changing the communication protocol.

---

# Identity Package

The Identity Package is transmitted to the Cognitive Runtime using UDP.

Example:

```json
{
  "type":"identity_package",
  "profile":{
    "id":"claudio",
    "name":"Claudio",
    "role":"owner"
  },
  "context":"Research",
  "nfc":{
    "detected":true,
    "card_present":true,
    "uid":"8804DC32"
  },
  "source":"m5dial_identity_console_v1"
}
```

---

# User Interface

Current interface provides:

- Active profile
- User role
- Active context
- Node status
- Presence prompt
- Identity visualization

The interface is optimized for the circular display of the M5Dial.

---

# Build

```bash
idf.py build
```

---

# Flash

```bash
idf.py flash monitor
```

---

# Validation Checklist

The following items have been validated:

- NFC detection
- UID acquisition
- Profile mapping
- Presence integration
- Context selection
- Identity Package generation
- UDP transmission
- AX630C communication
- Circular UI

---

# Repository Structure

```text
identity-node/
├── main/
├── components/
├── CMakeLists.txt
├── sdkconfig
└── README.md
```

---

# Related Components

This node communicates with:

```text
Presence Node
        │
        ▼
Identity Node
        │
        ▼
Cognitive Runtime (AX630C)
        │
        ▼
Ambient Runtime
        │
        ▼
Expression Layer
```

---

# Documentation

Additional engineering documentation is available in the project's `docs/` directory, including:

- System Architecture
- Engineering Decisions
- Integration Notes
- Discovery Reports

---

# Project

Ambient Physical AI

Distributed Cognitive Ecosystem Powered by StackFlow

M5Stack Global Innovation Contest 2026