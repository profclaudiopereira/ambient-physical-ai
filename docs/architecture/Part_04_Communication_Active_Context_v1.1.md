# Ambient Physical AI

## System Architecture Specification

**Version:** 1.1\
**Part 04 --- Communication Model & Active Context Model**

------------------------------------------------------------------------

# 6. Communication Model

## 6.1 Communication Philosophy

As introduced in Part 01, the Distributed Cognitive Ecosystem
communicates through **Semantic Events** rather than hardware-specific
commands.

Nodes describe **what happened** or **what should be achieved**, never
**how another subsystem must implement the requested behavior**. This
semantic contract preserves modularity and enables independent subsystem
evolution.

------------------------------------------------------------------------

## 6.2 Semantic Events

Typical Semantic Events include:

-   Presence detected
-   Identity recognized
-   Context changed
-   Research started
-   Meeting activated
-   Classroom mode enabled
-   Welcome Researcher
-   Ambient adaptation requested

Semantic Events provide the common language shared by every
architectural layer.

------------------------------------------------------------------------

## 6.3 Communication Topology

``` text
Presence Layer
      │
      ▼
Identity Layer
      │
      ▼
Cognitive Runtime
      │
      ├──────────────┬──────────────┬──────────────┐
      ▼              ▼              ▼              ▼
Ambient Runtime  StackChan  Voice Pyramid  Expression Layer
```

The **Cognitive Runtime** receives, validates and interprets semantic
information before coordinating distributed responses throughout the
ecosystem.

------------------------------------------------------------------------

## 6.4 Communication Strategy in Version 1

Version 1 intentionally prioritizes:

-   simplicity;
-   low latency;
-   reproducibility;
-   straightforward debugging;
-   rapid engineering validation.

For these reasons, UDP was selected as the initial transport mechanism.
The architecture intentionally favors engineering validation over
production-grade communication security during the first implementation
cycle.

------------------------------------------------------------------------

## 6.5 Future Evolution

Future communication technologies under evaluation include:

-   authenticated messages;
-   message signing;
-   MQTT;
-   MQTT over TLS;
-   DTLS;
-   secure device identity;
-   secure boot.

These improvements represent implementation evolution and do not modify
the validated architectural model.

------------------------------------------------------------------------

# 7. Active Context Model

## 7.1 Motivation

As established in Part 01, Ambient Physical AI adapts to the semantic
meaning of the current activity rather than reacting solely to isolated
sensor values.

This behavior is represented by the **Active Context**.

------------------------------------------------------------------------

## 7.2 Active Researcher

Although multiple people may be present, only one individual is
considered the **Active Researcher** at any given moment. The Identity
Layer establishes this information before semantic reasoning begins.

------------------------------------------------------------------------

## 7.3 Active Context

The Active Context defines how the Human-Centered Laboratory should
behave.

Typical contexts include:

-   Research
-   Laboratory
-   Meeting
-   Classroom
-   Demonstration

The context may be:

-   suggested from the researcher's profile;
-   selected through the Identity Layer;
-   requested through voice interaction.

Only the **Cognitive Runtime** validates context transitions.

------------------------------------------------------------------------

## 7.4 Attention Focus

The **Attention Focus** determines how interaction channels cooperate
without competing for the user's attention.

``` text
Active Context = Classroom

↓

Attention Focus = Teaching

↓

StackChan → Silent

Voice Pyramid → Wake Word Only

Ambient Runtime → Classroom Dashboard

Expression Layer → Classroom Pattern
```

This mechanism allows the laboratory to remain responsive while
preserving a coherent interaction strategy.

------------------------------------------------------------------------

## 7.5 Context Transition

``` text
Presence

↓

Identity

↓

Suggested Context

↓

Explicit Confirmation

↓

Cognitive Validation

↓

Semantic Events

↓

Distributed Adaptation
```

Each transition represents a semantic refinement that progressively
transforms physical observations into coordinated system behavior.

------------------------------------------------------------------------

## 7.6 Architectural Principle

Throughout normal operation the ecosystem maintains:

-   One Active Researcher.
-   One Active Context.
-   One Attention Focus.

This principle ensures deterministic reasoning, semantic consistency and
coordinated adaptation across the Distributed Cognitive Ecosystem.

------------------------------------------------------------------------

**End of Part 04 (Version 1.1 -- Editorial Edition)**
