# User Journey — Context-Aware Cognitive Lab

## Demonstration Experience

This document describes the visitor experience during the Ambient Physical AI demonstration.

The objective is to present the system from the user's perspective rather than from the perspective of the hardware or software components.

---

# Before Arrival

The environment is operating in standby mode.

No user is currently interacting with the system.

```text
Environment State

Presence: Idle
Identity: Unknown
Context: Empty
Ambient Runtime: Waiting
```

---

# Arrival

The visitor approaches the demonstration area.

Without requiring any action from the user, the environment detects presence.

```text
Presence detected
```

At this moment the visitor realizes that the environment is aware that someone has arrived.

---

# Identification

The visitor presents an NFC credential using M5Dial.

The system identifies the user.

Example:

```text
User identified

Name: Claudio
Role: Owner
Mode: Developer
```

The visitor understands that the environment can distinguish different people and contexts.

---

# Context Awareness

The identity information is sent to the Cognitive Runtime.

The system now knows:

```text
Who arrived
```

instead of simply:

```text
Someone arrived
```

Context becomes available.

The environment transitions from detection to understanding.

---

# Cognitive Processing

The AX630C processes:

* identity;
* role;
* context;
* environmental information.

A contextual decision is generated.

Example:

```text
Developer mode activated.
```

The visitor observes that the system is no longer reacting to a sensor event.

The system is interpreting context.

---

# Ambient Adaptation

The Ambient Runtime Node receives the decision.

Environmental components react.

Possible actions include:

* visual feedback;
* environmental indicators;
* contextual information;
* display updates.

The environment changes according to the identified user.

---

# Embodied Interaction

StackChan becomes active.

Example:

```text
Welcome back Claudio.
Developer mode activated.
```

The interaction gains a physical presence.

The visitor no longer interacts with an invisible system.

The environment now has an embodiment.

---

# Conversational Interaction

Voice Pyramid becomes available.

The visitor may continue naturally.

Example:

```text
What should I work on today?
```

The environment responds using the Cognitive Runtime.

The interaction becomes multimodal.

---

# User Perception

At this point the visitor should perceive:

```text
The environment noticed me.
The environment recognized me.
The environment understood my context.
The environment adapted itself.
The environment interacted with me.
```

---

# Core Message

Ambient Physical AI is not demonstrating:

* a sensor;
* a chatbot;
* a display;
* an ESP32 board.

Ambient Physical AI demonstrates a distributed cognitive environment capable of:

```text
Perceiving
Understanding
Adapting
Interacting
```

through the collaboration of multiple specialized runtime nodes.

---

# Final Demonstration Statement

The visitor does not interact with individual devices.

The visitor interacts with an environment.

That environment perceives presence, understands identity, reasons about context and adapts its behavior accordingly.

This is the essence of Ambient Physical AI.
