# Echo Pyramid Voice Node

## Ambient Physical AI

### Expression Layer — Voice Interaction

The **Echo Pyramid Voice Node** is the voice interaction endpoint of the Ambient Physical AI ecosystem.

It is responsible for providing bidirectional voice communication between users and the Cognitive Runtime while preserving the distributed architecture of the project.

This node executes locally on the **M5Stack Echo Pyramid (AtomS3R)** and combines:

- local audio capture;
- wake word detection;
- constrained voice command recognition;
- audio playback;
- local operational display;
- network communication with the Cognitive Runtime.

The node intentionally performs only voice interaction responsibilities.

It does **not** perform cognitive reasoning, identity management, authorization, context selection, or decision making.

Those responsibilities remain under the authority of the Cognitive Runtime.

---

# Project Status

```text
Expression Layer

Voice Interaction

STATUS

VALIDATED
```

Current validated capabilities include:

- Local Status Display V1
- Wi-Fi communication
- TCP audio reception
- PCM audio playback
- Wake Word detection
- MultiNet command recognition
- Context change request generation
- Bidirectional integration with the Cognitive Runtime

Features under active engineering remain clearly identified throughout this documentation.

---

# Purpose

The purpose of the Echo Pyramid Voice Node is to provide a natural voice interface between users and the Ambient Physical AI ecosystem.

The node operates in both directions.

## Incoming Voice

The Cognitive Runtime may transmit speech responses that are rendered locally through the Echo Pyramid loudspeaker.

```text
Cognitive Runtime
        │
        ▼
TCP PCM Stream
        │
        ▼
Echo Pyramid
        │
        ▼
Audio Playback
```

---

## Outgoing Voice

Users interact with the environment using a wake word followed by a constrained voice command.

```text
User

↓

Wake Word

↓

Voice Command

↓

Context Change Request

↓

Cognitive Runtime
```

The node never changes the active context locally.

It only requests a context transition.

The Cognitive Runtime validates the request, applies authorization rules, preserves the active identity, updates the global system state, and distributes the resulting semantic event to the remaining nodes.

---

# Architectural Position

Within the Ambient Physical AI architecture, the Echo Pyramid Voice Node belongs exclusively to the **Expression Layer**.

```text
Expression Layer

│

├── Voice Interaction
│       Echo Pyramid Voice Node
│
├── Runtime State Visualization
│       StickC Plus2 RGB Node
│
└── Ambient Lighting
        ├── Atom Matrix RGB Node
        └── RGB Strip Node
```

Each node has a single architectural responsibility.

The Echo Pyramid Voice Node is exclusively responsible for **voice interaction**.

---

# Hardware Platform

Validated hardware configuration:

```text
M5Stack Echo Pyramid

AtomS3R

ES7210
Microphone ADC

ES8311
Audio Codec

AW87559
Power Amplifier

SI5351
Clock Generator

Integrated Speaker

Integrated Microphone Array

Integrated Display
```

The firmware uses the native ESP-IDF SDK and integrates a vendor library adapted for ESP-IDF operation while preserving its original architecture whenever possible.

---

# Responsibilities

The Echo Pyramid Voice Node is responsible for:

- capturing voice input;
- detecting the wake word;
- recognizing predefined context commands;
- generating context change requests;
- receiving PCM audio streams;
- reproducing speech locally;
- presenting operational status through the integrated display;
- maintaining network connectivity required for voice interaction.

The node intentionally does **not** perform:

- user authentication;
- identity management;
- authorization;
- semantic reasoning;
- context ownership;
- dialogue management;
- LLM inference.

These responsibilities belong to the Cognitive Runtime.

---

# High-Level Architecture

The implementation follows a modular architecture composed of specialized components.

```text
                   Echo Pyramid Voice Node

                          app_main
                              │
      ┌──────────────┬─────────┼───────────────┬──────────────┐
      │              │         │               │              │
      ▼              ▼         ▼               ▼              ▼
 network        display    audio_bridge   voice_receiver  wake_word_detector
      │              │         │               │              │
      │         lcd_driver     │               │              │
      └──────────────┴─────────┴───────────────┴──────────────┘
                              │
                      M5Echo-Pyramid Library
                              │
                  Audio Codec • I²S • Microphones
```

Each component owns a single primary responsibility and communicates with the remaining system through small, well-defined interfaces.

