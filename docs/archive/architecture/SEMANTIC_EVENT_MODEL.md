# SEMANTIC_EVENT_MODEL

## Ambient Physical AI

### Architectural Decision

### Date

2026-06-28

---

# Purpose

This document defines the official semantic boundary between the Cognitive Runtime and the Expression Layer.

Its purpose is to preserve the separation between semantic reasoning and multimodal expression while allowing the architecture to evolve without coupling expression devices to internal cognitive representations.

---

# Architectural Motivation

The Cognitive Runtime is responsible for transforming physical observations into semantic understanding.

Expression devices are responsible for presenting that understanding through speech, animation, lighting, displays, haptics or other interaction modalities.

To preserve this separation, only Semantic Events shall cross the boundary between Cognition and Expression.

---

# Cognitive Runtime

The Cognitive Runtime owns semantic reasoning.

Its internal representations include:

```text
Identity Package

5W Context Package

Current Runtime Context

Decision Engine internal state
```

These objects are private cognitive representations.

They are implementation details.

They may evolve without affecting any Expression Layer device.

---

# Public Cognitive Interfaces

The Cognitive Runtime may expose two kinds of public semantic interfaces.

## 1. Semantic Events

Used by Expression Layer devices.

Purpose:

```text
Notify that a semantic decision has already been made.
```

Examples:

```text
welcome_user

start_classroom_mode

ambient_idle

presentation_started

meeting_finished
```

Expression devices consume these events.

They do not interpret identity or context.

---

## 2. Semantic Services

Used by authorized runtime consumers.

Examples:

```text
GetCurrentIdentity()

GetCurrentContext()

GetRuntimeStatus()

GetCurrentActivity()
```

These services provide semantic queries.

They are independent from the Expression Layer.

---

# Expression Layer

Expression devices shall never consume:

```text
Identity Package

5W Context Package

Current Runtime Context

Decision Engine internal state
```

Instead they consume:

```text
Semantic Events
```

Their responsibility is limited to rendering those events.

Examples include:

```text
Speech

Voice

Displays

Lighting

RGB animations

Wearables

Robotic motion

Future multimodal interfaces
```

---

# Architectural Flow

```text
Presence
        │
        ▼
Identity
        │
        ▼
Context Builder
        │
        ▼
5W Context Package
        │
        ▼
Current Runtime Context
        │
        ▼
Decision Engine
        │
        ▼
Semantic Event
        │
────────┼────────────────────────────
        │
        ▼
Expression Layer
```

---

# Example

Internal cognitive representation:

```json
{
  "who":"Claudio",
  "where":"Lab",
  "when":"Morning",
  "what":"Arrived",
  "why":"Teaching"
}
```

This object never leaves the Cognitive Runtime.

Instead, the Runtime generates:

```json
{
  "event":"welcome_professor"
}
```

Expression devices independently render that event.

Example:

```text
StackChan
→ "Welcome back, Professor."

Voice Pyramid
→ Spoken greeting.

Tab5
→ Welcome screen.

RGB Nodes
→ Blue animation.

Wearable
→ Short vibration.
```

Each device expresses the same semantic event according to its own modality.

---

# Benefits

This model provides:

* centralized semantic reasoning;
* lightweight expression devices;
* hardware independence;
* architectural scalability;
* maintainability;
* reproducibility;
* complete separation between cognition and presentation.

---

# Architectural Principle

The Cognitive Runtime owns knowledge.

The Expression Layer owns presentation.

The semantic boundary between them is the Semantic Event.

Internal cognitive representations remain private to the Runtime.

Expression devices consume only semantic events and never perform contextual reasoning.

This principle becomes one of the core architectural foundations of Ambient Physical AI.
