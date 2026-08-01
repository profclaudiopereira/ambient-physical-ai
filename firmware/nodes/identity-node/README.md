# Identity Node V1

## Ambient Physical AI

### Identity Layer Runtime Node

The **Identity Node** is responsible for transforming a physical identifier into a semantic digital identity within the Ambient Physical AI ecosystem.

It bridges the physical and cognitive worlds by identifying users, selecting the active operational context, presenting identity locally on the M5Dial interface, and generating standardized Identity Packages consumed by the Cognitive Runtime.

---

# Project Status

```text
Identity Node V1

VALIDATED
```

Current milestone:

```text
IDENTITY_NODE_V1_MILESTONE_002

Identity visualization with profile images validated.
```

---

# Mission

The Identity Node is responsible for:

- Detecting NFC cards
- Resolving user identities
- Selecting the active operational context
- Presenting identity locally
- Producing standardized Identity Packages

---

# Hardware

Validated hardware platform:

```text
M5Dial V1.1

ESP32-S3
↓
Round IPS Display
↓
Rotary Encoder
↓
Capacitive Touch
↓
Speaker
↓
WS1850S NFC Reader
```

---

# Runtime Overview

```text
Presence Node
      │
presence_event
      │
      ▼
Identity Node
      │
Identity Package (UDP)
      │
      ▼
Cognitive Runtime (AX630C)
```

The Identity Node acts as the boundary between physical identification and semantic processing.

---

# Software Organization

The firmware is organized around independent FreeRTOS tasks.

## UI Task

Responsible for:

- display
- touch
- rotary encoder
- buzzer
- profile visualization
- context visualization

## NFC Task

Responsible for:

- WS1850S communication
- card polling
- UID acquisition
- recovery
- event generation

Communication between both tasks occurs through an Identity Event Queue, isolating the graphical interface from NFC timing requirements.

---

# Identity Flow

```text
Presence detected
        │
        ▼
Tap NFC Card
        │
        ▼
Read UID
        │
        ▼
Resolve Profile
        │
        ▼
Associate Context
        │
        ▼
Display Identity
        │
        ▼
Generate Identity Package
        │
        ▼
AX630C
```

---

# User Interface

The current interface includes:

- Identity Console
- Presence Prompt
- Identity Visualization

The visualization screen displays:

- profile image
- user name
- role
- active context
- NFC UID

---

# Profile Images

Profile image management is implemented through the `ProfileImageManager` abstraction.

The user interface never accesses image assets directly. Instead it requests rendering through a single interface:

```cpp
ProfileImageManager::drawProfile(profile.id, x, y);
```

Additional implementation details are documented in:

```text
main/README.md
```

---

# Supported Profiles

Validated profiles:

- Claudio
- Hermínio
- Mariana
- Student
- Unknown

Each profile is associated with an independent NFC identifier.

---

# Context Selection

The rotary encoder selects the current operating context.

Validated contexts:

- Lab
- Research
- Classroom
- Demo
- Meeting

The selected context becomes part of every generated Identity Package.

---

# Presence Integration

The Identity Node continuously listens for UDP `presence_event` messages.

When presence is detected, the interface requests the user to authenticate before identity acquisition begins.

---

# Identity Package

After successful authentication the node generates a standardized JSON package containing:

- profile
- role
- context
- UID
- source

The package is transmitted to the Cognitive Runtime over UDP.

---

# Documentation

Project documentation is organized hierarchically.

```text
identity-node/

README.md

components/
    ws1850s/
        README.md

main/
    README.md
```

- **README.md** — Identity Layer overview.
- **components/ws1850s/README.md** — reusable NFC reader component.
- **main/README.md** — profile image management and graphical assets.

---

# Repository Structure

```text
identity-node/

components/
    ws1850s/

main/
    main.cpp
    profile_image_manager.cpp
    profile_image_manager.h

    profile_images/
```

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

# Validation Status

Validated features:

- FreeRTOS runtime
- Shared I²C protection
- WS1850S communication
- UID mapping
- Five user profiles
- Context selection
- Identity visualization
- Profile images
- Presence integration
- UDP Identity Package
- AX630C integration

```text
Identity Layer

VALIDATED
```

---

# Future Work

The current implementation intentionally stores profile images as embedded RGB565 assets.

The existing `ProfileImageManager` abstraction allows future migration to synchronized profile assets without changing the user interface.

Planned evolution includes:

- centralized profile synchronization;
- NDEF-based identity resolution;
- runtime-managed profile assets.

These items are future work and are **not** part of the validated implementation.

---

# Identity Package V1.1

The current implementation produces **Identity Package Specification V1.1**.

Enhancements include:

- `contract_version`
- `current_context`
- backward compatibility with `context`

The complete end-to-end pipeline has been validated:

```text
Identity Node
        ↓
Identity Package V1.1
        ↓
Identity UDP Listener
        ↓
Context Builder
        ↓
Context Package
        ↓
Cognitive Runtime
```

Both V1.0 and V1.1 contracts remain supported, providing backward compatibility while establishing V1.1 as the stable interface for the Ambient Physical AI Identity Layer.