This organization minimizes coupling, simplifies maintenance, and allows each subsystem to evolve independently without affecting validated runtime contracts.

---

# Voice Interaction Overview

The firmware supports bidirectional voice communication.

## Speech Playback

```text
Cognitive Runtime
        │
        ▼
TCP PCM Stream
        │
        ▼
Voice Receiver
        │
        ▼
Audio Bridge
        │
        ▼
Speaker
```

---

## Voice Commands

```text
Microphones
        │
        ▼
Audio Bridge
        │
        ▼
Wake Word Detector
        │
        ▼
Recognized Command
        │
        ▼
Context Change Request
        │
        ▼
Cognitive Runtime
```

This architecture preserves the Cognitive Runtime as the single authority responsible for semantic interpretation and global context management.

---

# Repository Organization

```text
echo-pyramid-voice-node/

├── README.md
├── main/
├── components/
├── partitions.csv
└── CMakeLists.txt

Project Documentation

docs/
└── notes/
```

The root README provides the architectural overview of the firmware.

Implementation details remain close to the corresponding source modules, while engineering investigations and design rationale are documented separately through Technical Notes.

---

# Software Architecture

The Echo Pyramid Voice Node follows a modular software architecture in which each component owns a single primary responsibility.

The application layer coordinates component initialization, while individual modules encapsulate networking, display management, audio hardware, voice reception and speech recognition.

This organization minimizes coupling, simplifies testing, and allows components to evolve independently without modifying validated runtime contracts.

---

# Software Components

```text
app_main
│
├── network_config
├── display_manager
├── audio_bridge
├── voice_receiver
├── wake_word_detector
└── M5Echo-Pyramid (vendor library)
```

Each component exposes a small public interface while hiding implementation details internally.

---

# Main Application

The application entry point is responsible only for orchestrating the firmware lifecycle.

Its responsibilities include:

- hardware initialization;
- network initialization;
- display initialization;
- audio subsystem startup;
- voice receiver startup;
- wake word detector startup;
- callback registration;
- runtime heartbeat.

The main application intentionally avoids implementing business logic that belongs to specialized components.

---

# Network Component

```text
network_config
```

The network component manages all Wi-Fi connectivity required by the node.

Responsibilities include:

- Wi-Fi Station initialization;
- network event registration;
- connection management;
- automatic reconnection;
- IP address acquisition;
- connection status reporting.

The remainder of the firmware interacts with the network only through the component's public interface.

---

# Local Display

```text
display_manager
```

The Display Manager provides the local operational user interface.

Its responsibility is to translate application states into visual information presented on the integrated display.

Current validated display states include:

```text
BOOT

READY

WELCOME

ERROR
```

The component contains no LCD hardware knowledge.

Rendering operations are delegated to the LCD Driver.

---

# LCD Driver

```text
lcd_driver
```

The LCD Driver implements the low-level display interface.

Responsibilities include:

- LCD initialization;
- backlight control;
- screen clearing;
- rectangle rendering;
- text rendering;
- font management;
- coordinate handling.

The driver encapsulates the complete hardware interface, allowing higher software layers to remain independent of display implementation details.

---

# Audio Bridge

```text
audio_bridge
```

The Audio Bridge provides a hardware abstraction layer for the Echo Pyramid audio subsystem.

It isolates the remaining firmware from:

- audio codec implementation;
- I²S configuration;
- microphone acquisition;
- speaker playback;
- vendor library details.

Its public interface provides:

```text
Initialization

PCM Recording

PCM Playback

Audio Diagnostic Test
```

The Audio Bridge also protects concurrent access to the audio subsystem through internal synchronization mechanisms.

---

# Voice Receiver

```text
voice_receiver
```

The Voice Receiver implements the incoming audio transport.

Its responsibilities include:

- TCP server initialization;
- APAI Voice Protocol validation;
- PCM stream reception;
- packet integrity verification;
- playback callback invocation.

The component does not perform audio playback directly.

Instead, validated PCM blocks are forwarded to the Audio Bridge through a callback interface.

This separation keeps transport independent from hardware implementation.

---

# Wake Word Detector

```text
wake_word_detector
```

The Wake Word Detector implements the complete local speech recognition pipeline.

Responsibilities include:

