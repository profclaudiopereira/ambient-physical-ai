# Echo Pyramid Voice Integration

## Engineering Milestone

### Ambient Physical AI

**Node:** Echo Pyramid Voice Node

**Branch:** `feature/ambient-runtime-node`

**Milestone:** Dynamic Personalized Voice Delivery through StackFlow TTS

**Status:** VALIDATED

---

# Purpose

This milestone documents the engineering baseline achieved during the first complete voice integration of the Echo Pyramid Voice Node with the Ambient Physical AI Cognitive Runtime.

Unlike the Technical Notes that describe individual engineering investigations, this document consolidates the complete milestone reached after integrating the embedded firmware, the Cognitive Runtime, and the StackFlow TTS services.

The objective is to preserve the validated engineering state before subsequent milestones, such as Wake Word interaction and bidirectional voice communication, introduce additional complexity.

This document records:

- implemented functionality;
- architectural decisions;
- embedded firmware evolution;
- Cognitive Runtime evolution;
- validated communication contracts;
- engineering limitations;
- lessons learned during implementation.

---

# Milestone Summary

This milestone validates the first complete end-to-end personalized voice journey within the Ambient Physical AI ecosystem.

Validated execution flow:

```text
Presence detected
        │
        ▼
Identity Node requests NFC authentication
        │
        ▼
Identity Package reaches the Cognitive Runtime
        │
        ▼
Current semantic context is generated
        │
        ▼
Personalized speech message is selected
        │
        ▼
StackFlow TTS generates PCM audio
        │
        ▼
PCM stream is packaged
        │
        ▼
TCP voice transport
        │
        ▼
Echo Pyramid Voice Node
        │
        ▼
Personalized speech playback
```

Example of validated execution:

```text
Bem-vindo ao laboratório, Hermínio.
```

Although the currently installed TTS model is English-oriented, the complete personalized message was successfully generated, transported and reproduced.

---

# Architectural Overview

The milestone preserves the architectural principles adopted throughout Ambient Physical AI.

The Cognitive Runtime remains the only authority responsible for semantic reasoning.

The Echo Pyramid Voice Node is responsible only for local voice interaction.

```text
AX630C Cognitive Runtime

    Semantic reasoning
    Identity ownership
    Authorization
    Context selection
    Message generation
    Speech synthesis

                │

                ▼

Echo Pyramid Voice Node

    Voice reception
    Audio playback
    Local display
    Visual feedback
```

This separation keeps cognition independent from expression and allows each subsystem to evolve independently.

---

# Communication Architecture

The implementation intentionally preserves two independent communication channels.

## Semantic Channel

```text
UDP

Port 5005

Compact semantic commands
```

Examples:

```text
WELCOME

WELCOME|<User>

RED

GREEN

BLUE

OFF
```

UDP remains appropriate for lightweight semantic notifications.

---

## Voice Channel

```text
TCP

Port 5006

APAI Voice Protocol V1
```

The protocol transports:

- ordered PCM audio;
- mono channel;
- 16-bit signed samples;
- 16 kHz sampling rate.

TCP guarantees ordered delivery and avoids fragmentation issues during voice playback.

---

# Embedded Firmware Evolution

This milestone significantly expanded the capabilities of the Echo Pyramid firmware.

Major engineering work included:

- vendor library adaptation for ESP-IDF;
- Audio Bridge abstraction;
- TCP voice receiver implementation;
- APAI Voice Protocol V1;
- Wi-Fi integration;
- runtime callback architecture;
- preservation of existing semantic commands.

The resulting firmware evolved from a local demonstration firmware into a reusable Expression Layer node.

---

# M5Echo-Pyramid Integration

The vendor-provided M5Echo-Pyramid library was integrated without altering its architectural responsibilities.

Engineering adaptations introduced by the project include:

- compatibility with ESP-IDF v5;
- support for application-owned I²C master buses;
- compatibility with the current ESP-IDF I²S driver;
- preservation of the original initialization sequence.

