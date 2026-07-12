# Cognitive Context, Memory and Agent Roles

## Ambient Physical AI

### Status

Architectural Decision (V1 Roadmap)

### Date

2026-06-24

---

# Purpose

This document consolidates the architectural decisions regarding:

* Cognitive Runtime responsibilities
* Memory architecture
* Context restoration
* Personalized information feeds
* StackChan role
* Voice Pyramid role
* Federation model
* V1 priorities until 2026-07-27

---

# Core Principle

Ambient Physical AI is not a chatbot.

Ambient Physical AI is:

```text
A Distributed Physical-Cognitive Ecosystem
```

The system combines:

```text
Presence
Identity
Memory
Context
Cognition
Ambient Transformation
Expression
```

into a coordinated environment.

---

# Cognitive Runtime

Current platform:

```text
AX630C + LLM Mate
```

Current baseline model:

```text
Qwen2.5-0.5B
```

The local model should be treated as:

```text
Cognitive Decision Engine
```

and not as:

```text
Memory System
Database
Full RAG Platform
General-Purpose Chatbot
```

---

# Memory Architecture

Fundamental rule:

```text
LLM ≠ Memory
```

The LLM receives context.

Memory remains external.

Reference flow:

```text
Presence
↓
Identity
↓
Memory Store
↓
Context Restore
↓
Cognitive Runtime
↓
Ambient Decision
```

---

# User Memory

Future user memory may contain:

```json
{
  "uid": "8804DC32",
  "profile": "claudio",
  "role": "owner",
  "last_context": "lab",
  "preferences": {
    "language": "pt-BR"
  }
}
```

Purpose:

```text
Restore user preferences
Restore environmental state
Restore contextual information
```

---

# Personalized Context Feed

Ambient Physical AI may enrich local cognition using external information.

Examples:

```text
USD/BRL
Weather
Calendar
Project Status
GitHub Issues
Sports
News
```

Principle:

```text
Local-First
Internet-Enhanced
```

The environment must continue operating without internet access.

---

# Agent Roles

## AX630C

Role:

```text
Ambient Cognitive Runtime
```

Responsibilities:

```text
Context processing
Decision making
State coordination
Context distribution
```

---

## StackFlow

Role:

```text
Distributed Context Fabric
```

Responsibilities:

```text
Context propagation
State synchronization
Inter-node coordination
```

---

## PoE-P4

Role:

```text
Ambient Runtime Node
```

Responsibilities:

```text
Physical environment transformation
Displays
Lighting
Sensors
Environmental adaptation
```

---

## StackChan

Role:

```text
Research Companion
Social Cognitive Agent
```

Responsibilities:

```text
Conversation
Explanation
Research
Brainstorming
Educational interaction
```

Important decision:

```text
StackChan should not be limited to the local AX630C model.
```

StackChan may use:

```text
Cloud LLMs
Internet resources
External knowledge
```

while remaining context-aware through StackFlow.

---

## Voice Pyramid + AtomS3R

Role:

```text
Ambient Voice Interface
```

Responsibilities:

```text
Wake word
Voice commands
Audio feedback
Environmental control
Operational interaction
```

Important decision:

```text
Voice Pyramid is not the primary research assistant.
```

Its role is operational interaction with the environment.

---

# Federation Model

Preferred architecture:

```text
                StackChan
                    |
                    |
              StackFlow Bus
                    |
                    |
AX630C ---------------------- Voice Pyramid
                    |
                    |
                 PoE-P4
```

Principle:

```text
Federated cognition
```

The ecosystem is coordinated but not fully centralized.

---

# Accessibility Vision

Future versions may adapt expression according to user accessibility profiles.

Examples:

```text
Color adaptation
High contrast modes
Audio feedback
Wearable haptics
```

This concept originated from accessibility practices commonly used in modern games.

---

# Wearable Runtime Node

Future exploration:

```text
M5StickC Plus 2
+
Vibration HAT
```

Potential role:

```text
Wearable Runtime Node
```

Possible functions:

```text
Silent notifications
Haptic feedback
Context-aware alerts
Accessibility support
```

---

# V1 Priorities

## Priority 1

Validate:

```text
Presence
↓
Identity
↓
AX630C
```

Expected result:

```text
Identity Package received
```

---

## Priority 2

Implement:

```text
Memory Store V1
```

Using:

```text
JSON
or
SQLite
```

---

## Priority 3

Implement:

```text
Context Restore
```

---

## Priority 4

Implement:

```text
Personalized Display
```

---

## Priority 5

Implement:

```text
External Context Feed
```

Starting with:

```text
USD/BRL
```

---

## Priority 6

Voice Pyramid operational integration.

---

## Priority 7

Context-aware StackChan integration.

---

# What Not To Do Before V1

Avoid:

```text
Heavy vector databases
Complex RAG systems
Deep StackChan integration
Model replacement without benchmarks
Internet dependency
Over-centralization
```

---

# Architectural Summary

```text
AX630C
=
Ambient Cognition

StackFlow
=
Context Fabric

Memory Store
=
Persistent Context

PoE-P4
=
Physical Transformation

Voice Pyramid
=
Operational Voice Interface

StackChan
=
Social and Research Agent
```

---

# Guiding Principle

```text
The environment thinks locally.

The StackChan learns, explains and explores.

The Voice Pyramid listens, commands and responds.
```

---

# Conclusion

Ambient Physical AI should be understood as:

```text
A Distributed Physical-Cognitive Ecosystem
```

rather than a single embedded chatbot.

The architecture prioritizes:

```text
Local cognition
Persistent context
Distributed coordination
Physical transformation
Human-centered interaction
```

while preserving the flexibility to integrate cloud intelligence when appropriate.