- microphone capture;
- Audio Front-End (AFE);
- WakeNet execution;
- MultiNet command recognition;
- command callback generation;
- listening state management.

The component intentionally remains independent of networking and Cognitive Runtime communication.

It never sends packets directly.

Instead, recognized commands are delivered to the application through callback interfaces.

---

# Vendor Library

```text
M5Echo-Pyramid
```

The firmware integrates the vendor-provided M5Echo-Pyramid library as the hardware interface for the Echo Pyramid platform.

The library provides access to:

- audio codec;
- microphone ADC;
- power amplifier;
- clock generator;
- peripheral controller;
- I²S audio interface.

Local engineering adaptations were introduced to improve integration with ESP-IDF while preserving the original architecture whenever possible.

The vendor library remains isolated from the application through the Audio Bridge.

---

# Firmware Initialization Flow

The firmware follows a deterministic startup sequence.

```text
Power On
      │
      ▼
Display Initialization
      │
      ▼
Audio Initialization
      │
      ▼
Network Initialization
      │
      ▼
Voice Receiver
      │
      ▼
Wake Word Detector
      │
      ▼
UDP Services
      │
      ▼
Operational State
```

Each subsystem reports initialization status before the next stage begins.

---

# Voice Playback Flow

Incoming speech generated by the Cognitive Runtime follows the path below.

```text
Cognitive Runtime
        │
        ▼
TCP PCM Stream
        │
        ▼
Voice Receiver
        │
        ▼
Playback Callback
        │
        ▼
Audio Bridge
        │
        ▼
M5Echo-Pyramid
        │
        ▼
Speaker
```

The transport layer and hardware layer remain fully independent.

---

# Voice Command Flow

Outgoing voice interaction follows a separate processing pipeline.

```text
Microphones
        │
        ▼
Audio Bridge
        │
        ▼
Wake Word Detector
        │
        ▼
WakeNet
        │
        ▼
MultiNet
        │
        ▼
Recognized Command
        │
        ▼
Context Change Request
        │
        ▼
Cognitive Runtime
```

The Echo Pyramid Voice Node does not execute semantic interpretation.

It only recognizes validated commands and requests a context transition.

The Cognitive Runtime remains responsible for authorization, context ownership, identity preservation and semantic decision making.

---

# Component Interaction

```text
                 app_main
                     │
      ┌──────────────┼──────────────┐
      │              │              │
      ▼              ▼              ▼
 Network       Display Manager  Audio Bridge
      │              │              │
      │         LCD Driver          │
      │              │              │
      └──────────────┼──────────────┘
                     │
        ┌────────────┴─────────────┐
        ▼                          ▼
 Voice Receiver          Wake Word Detector
        │                          │
        └────────────┬─────────────┘
                     ▼
            Cognitive Runtime
```

This modular organization provides clear ownership boundaries, preserves validated runtime contracts, and simplifies long-term maintenance of the Expression Layer.

---

# Engineering Decisions

The Echo Pyramid Voice Node was designed following the engineering principles adopted throughout the Ambient Physical AI project.

The implementation prioritizes:

- clear separation of responsibilities;
- low coupling between subsystems;
- reproducible engineering;
- modular software architecture;
- preservation of validated runtime contracts.

The firmware intentionally avoids concentrating multiple responsibilities inside the application entry point.

---

# Design Principles

The software architecture follows a component-oriented design.

Each module owns a single primary responsibility and exposes a compact public interface.

Examples include:

```text
network_config

↓

Network Management


display_manager

↓

Operational User Interface


audio_bridge

↓

Hardware Abstraction


voice_receiver

↓

Incoming Voice Transport


wake_word_detector

↓

Speech Recognition
```

This organization reduces maintenance cost while improving long-term scalability.

---

# Runtime Contracts

The Echo Pyramid Voice Node communicates with the Cognitive Runtime using validated runtime contracts.

The node never assumes ownership of:

- authenticated identity;
- semantic interpretation;
- authorization;
- active context.

Instead, it generates requests and consumes responses.

```text
Echo Pyramid

↓

Context Change Request

↓

Cognitive Runtime

↓

Validation

↓

Semantic Decision

↓

System Update
```

This preserves a single authority for cognitive decisions.

---

# Hardware Abstraction

Audio hardware access is isolated from the remainder of the firmware.

```text
Application

↓

Audio Bridge

↓

Vendor Library

↓

Hardware
```

