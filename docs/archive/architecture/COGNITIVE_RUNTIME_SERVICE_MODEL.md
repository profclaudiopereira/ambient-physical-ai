# COGNITIVE_RUNTIME_SERVICE_MODEL

## Ambient Physical AI

### Status

Architectural Decision

### Date

2026-06-24

---

# Purpose

This document formalizes an important architectural evolution of the Ambient Physical AI project.

The Cognitive Runtime is no longer viewed simply as a receiver of contextual information.

Instead, it becomes the provider of semantic cognitive services for the entire ecosystem.

This decision was established after the successful validation of MCP communication between the AX630C Cognitive Runtime and StackChan.

---

# Previous Architecture

The initial integration concept was based on direct communication.

```text
Presence
↓

Identity

↓

AX630C

↓

Context Builder

↓

Direct Message

↓

StackChan
```

In this model, the Cognitive Runtime generated messages and attempted to deliver them directly to StackChan.

---

# Architectural Discovery

During the MCP investigation, the following capabilities were experimentally validated:

```text
WebSocket communication

MCP initialize

MCP tools/list

MCP tools/call

MCP keepalive

AX630C acting as MCP Tool Provider

Successful execution of semantic tools

StackChan consuming returned results
```

This demonstrated that StackChan naturally operates as an MCP client capable of consuming services exposed by the AX630C.

---

# New Architectural Model

The Cognitive Runtime should not primarily send messages.

Instead, it should expose semantic services.

The architecture becomes:

```text
Physical World
        │
        ▼
Presence Layer
        │
        ▼
Identity Layer
        │
        ▼
Identity Package
        │
        ▼
AX630C Cognitive Runtime
        │
        ▼
Semantic Knowledge Layer
        │
        ▼
MCP Tool Provider
        │
        ▼
StackChan
        │
        ▼
Voice
Dialogue
Expression
```

---

# Responsibility of the AX630C

The AX630C becomes responsible for publishing semantic knowledge.

Examples include:

```text
Current Identity

Presence State

Environment Context

Ambient Status

Runtime Status

Memory Services (future)

Knowledge Services (future)
```

The Cognitive Runtime should expose meaning rather than low-level hardware information.

---

# Engineering Principle

Hardware nodes produce events.

The Cognitive Runtime transforms those events into semantic knowledge.

External agents consume semantic knowledge through MCP.

This separation keeps the system modular and scalable.

---

# Examples

Instead of exposing:

```text
GPIO

I2C

NFC UID

Distance Register
```

the Cognitive Runtime should expose:

```text
GetCurrentIdentity()

GetPresenceState()

GetEnvironmentContext()

GetRuntimeStatus()
```

These services represent the meaning of the environment rather than implementation details.

---

# Role of StackChan

Within Ambient Physical AI, StackChan is no longer viewed merely as an expressive device.

Instead, it acts as a cognitive agent capable of requesting semantic services from the Cognitive Runtime.

Its responsibilities include:

```text
Conversation

Voice

Facial Expression

Social Interaction

Context Consumption

MCP Client
```

StackChan should request information when needed instead of receiving predefined commands whenever possible.

---

# Layer Responsibilities

The ecosystem now follows this conceptual separation:

```text
Presence Layer

Detects physical presence.
```

```text
Identity Layer

Determines who is present.
```

```text
Cognitive Runtime

Transforms events into semantic knowledge.
```

```text
StackFlow

Coordinates semantic services and future distributed cognition.
```

```text
Expression Layer

Transforms semantic knowledge into interaction with people.
```

---

# Future Evolution

The first MCP services are expected to include:

```text
GetCurrentIdentity()

GetPresenceState()

GetEnvironmentContext()

GetRuntimeStatus()
```

Future versions may expand toward:

```text
Memory

Knowledge Base

Scheduling

Environmental Intelligence

Distributed Cognitive Services
```

without changing the overall architecture.

---

# Benefits

This model provides:

```text
Loose coupling

Scalability

Reusability

Service-oriented cognition

Clean separation of responsibilities

Natural MCP integration
```

The same Cognitive Runtime can simultaneously serve multiple expressive agents without changing its internal logic.

---

# Architectural Impact

This decision establishes the AX630C as the semantic service provider of the Ambient Physical AI ecosystem.

Rather than controlling expressive devices directly, the Cognitive Runtime publishes reusable cognitive capabilities.

This architecture aligns naturally with MCP and provides a scalable foundation for future distributed cognitive systems.

---

# Guiding Statement

```text
Sensors observe the world.

Identity gives meaning to people.

The Cognitive Runtime transforms events into knowledge.

MCP publishes knowledge.

Agents consume knowledge.

Expression becomes a consequence of understanding, not of predefined commands.
```
