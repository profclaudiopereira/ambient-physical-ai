# User Journey — Context-Aware Cognitive Lab

## Demonstration Experience

This document describes the participant experience during the Ambient Physical AI demonstration.

The objective is to present the system from the participant's perspective rather than from the perspective of individual hardware or software components.

---

# Before Arrival

The environment operates in standby mode.

No participant is currently interacting with the system.

```text
Presence: Idle
Identity: Unknown
Context: Empty
Ambient Runtime: Waiting
Cognitive State: Idle
```

---

# Arrival

The participant approaches the demonstration area.

Without requiring any initial action, the LD2410C Presence Node detects human presence.

```text
Presence detected
```

At this moment, the participant realizes that the environment is aware that someone has arrived.

---

# Identification

The participant presents an NFC credential using the M5Dial and WS1850S NFC reader.

The system identifies the participant and activates the associated profile.

Example:

```text
User identified

Name: Claudio
Role: Owner
Context: Lab
```

The participant understands that the environment can distinguish different people and contexts.

---

# Context Awareness

The Identity Package is sent to the AX630C Cognitive Runtime.

The system now knows:

```text
Who arrived
Which profile is active
Which context is selected
```

instead of simply knowing that someone is present.

The environment transitions from detection to contextual understanding.

---

# Cognitive Processing

The Cognitive Runtime processes:

- identity;
- active context;
- environmental information;
- runtime state;
- available semantic services.

The participant observes the system transitioning through visible cognitive states.

Example:

```text
THINKING
RESPONDING
IDLE
```

The system is no longer reacting to an isolated sensor event.

It is interpreting context and coordinating the distributed ecosystem.

---

# Ambient Adaptation

The M5Stack Tab5 Ambient Runtime receives the updated context.

The environment responds through:

- contextual information on the main display;
- personalized content on the Mini OLED;
- local sensor information;
- visual environmental feedback;
- network and runtime status.

The environment changes according to the authenticated participant.

---

# Embodied and Multimodal Interaction

The Expression Layer becomes active.

The participant may observe:

- StackChan acting as an embodied research assistant;
- the Echo Pyramid delivering a personalized welcome;
- RGB nodes presenting runtime and profile states;
- the Wearable Haptic Node producing tactile feedback during response;
- the Cognitive Runtime Console displaying identity, context, health and runtime state.

The participant no longer interacts with an invisible software service.

The environment gains physical expression through coordinated devices.

---

# Conversational Interaction

The participant may use the Echo Pyramid to request a context change.

Example:

```text
Hi ESP, Meeting.
```

The Cognitive Runtime processes the request, updates the active context and redistributes the resulting state across the ecosystem.

The participant observes the environment adapting again without interacting separately with each device.

---

# User Perception

At this point, the participant should perceive:

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

- a sensor;
- a chatbot;
- a display;
- an ESP32 board;
- an isolated automation routine.

Ambient Physical AI demonstrates a distributed cognitive environment capable of:

```text
Perceiving
Recognizing
Understanding
Adapting
Interacting
```

through the collaboration of multiple specialized runtime nodes.

---

# Final Demonstration Statement

The participant does not interact with individual devices.

The participant interacts with an environment.

That environment detects presence, resolves identity, maintains context, performs cognitive processing and adapts its physical behavior accordingly.

This is the essence of Ambient Physical AI.