This abstraction provides several engineering advantages:

- hardware independence;
- simplified maintenance;
- reduced coupling;
- easier migration to future hardware revisions.

The remaining firmware never accesses codec drivers directly.

---

# Voice Processing Strategy

Incoming and outgoing voice processing are intentionally independent.

## Incoming Speech

```text
TCP Audio

↓

Voice Receiver

↓

Audio Bridge

↓

Speaker
```

---

## Outgoing Speech

```text
Microphones

↓

Audio Bridge

↓

Wake Word Detector

↓

Context Request
```

This bidirectional organization allows both pipelines to evolve independently.

---

# Vendor Library Integration

The firmware incorporates the **M5Echo-Pyramid** library supplied by the hardware manufacturer.

Rather than modifying application logic to match the library, a dedicated abstraction layer was adopted.

Local engineering adaptations include:

- integration with the ESP-IDF build system;
- compatibility with ESP-IDF v5;
- support for application-owned I²C master buses;
- integration with the current I²S driver model.

The original library architecture is preserved whenever possible.

---

# Build Environment

Validated development environment:

```text
ESP-IDF

ESP32-S3

Native ESP-IDF Build System

CMake

FreeRTOS
```

The project is developed using the native ESP-IDF toolchain without relying on Arduino compatibility layers.

---

# External Dependencies

Primary software dependencies include:

```text
ESP-IDF

ESP-SR

FreeRTOS

LWIP

ESP Wi-Fi

M5Echo-Pyramid
```

Each dependency is isolated within dedicated components whenever practical.

---

# Repository Organization

```text
echo-pyramid-voice-node/

├── README.md
├── CMakeLists.txt
├── main/
├── components/
│   ├── M5Echo-Pyramid/
│   ├── voice_receiver/
│   └── wake_word_detector/
```

Implementation remains close to source code, while engineering investigations and architectural rationale are documented separately.

---

# Validated Features

The following capabilities have been validated during the current engineering baseline.

```text
✓ Wi-Fi connectivity

✓ Local operational display

✓ Audio subsystem initialization

✓ TCP PCM reception

✓ PCM playback

✓ Wake Word detection

✓ MultiNet command recognition

✓ Context change request generation

✓ Bidirectional integration with the Cognitive Runtime

✓ Modular software architecture

✓ Hardware abstraction through Audio Bridge
```

Only validated functionality is described as implemented.

---

# Future Work

The following items remain outside the validated baseline and are considered future engineering work.

Examples include:

- expanded conversational capabilities;
- additional voice commands;
- broader natural language interaction;
- user experience refinements;
- performance optimization and calibration improvements where documented in the corresponding Technical Notes.

Future work is intentionally separated from validated functionality to preserve engineering accuracy.

---

# Documentation Strategy

The Echo Pyramid Voice Node documentation follows a layered organization.

The README provides the architectural overview.

Component-specific implementation details remain close to their respective source modules.

Engineering investigations, design rationale and validation history are preserved through dedicated Technical Notes.

This organization minimizes duplication while improving long-term maintainability.

---

# Engineering Philosophy

The firmware is developed according to the following principles:

- implementation is the authoritative engineering source;
- documentation reflects validated behavior;
- architectural responsibilities remain clearly separated;
- hardware dependencies are encapsulated;
- runtime contracts are preserved;
- reproducibility takes precedence over implementation shortcuts.

These principles are consistently applied throughout the Ambient Physical AI project and guide future evolution of the Expression Layer.

---

# Documentation Hierarchy

The Echo Pyramid Voice Node documentation is organized into multiple layers in order to separate architectural documentation, implementation details and engineering investigations.

The documentation hierarchy follows the engineering standards adopted throughout the Ambient Physical AI project.

```text
README.md
        │
        ├── Firmware overview
        ├── Architecture
        ├── Component organization
        └── Documentation navigation

docs/
└── notes/
        │
        ├── Engineering Guides
        ├── Technical Notes
        ├── Integration Milestones
        ├── Validation Reports
        └── Design Rationale
```

The README intentionally avoids duplicating detailed engineering investigations already documented in the Technical Notes.

---

# Node Boundaries

The Echo Pyramid Voice Node is responsible exclusively for voice interaction.

Its architectural boundaries are intentionally strict.

