# Scenario — Context-Aware Cognitive Lab

## Demonstration Narrative

This document describes the official demonstration scenario for Ambient Physical AI.

The objective is to show how a distributed cognitive ecosystem can perceive, identify, understand and adapt to human presence inside a contextual environment.

---

# Environment

The demonstration takes place inside a technology laboratory.

The laboratory contains:

- an LD2410C Radar Presence Node;
- an M5Dial Identity Node with WS1850S NFC;
- the AX630C Cognitive Runtime with LLM Mate;
- the M5Stack Tab5 Ambient Runtime;
- StackChan as an embodied research assistant;
- the Echo Pyramid with AtomS3R as the voice interface;
- distributed RGB Expression Nodes;
- a Wearable Haptic Node;
- a dedicated Cognitive Runtime Console.

All components cooperate through the Ambient Physical AI architecture.

---

# Initial State

The environment is idle.

No participant is currently interacting with the system.

```text
Presence: Idle
Identity: Unknown
Context: Empty
Ambient Runtime: Waiting
Cognitive State: Idle
```

---

# Step 1 — Presence Detection

A person enters the environment.

The LD2410C radar detects human presence and initiates the contextual interaction.

```text
Presence detected
```

A presence event is transmitted to the Identity Node.

---

# Step 2 — Identity Recognition

The participant presents an NFC credential using the M5Dial and WS1850S NFC reader.

The Identity Node resolves the credential and associates it with a validated profile.

Example:

```json
{
  "profile_id": "claudio",
  "role": "owner",
  "context": "Lab"
}
```

The Identity Package becomes available to the ecosystem.

---

# Step 3 — Context Creation

The Identity Package is forwarded to the AX630C Cognitive Runtime.

The Runtime updates the Current Runtime Context using the authenticated profile, selected context and available environmental information.

Example:

```json
{
  "profile_id": "claudio",
  "context": "Lab",
  "authenticated": true
}
```

---

# Step 4 — Cognitive Processing

The AX630C processes:

- identity;
- active context;
- environmental information;
- runtime state;
- available semantic services.

The Runtime transitions through cognitive states such as:

```text
IDLE
THINKING
RESPONDING
```

Runtime snapshots and semantic events are distributed to the corresponding consumers.

---

# Step 5 — Ambient Transformation

The M5Stack Tab5 Ambient Runtime receives the contextual information.

Possible actions include:

- updating the main Ambient Runtime interface;
- presenting personalized information on the Mini OLED;
- displaying local sensor information;
- exposing network and runtime status;
- coordinating profile-based environmental adaptation.

The environment changes according to the authenticated participant and the active context.

---

# Step 6 — Multimodal Expression

The Expression Layer presents the cognitive state through multiple physical modalities.

Validated behaviors include:

- StackChan contextual interaction;
- personalized welcome through the Echo Pyramid;
- synchronized RGB state presentation;
- runtime state presentation on dedicated displays;
- a short haptic pulse during the `RESPONDING` state;
- runtime observability through the Cognitive Runtime Console.

Example:

```text
Welcome, Claudio.
Context: Lab.
```

The interaction becomes visible, audible, embodied and tactile.

---

# Step 7 — Voice-Driven Context Change

The Echo Pyramid with AtomS3R supports wake-word-driven context change requests.

Example:

```text
Hi ESP, Meeting.
```

The request is forwarded to the Cognitive Runtime, which validates the request, updates the active context and redistributes the resulting state to the ecosystem.

---

# Complete Flow

```text
Person
  ↓
Presence Detection
  ↓
Identity Recognition
  ↓
Identity Package
  ↓
Current Runtime Context
  ↓
Cognitive Processing
  ↓
Semantic Event Distribution
  ↓
Ambient Transformation
  ↓
Multimodal Expression
```

---

# Key Demonstration Message

The objective of the demonstration is not to show individual devices.

The objective is to show how multiple specialized nodes cooperate to create a context-aware cognitive environment.

The environment perceives.

The environment recognizes.

The environment understands.

The environment adapts.

The environment interacts.

This is the essence of Ambient Physical AI.
