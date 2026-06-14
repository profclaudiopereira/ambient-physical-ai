# Scenario — Context-Aware Cognitive Lab

## Demonstration Narrative

This document describes the official demonstration scenario for Ambient Physical AI.

The objective is to show how a distributed cognitive ecosystem can perceive, identify, understand and adapt to human presence inside a contextual environment.

---

# Environment

The demonstration takes place inside a technology laboratory.

The laboratory contains:

* Presence sensors
* Identity devices
* Cognitive runtime nodes
* Ambient runtime nodes
* Expression nodes
* Voice interaction nodes

All components cooperate through the Ambient Physical AI architecture.

---

# Initial State

The environment is idle.

No user is currently interacting with the system.

```text
Presence: Idle
Identity: Unknown
Context: Empty
Ambient State: Standby
```

---

# Step 1 — Presence Detection

A person enters the environment.

The Unit Mini ToF detects movement and proximity.

```text
Person detected
```

Event generated:

```json
{
  "event": "presence_detected"
}
```

---

# Step 2 — Identity Recognition

The user presents an NFC credential using M5Dial.

Example:

```json
{
  "user": "claudio",
  "role": "owner",
  "mode": "developer"
}
```

Identity becomes available to the ecosystem.

---

# Step 3 — Context Creation

Identity information is forwarded to the Cognitive Runtime.

The AX630C creates a contextual package.

Example:

```json
{
  "user": "claudio",
  "role": "owner",
  "mode": "developer",
  "location": "lab"
}
```

---

# Step 4 — Cognitive Processing

The AX630C processes:

* identity;
* context;
* environmental information;
* system state.

A contextual decision is produced.

Example:

```text
Developer mode activated.
```

---

# Step 5 — Ambient Transformation

The Ambient Runtime Node receives the decision.

PoE-P4 adapts the environment.

Possible actions:

* display information;
* update OLED messages;
* environmental feedback;
* visual indicators.

---

# Step 6 — Expression

StackChan becomes the physical embodiment of the system.

Example:

```text
Welcome back Claudio.
Developer mode activated.
```

The interaction becomes visible and tangible.

---

# Step 7 — Voice Interaction

Voice Pyramid becomes available.

The user may continue interacting naturally through speech.

Example:

```text
What should I work on today?
```

The Cognitive Runtime processes the request and generates a response.

---

# Complete Flow

```text
Person
    ↓
Presence
    ↓
Identity
    ↓
Context
    ↓
Cognition
    ↓
Ambient Transformation
    ↓
Expression
    ↓
Voice Interaction
```

---

# Key Demonstration Message

The objective of the demonstration is not to show individual devices.

The objective is to show how multiple specialized nodes cooperate to create a context-aware cognitive environment.

The environment perceives.

The environment understands.

The environment adapts.

The environment interacts.

This is the essence of Ambient Physical AI.