```text
Identity Node

↓

Identity Authentication

↓

Cognitive Runtime

↓

Semantic Decision

↓

Echo Pyramid Voice Node

↓

Voice Interaction
```

The node never:

- authenticates users;
- stores user identity;
- authorizes commands;
- selects contexts autonomously;
- executes cognitive reasoning.

Likewise, it never modifies the global system state directly.

Instead, it communicates with the Cognitive Runtime using validated runtime contracts.

---

# Validation Status

## Validated

The following capabilities are part of the validated engineering baseline.

```text
✓ Local Status Display

✓ Wi-Fi connectivity

✓ Audio subsystem initialization

✓ PCM playback

✓ TCP voice reception

✓ Wake Word detection

✓ MultiNet command recognition

✓ Context change requests

✓ Audio hardware abstraction

✓ Modular component architecture

✓ Integration with the Cognitive Runtime
```

---

## Planned / Future Work

The following items are intentionally documented as future engineering work.

```text
Additional voice commands

Expanded conversational interaction

User experience improvements

Additional optimization and calibration

Future voice capabilities documented in the Technical Notes
```

These items shall not be interpreted as currently implemented functionality.

---

# Related Documentation

The following Technical Notes complement this README.

| Document | Description |
|----------|-------------|
| `docs/notes/TECHNICAL_NOTE_PERSONALIZED_WELCOME_INTEGRATION.md` | Documents the introduction of the personalized welcome protocol. |
| `docs/notes/ECHO_PYRAMID_VOICE_INTEGRATION_MILESTONE.md` | Documents the first end-to-end voice integration milestone. |
| `docs/notes/ECHO_PYRAMID_WAKE_WORD_CONTEXT_INTEGRATION_TECHNICAL_NOTE.md` | Documents Wake Word integration, context change requests and ESP-SR engineering decisions. |
| `docs/notes/WAKE_WORD_CALIBRATION_AND_BUILD_RECOVERY.md` | Engineering guide for Wake Word calibration, build recovery and reproducible ESP-SR validation procedures. |

These documents preserve engineering rationale and implementation history without overloading the firmware README.

---

# Repository Navigation

```text
echo-pyramid-voice-node/

README.md
│
├── main/
│
├── components/
│      ├── M5Echo-Pyramid/
│      ├── voice_receiver/
│      └── wake_word_detector/
```

The repository organization intentionally keeps implementation close to source code while preserving architectural documentation at the firmware root.

---

# Related Documentation

The Echo Pyramid Voice Node is part of the Expression Layer.

Additional documentation can be found in the corresponding firmware nodes.

| Layer | Node |
|--------|------|
| Expression Layer | StickC Plus2 RGB Node |
| Expression Layer | Atom Matrix RGB Node |
| Expression Layer | RGB Strip Node |
| Identity Layer | Identity Node |
| Presence Layer | Presence Node |
| Ambient Runtime | Ambient Runtime Node |
| Cognitive Runtime | Cognitive Runtime |

Together these documents describe the complete distributed architecture of the Ambient Physical AI ecosystem.

---

# Documentation Principles

The documentation of this firmware follows the engineering principles adopted across the Ambient Physical AI project.

Primary principles include:

- implementation is the authoritative engineering source;
- documentation reflects validated behavior;
- architectural responsibilities remain clearly separated;
- implementation details remain close to source code;
- engineering investigations are preserved through Technical Notes;
- future work is explicitly distinguished from validated functionality.

This approach improves long-term maintainability while reducing documentation duplication.

---

# Engineering Philosophy

The Echo Pyramid Voice Node was developed following a modular systems engineering approach.

Each subsystem owns a single primary responsibility.

Hardware access is isolated.

Runtime communication is contract-driven.

Voice processing remains independent from semantic reasoning.

This separation enables independent evolution of the Expression Layer while preserving compatibility with the Cognitive Runtime and the remaining Ambient Physical AI ecosystem.

---

# License

Unless otherwise noted, the firmware source code follows the licensing adopted by the Ambient Physical AI project.

Vendor libraries incorporated into this firmware retain their respective original licenses.

---

# Revision Policy

This documentation shall evolve together with the validated implementation.

Whenever implementation and documentation disagree:

```text
Implementation

↓

Authoritative Source

↓

Documentation Updated
```

The implementation shall always be considered the authoritative engineering reference.

---

**End of Document**