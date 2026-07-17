# Ambient Physical AI

## System Architecture Specification

**Version:** 1.1\
**Part 02 --- System Overview & Architectural Layers**

------------------------------------------------------------------------

# 3. System Overview

## 3.1 Architectural Concept

Building upon the conceptual foundation established in **Part 01**,
Ambient Physical AI is organized as a **Distributed Cognitive
Ecosystem** composed of specialized nodes that cooperate through
**Semantic Events**.

The architecture is intentionally centered on functional
responsibilities rather than individual hardware devices. Each subsystem
owns a clearly defined role, allowing independent implementation and
evolution while preserving the overall architectural model.

This separation enables incremental hardware improvements without
requiring system redesign, reinforcing one of the fundamental
engineering principles of the project.

------------------------------------------------------------------------

## 3.2 High-Level System Flow

``` text
Presence Layer
        ↓
Identity Layer
        ↓
Cognitive Runtime (AX630C + StackFlow)
        ↓
Semantic Events
        ├──────────────┬──────────────┐
        ▼              ▼              ▼
Ambient Runtime   Human Interaction  Expression Layer
```

The **Cognitive Runtime** acts as the semantic authority of the
ecosystem. It validates incoming information, maintains the **Active
Context**, coordinates reasoning, and distributes Semantic Events to the
remaining architectural layers.

------------------------------------------------------------------------

## 3.3 Human-Centered Laboratory

The reference deployment for Ambient Physical AI is a **Human-Centered
Laboratory**, where the environment actively supports research
activities rather than functioning solely as a passive infrastructure.

Although multiple individuals may be physically present, the
architecture intentionally maintains:

-   One **Active Researcher**
-   One **Active Context**
-   One **Attention Focus**

This design simplifies decision making, reduces ambiguity and preserves
semantic consistency across the ecosystem.

------------------------------------------------------------------------

## 3.4 Active Context

As introduced in Part 01, the **Active Context** represents the semantic
description of the current activity being performed.

Typical contexts include:

-   Research
-   Laboratory
-   Meeting
-   Classroom
-   Demonstration

The initial context may be inferred from the researcher's profile and
subsequently refined through user interaction, including the Identity
Layer and voice-based interfaces.

------------------------------------------------------------------------

# 4. Architectural Layers

The architecture is organized into functional layers. Each layer
encapsulates a specific responsibility while collaborating through
semantic contracts rather than implementation details.

## 4.1 Presence Layer

**Purpose**

Detect the presence of people within the environment and generate the
initial Presence Events.

**Current implementation**

-   AtomS3 Lite
-   HLK-LD2410C radar sensor

**Output**

Presence Events.

------------------------------------------------------------------------

## 4.2 Identity Layer

**Purpose**

Determine the identity of the Active Researcher.

**Current implementation**

-   M5Dial
-   NFC
-   Encoder
-   Touch Interface

The Identity Layer produces the **Identity Package**, which is consumed
by the Cognitive Runtime.

------------------------------------------------------------------------

## 4.3 Cognitive Runtime

Implemented on the AX630C + LLM Mate platform.

Primary responsibilities include:

-   semantic interpretation;
-   Active Context management;
-   reasoning;
-   StackFlow coordination;
-   interaction with external Large Language Models when appropriate;
-   generation of Semantic Events.

------------------------------------------------------------------------

## 4.4 Ambient Runtime

Implemented on the Tab5 platform.

Responsibilities include:

-   environmental adaptation;
-   runtime console;
-   environmental sensing;
-   execution of ambient actions;
-   peripheral coordination;
-   hardware abstraction for ambient devices.

The Ambient Runtime executes decisions issued by the Cognitive Runtime
but does not own semantic authority.

------------------------------------------------------------------------

## 4.5 Human Interaction

Human interaction is achieved through complementary interfaces.

### Research Assistant

**StackChan**

Provides conversational assistance to the Active Researcher.

### Voice Interface

**Voice Pyramid + AtomS3R**

Receives wake words, processes voice interaction and renders spoken
responses.

### Haptic Interface *(Future Evolution)*

Wearable devices providing discreet tactile feedback.

### Visual Expression

Distributed RGB-based Expression Nodes provide semantic visual feedback
regarding the current cognitive state.

------------------------------------------------------------------------

## 4.6 Layer Independence

Architectural layers communicate exclusively through semantic contracts
rather than hardware-specific implementations.

This approach promotes modularity, reproducibility and maintainability
while allowing independent subsystem evolution.

The architecture has experimentally validated the principle of
**Hardware evolution without system redesign**, demonstrating that
hardware platforms may evolve while preserving the conceptual integrity
of the Distributed Cognitive Ecosystem.

------------------------------------------------------------------------

**End of Part 02 (Version 1.1 -- Editorial Edition)**