The adopted strategy avoided duplicated hardware initialization while maintaining compatibility with future vendor updates.

---

# Audio Bridge

The Audio Bridge became the hardware abstraction layer for every audio operation.

Responsibilities include:

```text
Audio initialization

PCM recording

PCM playback

Hardware isolation

Audio diagnostics
```

The remaining firmware never communicates directly with codec drivers.

Instead, all hardware interaction passes through the Audio Bridge.

This significantly reduces coupling between application logic and hardware implementation.

---

# Voice Receiver

A dedicated Voice Receiver component was introduced to isolate voice transport from audio playback.

Responsibilities include:

- TCP server management;
- APAI Voice Protocol validation;
- PCM stream reception;
- callback-based delivery to the Audio Bridge.

The component intentionally contains no hardware-specific code.

This separation allows transport and playback to evolve independently.

---

# APAI Voice Protocol V1

The milestone introduces the first validated version of the Ambient Physical AI voice transport protocol.

Protocol structure:

```text
Header

↓

PCM Audio

↓

Playback
```

Header information includes:

- protocol identifier;
- version;
- number of channels;
- sample rate;
- sample size;
- total sample count.

The payload contains signed 16-bit PCM samples encoded as little-endian mono audio.

---

# Firmware Initialization

The validated startup sequence is:

```text
Display

↓

Audio

↓

Wi-Fi

↓

Voice Receiver

↓

Semantic Receiver

↓

Operational State
```

Each subsystem is initialized independently and reports its operational status before normal execution begins.

---

# Cognitive Runtime Evolution

This milestone also introduced significant improvements inside the Cognitive Runtime.

Engineering work included:

- canonical voice message selection;
- personalized speech generation;
- StackFlow TTS integration;
- TCP audio delivery;
- Echo Pyramid adapter evolution;
- automated identity-to-voice workflow.

These additions preserve the existing semantic pipeline while extending it with dynamic voice interaction.

---

# StackFlow Integration

Voice generation is performed entirely inside the Cognitive Runtime.

Validated processing pipeline:

```text
Identity

↓

Semantic Context

↓

Voice Message

↓

StackFlow TTS

↓

PCM Audio

↓

TCP Voice Stream

↓

Echo Pyramid
```

This architecture preserves centralized cognition while allowing the Expression Layer to remain lightweight and focused exclusively on interaction.

---

# StackFlow Discovery and Validation

During this milestone, the Cognitive Runtime environment was investigated to identify the available speech synthesis services provided by the LLM Mate platform.

The engineering objective was to integrate the existing StackFlow infrastructure instead of introducing an external Text-to-Speech solution.

The investigation confirmed the presence of dedicated StackFlow services responsible for speech synthesis.

Discovered services included:

```text
llm_sys

llm_tts

llm_melotts
```

The runtime also confirmed an external communication endpoint used by StackFlow components.

```text
TCP Port 10001
```

The final integration therefore relied on the existing StackFlow communication model instead of executing standalone TTS applications.

---

# Engineering Challenges

Several approaches were evaluated during implementation before reaching the validated architecture.

The following observations are preserved because they explain important engineering decisions.

---

## StackFlow Services

An initial assumption considered the TTS executables to be conventional command-line utilities.

Practical validation demonstrated that these executables behave as long-running StackFlow services rather than standalone applications.

Consequently, speech synthesis must be requested through the StackFlow communication protocol instead of spawning external processes for each request.

This finding significantly simplified the final architecture and aligned the implementation with the native StackFlow execution model.

---

## PCM Stream Assembly

Early implementations assumed that a single returned audio block represented the complete synthesized speech.

Validation demonstrated that speech is delivered as multiple PCM blocks.

The client therefore evolved to:

```text
Receive all PCM blocks

↓

Assemble the complete stream

↓

Create a WAV-compatible buffer

↓

Transmit the complete audio
```

This correction ensured complete playback of personalized messages.

