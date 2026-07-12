# ACTIVE_CONTEXT_MODEL

## Ambient Physical AI

### Architectural Decision

### Date

2026-06-28

---

# Purpose

This document defines how Ambient Physical AI manages the active semantic context of the environment.

Version 1 intentionally adopts a **Single Active Context Model**, providing a deterministic semantic state while preserving a clear migration path toward future multi-user environments.

---

# Motivation

Ambient Physical AI is designed to be context-aware.

However, supporting multiple simultaneous users introduces additional architectural challenges, including:

```text
User priority
Context ownership
Context merging
Conflict resolution
Presence attribution
Concurrent user intents
Session management
```

These capabilities are important but are outside the scope of Version 1.

The objective of V1 is to provide a robust, reproducible and easily understandable cognitive model for the competition.

---

# Architectural Decision

Ambient Physical AI Version 1 maintains exactly one:

```text
Current Runtime Context
```

at any given time.

This context represents the active semantic state of the environment and the user currently interacting with the system.

---

# Cognitive Model

```text
One Environment
        │
        ▼
One Active User
        │
        ▼
One Current Runtime Context
```

This model guarantees deterministic semantic reasoning while significantly reducing implementation complexity.

---

# Context Creation

When no active context exists:

```text
Presence evidence detected
        │
        ▼
Identity validated
        │
        ▼
Current Runtime Context created
```

Important:

Presence sensors provide **evidence of occupancy**, not proof of human identity.

Identity validation is required before creating an active semantic context.

---

# Context Refresh

If the currently active user is identified again:

```text
Same user detected
        │
        ▼
Current Runtime Context refreshed
```

No semantic conflict is generated.

---

# User Switch

If another authenticated user is identified while an active context already exists:

```text
Different user detected
        │
        ▼
Explicit user transition
        │
        ▼
Current Runtime Context replaced
        │
        ▼
Semantic Event generated
```

Example:

```json
{
    "event": "active_user_changed",
    "previous_user": "Claudio",
    "current_user": "Student",
    "reason": "new_nfc_identity_detected"
}
```

The Runtime never silently overwrites the current context.

Every transition is explicit.

---

# Active Session

Once a Current Runtime Context exists, new interactions from the active user do **not** create another context.

Instead, they generate:

```text
User Intent Events
```

Possible sources include:

```text
M5Dial
Voice Pyramid
StackChan
Future Interfaces
```

Examples:

```text
Rotate encoder

Change room mode

Turn lights on

Start presentation

Explain current context

Adjust ambient settings
```

These intent events are evaluated inside the existing Current Runtime Context before new Semantic Events are produced.

---

# User Exit

The Runtime never assumes immediately that a user has left.

Instead it performs semantic inference.

Conceptually:

```text
Presence evidence no longer available
        │
        ▼
Configurable validation period
        │
        ▼
User exit inferred
        │
        ▼
Current Runtime Context cleared
        │
        ▼
Idle State
```

Example configuration:

```json
{
    "presence_timeout_seconds": 60
}
```

Example Semantic Event:

```json
{
    "event": "active_user_left",
    "previous_user": "Claudio",
    "reason": "presence_timeout"
}
```

The timeout value is intentionally configurable and may evolve as new presence technologies become available.

---

# Idle State

When no active context exists:

```text
Idle
```

The system waits for a new Presence → Identity cycle before creating another semantic context.

---

# Future Evolution

Future versions may introduce more advanced cognitive models.

Possible topics include:

```text
Multiple active users

Primary user election

Role-based priority

Context conflict resolution

Distributed presence attribution

Presence confidence fusion

Multiple presence sensors

Collaborative environments

Group context

Context persistence
```

These capabilities are intentionally deferred to preserve simplicity during the delivery phase.

---

# Architectural Principles

Version 1 prioritizes:

* deterministic behavior;
* semantic clarity;
* reproducibility;
* incremental evolution.

The Current Runtime Context remains the unique semantic reference for the Cognitive Runtime.

All user interactions occurring during an active session are interpreted within that context.

Future multi-user support should extend this model rather than replace it.

---

# Final Principle

The Single Active Context Model is intentionally simple.

Its objective is not to solve every contextual scenario.

Its objective is to establish a robust semantic foundation capable of supporting future architectural evolution while remaining completely reproducible for the Ambient Physical AI competition.
