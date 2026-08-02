# Ambient Physical AI

## System Architecture Specification

**Version:** 1.1\
**Part 03 --- System Nodes**

------------------------------------------------------------------------

# 5. System Nodes

## 5.1 Design Philosophy

Following the architectural principles established in Parts 01 and 02,
Ambient Physical AI is implemented as a collection of autonomous and
specialized nodes forming a **Distributed Cognitive Ecosystem**.

Each node owns a single engineering responsibility and communicates
through **Semantic Events** instead of hardware-specific commands. This
separation preserves architectural stability while allowing individual
subsystems to evolve independently.

------------------------------------------------------------------------

## 5.2 Presence Node

**Purpose**

Detect the presence of a person within the environment and generate the
initial Presence Events.

**Current Hardware**

-   AtomS3 Lite
-   HLK-LD2410C radar sensor

**Responsibilities**

-   Detect human presence.
-   Generate Presence Events.
-   Trigger the first stage of the cognitive pipeline.
-   Never determine identity.

------------------------------------------------------------------------

## 5.3 Identity Node

**Current Hardware**

-   M5Dial
-   NFC
-   Encoder
-   Touch Interface

**Responsibilities**

-   Identify the Active Researcher.
-   Allow Active Context selection.
-   Produce the Identity Package.
-   Forward semantic identity information to the Cognitive Runtime.

The Identity Layer determines **who** is interacting with the
environment but does not perform semantic reasoning.

------------------------------------------------------------------------

## 5.4 Cognitive Runtime Node

**Current Hardware**

-   AX630C
-   LLM Mate
-   StackFlow

**Responsibilities**

-   Maintain the Active Researcher.
-   Maintain the Active Context.
-   Maintain the Attention Focus.
-   Interpret Semantic Events.
-   Coordinate the Distributed Cognitive Ecosystem.
-   Generate Semantic Events.

The **Cognitive Runtime** is the semantic authority responsible for
maintaining the global cognitive state of the system.

------------------------------------------------------------------------

## 5.5 Ambient Runtime Node

**Current Hardware**

-   Tab5 (ESP32-P4)

**Connected Devices**

-   PaHub
-   ENV-IV
-   DLight
-   Mini OLED
-   Future I²C peripherals

**Responsibilities**

-   Environmental sensing.
-   Runtime dashboard.
-   Physical adaptation.
-   Local device orchestration.
-   Execution of ambient actions requested by the Cognitive Runtime.

The Ambient Runtime performs physical execution while preserving the
separation between semantic reasoning and environmental control.

------------------------------------------------------------------------

## 5.6 Human Interaction Nodes

Human interaction is provided through complementary interfaces, each
optimized for a specific communication modality.

### Research Assistant

**StackChan**

-   Conversational research assistance.
-   Technical discussions.
-   Context-aware interaction.

### Voice Interface

**Voice Pyramid + AtomS3R**

-   Wake-word detection.
-   Voice commands.
-   User intent acquisition.
-   Spoken responses.

### Haptic Interface *(Future Evolution)*

Wearable devices providing discreet tactile feedback.

### Visual Expression

Distributed RGB-based Expression Nodes providing semantic visual
feedback.

------------------------------------------------------------------------

## 5.7 Node Cooperation

``` text
Presence Layer
        ↓
Identity Layer
        ↓
Cognitive Runtime
        ├── Ambient Runtime
        ├── StackChan
        ├── Voice Pyramid
        ├── Wearable
        └── Expression Layer
```

Each node contributes specialized information or capabilities, while
only the **Cognitive Runtime** maintains the global semantic state and
coordinates system-wide behavior.

------------------------------------------------------------------------

## 5.8 Engineering Benefits

This node-oriented architecture provides:

-   Modularity.
-   Scalability.
-   Hardware independence.
-   Maintainability.
-   Reproducibility.
-   Incremental evolution.
-   Hardware evolution without system redesign.

These characteristics enable the ecosystem to accommodate future
hardware platforms while preserving the validated architectural model.

------------------------------------------------------------------------

**End of Part 03 (Version 1.1 -- Editorial Edition)**