---

## Voice Session Management

Testing also demonstrated the importance of properly releasing StackFlow work units after speech generation.

The final implementation guarantees that each synthesis request is correctly finalized before subsequent requests are issued.

This prevents stale work units from affecting later speech requests and improves runtime stability.

---

# Physical Validation

The complete milestone was validated on physical hardware.

## Embedded Firmware

Validated components include:

```text
✓ Display initialization

✓ RGB feedback

✓ Wi-Fi connectivity

✓ UDP semantic receiver

✓ TCP voice receiver

✓ Audio codec initialization

✓ Microphone capture

✓ Speaker playback

✓ Audio Bridge

✓ Voice playback callback
```

---

## Cognitive Runtime

Validated services include:

```text
✓ Identity context generation

✓ Personalized message selection

✓ StackFlow communication

✓ Text-to-Speech request

✓ PCM generation

✓ Multi-block audio assembly

✓ TCP audio transmission

✓ Runtime stability
```

---

## End-to-End Journey

The complete Ambient Physical AI interaction was successfully validated.

```text
Presence Detection
        │
        ▼
Identity Authentication
        │
        ▼
Context Construction
        │
        ▼
Personalized Voice Message
        │
        ▼
StackFlow Speech Synthesis
        │
        ▼
TCP Audio Delivery
        │
        ▼
Echo Pyramid Voice Node
        │
        ▼
Personalized Speech Playback
```

Validation Status:

```text
PASS
```

---

# Known Limitations

The milestone intentionally documents the limitations observed during validation.

## Current TTS Model

The installed speech model is primarily optimized for English.

Portuguese messages are reproduced correctly but pronunciation quality is not yet ideal.

This limitation affects only speech quality.

The complete voice generation architecture has been successfully validated.

---

## Synchronous Voice Generation

Speech generation currently executes synchronously inside the validation workflow.

This approach is appropriate for the present milestone because it simplifies validation and preserves deterministic execution.

Future optimizations may introduce asynchronous processing while maintaining the existing communication contracts.

---

## Wake Word

Wake Word functionality is intentionally outside the scope of this milestone.

The objective of this engineering stage was to validate personalized speech generation and delivery.

Bidirectional voice interaction is documented separately and continues to evolve independently.

---

# Relevant Project Components

## Embedded Firmware

```text
firmware/
└── nodes/
    └── expression-node/
        └── echo-pyramid-voice-node/
```

Primary components:

```text
main/

components/
    ├── M5Echo-Pyramid/
    ├── voice_receiver/
    └── wake_word_detector/

notes/
```

---

## Cognitive Runtime

Primary runtime components:

```text
runtime/
└── cognitive/
    └── stackflow/
```

Relevant services include:

- semantic message generation;
- Echo Pyramid adapter;
- StackFlow TTS client;
- identity listener integration.

---

# Engineering Significance

This milestone represents the first complete integration of dynamic personalized speech within the Ambient Physical AI architecture.

The resulting implementation demonstrates that:

- authenticated identity can drive personalized speech;
- semantic decisions remain centralized in the Cognitive Runtime;
- speech synthesis can be performed through StackFlow services;
- PCM audio can be transported independently from semantic events;
- the Expression Layer remains responsible only for local interaction.

These architectural principles remain valid for future milestones.

---

# Milestone Conclusion

This milestone transforms the Echo Pyramid Voice Node from a local expression device into a distributed voice interaction endpoint within the Ambient Physical AI ecosystem.

The validated architecture preserves the project's fundamental engineering principles:

- centralized cognition;
- distributed expression;
- modular software architecture;
- contract-driven communication;
- clear separation of responsibilities.

The milestone establishes the engineering baseline upon which future Wake Word, conversational interaction, and multimodal voice capabilities will be developed.

---

# Milestone Status

```text
ECHO_PYRAMID_VOICE_INTEGRATION_MILESTONE_001

Status

VALIDATED
```