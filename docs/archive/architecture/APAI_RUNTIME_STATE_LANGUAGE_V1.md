# APAI_RUNTIME_STATE_LANGUAGE_V1.md

# Ambient Physical AI

## Runtime State Language v1

### Status

Architectural Specification

### Version

1.0

### Scope

Entire Ambient Physical AI Ecosystem

---

# Purpose

Ambient Physical AI is not only a collection of devices.

It is a distributed cognitive ecosystem.

To make system behavior understandable to users, developers, judges, and operators, all nodes should communicate their operational state through a common visual language.

This document defines the official Runtime State Language for Ambient Physical AI.

---

# Design Principles

The state language must be:

* simple;
* immediately recognizable;
* device independent;
* consistent across the ecosystem;
* applicable to displays, LEDs, dashboards, OLEDs, wearables and future interfaces.

---

# Runtime States

## 1. IDLE

### Color

Deep Blue

### Meaning

System available but inactive.

### Description

No active interaction is occurring.

The system is waiting for an event.

### Examples

* waiting for presence;
* waiting for user interaction;
* standby mode.

### Recommended Effects

```text
Steady light
Low intensity
```

---

## 2. PRESENCE

### Color

Cyan

### Meaning

Presence detected.

### Description

A person or entity has been detected by the environment.

### Examples

* ToF detection;
* motion detection;
* occupancy detection.

### Recommended Effects

```text
Short pulse
Medium intensity
```

---

## 3. LISTENING

### Color

Soft Purple

### Meaning

Listening for interaction.

### Description

The system is actively waiting for user input.

### Examples

* voice command;
* NFC presentation;
* touch interaction;
* context selection.

### Recommended Effects

```text
Slow breathing animation
```

---

## 4. THINKING

### Color

White / Ice Blue

### Meaning

Context processing.

### Description

The cognitive layer is analyzing information and generating a response.

### Examples

* AX630C reasoning;
* LLM inference;
* context evaluation;
* profile resolution.

### Recommended Effects

```text
Slow pulse
```

---

## 5. RESPONDING

### Color

Vibrant Blue

### Meaning

Response generation in progress.

### Description

The system is actively producing an output.

### Examples

* StackChan speaking;
* Voice Pyramid speaking;
* display update;
* action execution.

### Recommended Effects

```text
Continuous light
```

---

## 6. ALERT

### Color

Amber / Orange

### Meaning

Important event.

### Description

A condition requires attention but is not a failure.

### Examples

* context change;
* ambient notification;
* important system event.

### Recommended Effects

```text
Medium pulse
```

---

## 7. ERROR

### Color

Red

### Meaning

Failure or critical condition.

### Description

The system detected an abnormal condition.

### Examples

* communication failure;
* sensor failure;
* invalid identity;
* service unavailable.

### Recommended Effects

```text
Fast pulse
```

---

## 8. OFFLINE

### Color

Yellow

### Meaning

Local autonomous operation.

### Description

The node is operational but disconnected from higher-level services.

### Examples

* network unavailable;
* Cognitive Runtime unavailable;
* local-only mode.

### Recommended Effects

```text
Steady yellow
```

---

## 9. LEARNING

### Color

Evolution Gradient

Suggested:

```text
Cyan
→ Blue
→ Purple
```

### Meaning

Adaptation or knowledge evolution.

### Description

The system is learning, adapting or incorporating new information.

### Examples

* profile enrichment;
* memory update;
* contextual adaptation.

### Recommended Effects

```text
Gradient animation
```

---

# Animation Semantics

## Stable State

```text
Continuous light
```

Meaning:

```text
System stable
```

---

## Slow Pulse

```text
Processing
Waiting
Listening
```

---

## Fast Pulse

```text
Attention
Urgency
```

---

## Gradient Motion

```text
Evolution
Learning
Adaptation
```

---

# Device Mapping

The Runtime State Language may be implemented on:

```text
Display 7"
OLED SH1107
M5Dial
Atom Matrix
WS2812 RGB devices
Wearable Runtime Node
Voice Pyramid LEDs
Future devices
```

---

# Relationship with Architecture Layers

Runtime States are not architectural layers.

Architectural Layers:

```text
Presence
Identity
Cognition
Ambient Transformation
Expression
```

Runtime States:

```text
Idle
Presence
Listening
Thinking
Responding
Alert
Error
Offline
Learning
```

The same architectural layer may present different runtime states during execution.

Example:

```text
Identity Layer

Listening
↓
Thinking
↓
Responding
```

---

# Competition Demonstration Value

The Runtime State Language provides:

* immediate visual understanding;
* consistent behavior across devices;
* reduced explanation burden;
* stronger perception of system intelligence.

Judges can understand system status without reading documentation.

---

# MASTER Decision

Approved as:

```text
Ambient Physical AI Runtime State Language v1
```

This specification shall be considered the official visual runtime communication model for the Ambient Physical AI ecosystem.
