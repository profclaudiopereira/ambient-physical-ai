# Ambient Physical AI

## SNAPSHOT_2026_06_ARCHITECTURAL_DISCOVERIES

Date: June 2026

Status: Architecture Consolidation Completed

---

# Purpose

This snapshot records the major architectural discoveries, refinements and strategic decisions consolidated during the transition from architecture design to implementation.

This document serves as a milestone between:

* Architecture Phase
* Hardware Validation Phase
* Runtime Bringup Phase

---

# Current Ecosystem Vision

```text
Person
   ↓
Presence (ToF)
   ↓
Identity (M5Dial)
   ↓
Physical Context
   ↓
Cognition (AX630C + StackFlow)
   ↓
Ambient Transformation (PoE-P4)
   ↓
Embodiment (StackChan)
   ↓
Experience
```

---

# Architectural Refinements

## Cognitive Runtime Node

Hardware:

* AX630C Module LLM Kit
* LLM Mate

Responsibilities:

* reasoning
* memory
* context interpretation
* multimodal processing
* future RAG capabilities
* decision making

Current definition:

```text
AX630C
+
StackFlow
=
Cognitive Runtime Node
```

---

## Ambient Runtime Node

Hardware:

* Unit PoE-P4

Responsibilities:

* display management
* environmental adaptation
* physical synchronization
* ambient orchestration
* perceptible responses

Current definition:

```text
PoE-P4
=
Ambient Runtime Node
```

The PoE-P4 transforms the environment.

---

## Ambient Expression Nodes

The project evolved from a centralized display concept to a distributed expression model.

Examples:

* StackChan RGB LEDs
* Voice Pyramid RGB LEDs
* NanoC6 RGB Bar
* Atom Matrix
* Mini OLED

Responsibilities:

* visual feedback
* emotional expression
* contextual indication
* ambient status representation

These nodes execute local expressions coordinated by the Ambient Runtime.

---

## Identity & Contextual Interaction Node

Hardware:

* M5Dial V1.1

Responsibilities:

* user identification
* NFC/RFID authentication
* intent selection
* contextual interaction
* profile confirmation
* operational mode selection

Current definition:

```text
Identity
+
Intent
+
Context Selection
```

The M5Dial is no longer viewed as a simple RFID reader.

It acts as the gateway between user identity and environmental behavior.

---

## Physical Context Layer

One of the most important discoveries emerged from the NFC investigation.

Traditional approach:

```text
Presence
↓
Identity
↓
Cognition
```

Refined approach:

```text
Presence
↓
Identity
↓
Physical Context
↓
Cognition
```

Physical artifacts become contextual markers.

Examples:

* identity cards
* environment cards
* intent cards
* smart objects
* semantic anchors

---

# StackFlow Refinement

StackFlow is no longer viewed as a message transport layer.

Current definition:

```text
Distributed Cognitive Coordination Fabric
```

Responsibilities:

* perception coordination
* identity coordination
* cognitive coordination
* embodiment coordination
* ambient coordination
* contextual interaction

---

# StackChan Discovery Summary

## Discovery Phase Completed

Initial assumption:

```text
Robot
+
LLM
```

Current assessment:

```text
Physical AI Agent Platform
```

Validated capabilities:

* voice interaction
* memory adaptation
* personality adaptation
* cloud LLM integration
* OTA updates
* mobile ecosystem
* MCP infrastructure
* avatar system

---

## Architectural Assessment

Recommended role:

```text
AX630C
↓
Cognitive Runtime

StackFlow
↓
Coordination Layer

StackChan
↓
Embodiment / Expression Layer
```

Not recommended:

```text
StackChan
=
Primary Cognitive Runtime
```

---

# Strategic Decisions

## Demo First. Complexity Later.

---

## Explore First. Integrate Later.

---

## Exploration Outside. Validated Knowledge Inside.

---

## Hardware Before Assumptions.

---

## Architecture Before Integration.

---

# Competition Timeline Awareness

Competition:

M5Stack Global Innovation Contest 2026

Submission Target:

27 July 2026

---

## Current Phase

June 2026

```text
Architecture Consolidation
↓
Hardware Validation
↓
Runtime Bringup
```

---

## July Goals

* AX630C validation
* StackFlow validation
* Dial integration
* PoE-P4 integration
* first end-to-end demonstrations

---

## August Goals

* competition demonstrations
* documentation
* videos
* system stabilization
* final submission

---

# Next Phase

## AX630C Bringup

Objectives:

* Linux validation
* networking validation
* StackFlow installation
* endpoint discovery
* API discovery
* local cognition validation

No StackChan integration activities are planned until AX630C and StackFlow are validated.

---

# Snapshot Conclusion

The project architecture remains stable.

The major discoveries of this phase reinforced the original vision rather than replacing it.

The project now transitions from:

```text
Architecture Phase
```

to

```text
Implementation Phase
```

with hardware-driven validation becoming the primary source of future refinements.
