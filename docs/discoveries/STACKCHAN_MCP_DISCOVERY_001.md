# STACKCHAN_MCP_DISCOVERY_001

## Ambient Physical AI

### Status

Discovery Report

### Date

2026-06-24

---

# Purpose

This document records an important discovery made during the first investigations involving StackChan integration with the Ambient Physical AI ecosystem.

The discovery emerged while validating the first cognitive pipeline:

```text
Presence
↓
Identity
↓
Cognitive Runtime
↓
StackChan
```

The original objective was to determine how the AX630C Cognitive Runtime could notify StackChan after a user was identified.

During this investigation, evidence suggested that StackChan may operate through a context-oriented architecture rather than a simple command-driven architecture.

---

# Background

At the time of this discovery, the following milestones had already been validated.

## Presence Layer

Validated:

```text
AtomS3 Lite
+
Unit Mini ToF-90
```

Flow:

```text
Distance Measurement
↓
Presence Detection
↓
UDP Event
```

---

## Identity Layer

Validated:

```text
M5Dial
+
WS1850S NFC
```

Capabilities:

```text
UID Reading
Profile Mapping
Identity Package Generation
```

Example:

```json
{
  "type":"identity_package",
  "profile":{
    "id":"claudio",
    "name":"Claudio",
    "role":"owner"
  },
  "context":"Lab"
}
```

---

## Cognitive Runtime

Validated:

```text
AX630C + LLM Mate
Ubuntu 22.04
Python Runtime
UDP Listener
Context Builder
```

Validated flow:

```text
Identity Package
↓
UDP
↓
AX630C
↓
Context Object
↓
Human-readable Message
```

---

# Initial Assumption

The original assumption was:

```text
AX630C
↓
Direct Command
↓
StackChan
↓
Speech / Expression
```

Example:

```text
AX630C sends:
"Hello Claudio."
```

and StackChan simply displays or speaks the message.

---

# Discovery

While inspecting the StackChan mobile application, the following configurable elements were identified:

```text
LLM Model
Personality
Memory
MCP
```

The MCP field pointed to:

```text
wss://api.xiaozhi.me/mcp/?token=...
```

This was the first indication that StackChan might be connected to a larger context-aware architecture.

---

# MCP Endpoint Observation

Observed endpoint:

```text
wss://api.xiaozhi.me/mcp/?token=...
```

Characteristics:

```text
Secure WebSocket
External Service
Token-based Access
Cloud Endpoint
```

This suggests that StackChan may not simply receive commands.

Instead, it may operate through:

```text
Context
+
Memory
+
Personality
+
LLM
↓
Agent Behavior
```

---

# Network Validation

## DNS Resolution

Test:

```bash
ping api.xiaozhi.me
```

Result:

```text
PASS
47.83.164.0
0% packet loss
```

---

## HTTPS Endpoint

Test:

```bash
curl https://api.xiaozhi.me
```

Result:

```text
404 Not Found
nginx/1.20.1
```

Interpretation:

```text
Server exists.
HTTPS works.
Root path is not a public HTTP API.
```

---

# AX630C Capability Validation

The AX630C runtime was validated for WebSocket support.

Installed:

```bash
python3 -m pip install websocket-client
```

Validation:

```python
import websocket
print("websocket-client OK")
```

Result:

```text
PASS
```

Conclusion:

```text
AX630C can participate in WebSocket-based integrations.
```

---

# Architectural Reinterpretation

The most important result of this discovery is a shift in architectural thinking.

Instead of asking:

```text
How do we send commands to StackChan?
```

A better question may be:

```text
How do we provide context to StackChan?
```

---

# Emerging Architecture

Previous interpretation:

```text
Identity
↓
AX630C
↓
Command
↓
StackChan
```

New hypothesis:

```text
Identity
↓
AX630C
↓
Context Package
↓
MCP Context Channel
↓
StackChan
↓
Adaptive Behavior
```

---

# Context Package Concept

The Identity Layer already produces meaning.

StackChan does not need:

```text
Raw NFC UID
```

It needs:

```text
User Meaning
```

Example:

```json
{
  "event_type":"user_identified",
  "user":{
    "id":"claudio",
    "name":"Claudio",
    "role":"owner"
  },
  "environment":{
    "context":"Lab"
  }
}
```

This allows StackChan to adapt behavior without understanding NFC itself.

---

# Potential Implications

If the MCP channel accepts contextual information, StackChan could become:

```text
Context-Aware
User-Aware
Environment-Aware
```

without requiring direct firmware modifications.

This aligns strongly with the Ambient Physical AI vision.

---

# Risks

The MCP protocol remains unknown.

Unknown items include:

```text
Handshake Procedure
Authentication Model
Message Schema
Write Permissions
Memory APIs
Context APIs
```

At this stage there is no evidence that external context injection is supported.

Therefore no such claim should be made.

---

# Important Restriction

The following actions are intentionally excluded:

```text
Token extraction
Aggressive protocol reverse engineering
Unauthorized API interaction
Firmware modification
Undocumented cloud service manipulation
```

The investigation should remain controlled and reproducible.

---

# Current Status

Validated:

```text
Presence Layer
Identity Layer
Identity → AX630C UDP
Context Builder
Human-readable Message Generation
StackChan MCP Endpoint Discovery
AX630C WebSocket Capability
```

Not yet validated:

```text
StackChan Context Injection
StackChan MCP Integration
StackChan Adaptive Reaction
```

---

# Recommended Next Laboratory

Suggested laboratory:

```text
LAB_STACKCHAN_MCP_001
```

Objective:

```text
Understand the MCP architecture used by StackChan.
```

Research questions:

```text
Is MCP publicly documented?
Does StackChan accept external context?
Can AX630C act as a context provider?
Can MCP be self-hosted?
Can context be updated programmatically?
```

---

# Architectural Impact

If the MCP hypothesis is confirmed, the future architecture may evolve toward:

```text
Presence
↓
Identity
↓
AX630C
↓
Context Package
↓
StackFlow
↓
MCP Context Channel
↓
StackChan
↓
Expression
```

This would reinforce the design principle that:

```text
AX630C
=
Cognition

StackFlow
=
Semantic Coordination

StackChan
=
Social and Expressive Agent
```

---

# Conclusion

This discovery suggests that StackChan should not be viewed merely as an output device.

Instead, it may function as a context-aware agent whose behavior is influenced by:

```text
Personality
Memory
MCP Context
LLM
```

The MCP endpoint discovery represents an important architectural insight and may define the future integration path between the Cognitive Runtime and StackChan.

Status:

```text
DISCOVERED
NOT YET VALIDATED
```

Further protocol investigation is required before implementation decisions are made.
