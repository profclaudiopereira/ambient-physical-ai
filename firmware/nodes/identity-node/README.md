# Identity Node V1

## Ambient Physical AI

### Identity Layer Runtime Node

The Identity Node is responsible for transforming a physical identifier into a semantic digital identity inside the Ambient Physical AI ecosystem.

This node bridges the physical world and the cognitive runtime by recognizing users, selecting the active operational context and producing standardized identity packages for the remaining distributed system.

---

# Mission

The Identity Node performs five primary responsibilities:

- Detect NFC cards
- Resolve user identity
- Select operational context
- Generate identity packages
- Present identity through a graphical interface

---

# Current Project Status

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

# Hardware

Current validated hardware:

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

# Runtime Architecture

```
                 Presence Node
                       │
             UDP presence_event
                       │
                       ▼
              Identity Node
                       │
          ┌────────────┴────────────┐
          │                         │
    Identity Visualization    Identity Package
          │                         │
          └────────────┬────────────┘
                       │
                 UDP JSON
                       │
                       ▼
           Cognitive Runtime (AX630C)
```

---

# Software Architecture

The implementation is organized around two independent FreeRTOS tasks.

## UI Task

Responsible for:

- display
- touch
- encoder
- buzzer
- context visualization
- profile visualization

## NFC Task

Responsible for:

- WS1850S communication
- card polling
- UID reading
- recovery
- event generation

Both tasks communicate through an Identity Event Queue, keeping the user interface isolated from NFC timing requirements. This separation is reflected directly in the current implementation. :contentReference[oaicite:2]{index=2}

---

# Identity Flow

```
Presence detected
        │
        ▼
Tap NFC Card
        │
        ▼
Read UID
        │
        ▼
Profile Resolution
        │
        ▼
Context Association
        │
        ▼
Identity Visualization
        │
        ▼
Identity Package
        │
        ▼
AX630C
```

---

# User Interface

The Identity Node currently provides three operating screens:

- Identity Console
- Presence Prompt
- Identity Visualization

The visualization screen presents:

- profile image
- user name
- role
- active context
- NFC UID

---

# Profile Image Manager

Version 1 introduces a dedicated abstraction named:

```text
ProfileImageManager
```

The graphical interface never accesses image data directly.

Instead it simply requests:

```cpp
ProfileImageManager::drawProfile(profile.id, x, y);
```

Current implementation stores embedded RGB565 avatars.

This abstraction intentionally prepares the project for future synchronized profile storage without modifying the user interface.

---

# Current Profiles

Validated profiles:

- Claudio
- Hermínio
- Mariana
- Student
- Unknown

Each profile is mapped to an independent NFC card and rendered with its own avatar.

---

# Context Selection

The rotary encoder selects the active operating context.

Current contexts include:

- Lab
- Research
- Classroom
- Demo
- Meeting

The selected context becomes part of every generated identity package.

---

# Presence Integration

The Identity Node continuously listens for UDP `presence_event` messages.

When presence is detected:

```
Presence detected
        ↓
Tap NFC card
```

is presented to the user before identity acquisition begins. :contentReference[oaicite:3]{index=3}

---

# Identity Package

After successful identification the node generates a JSON identity package containing:

- profile
- role
- context
- UID
- source

The package is transmitted to the Cognitive Runtime over UDP.

---

# Current Engineering Status

Validated:

- FreeRTOS runtime
- Shared I2C protection
- NFC communication
- UID mapping
- Five user profiles
- Context selection
- Identity visualization
- Profile images
- UDP communication
- Presence integration
- Identity Package generation

---

# Current Limitations

Current implementation intentionally uses embedded RGB565 profile images.

This decision was adopted to validate the complete identity pipeline before implementing centralized profile synchronization.

The future implementation will preserve the same ProfileImageManager abstraction while replacing embedded images with synchronized profile assets provided by the Cognitive Runtime.

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
        claudio.h
        herminio.h
        mariana.h
        student.h
        unknown.h
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

```text
Identity Layer

VALIDATED
```

Current validated features:

✔ NFC

✔ Five mapped profiles

✔ Context selection

✔ Identity visualization

✔ Profile avatars

✔ Presence integration

✔ UDP identity package

✔ AX630C integration interface

---

# Future Evolution

The current implementation validates the complete Identity Layer while keeping the architecture compatible with future profile synchronization, NDEF-based identities and centralized profile management provided by the Cognitive Runtime.