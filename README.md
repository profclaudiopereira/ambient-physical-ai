# Ambient Physical AI

## Distributed Cognitive Ecosystem Powered by StackFlow

> Perceive. Identify. Understand. Decide. Transform. Express.

Ambient Physical AI is an exploration of what happens when Ambient Intelligence, Physical AI, Edge Computing, Large Language Models, Context-Aware Computing and Embodied Interaction converge into a single ecosystem.

Rather than building isolated devices, dashboards or chatbots, the project investigates how environments themselves can become intelligent, contextual and responsive.

The long-term vision is simple:

> The environment understands the person.

---

# Inspiration

Ambient Physical AI is strongly inspired by the vision of ubiquitous computing proposed by Mark Weiser.

His famous observation remains one of the most important design principles behind this project:

> "The most profound technologies are those that disappear. They weave themselves into the fabric of everyday life until they are indistinguishable from it."

While traditional systems focus on interfaces, Ambient Physical AI focuses on environments.

Instead of interacting with a computer, the user interacts with a space capable of perception, cognition and expression.

---

# The Problem

Modern intelligent systems often remain trapped inside screens.

Even advanced AI systems typically:

* wait for explicit commands;
* lack physical awareness;
* ignore environmental context;
* cannot perceive presence;
* cannot adapt their behavior to individuals;
* struggle to express their internal state in meaningful ways.

Ambient Physical AI investigates a different approach:

```text
Environment
↓
Perception
↓
Identity
↓
Context
↓
Cognition
↓
Physical Transformation
↓
Expression
```

---

# Ecosystem Vision

The current ecosystem vision is:

```text
Person
   ↓
Presence
   ↓
Identity
   ↓
Physical Context
   ↓
Cognition
   ↓
Ambient Transformation
   ↓
Expression
   ↓
Experience
```

This flow emerged through multiple architectural discoveries, hardware validations and runtime investigations.

---

# Architectural Layers

## Presence Layer

Question:

> Is someone here?

Responsibilities:

* presence detection;
* distance measurement;
* approach detection;
* occupancy awareness.

Current baseline:

```text
AtomS3 Lite
+
Unit Mini ToF-90
(VL53L0X)
```

Status:

```text
Operational Baseline
```

---

## Identity Layer

Question:

> Who is here?

Responsibilities:

* NFC identification;
* profile recognition;
* context selection;
* identity confirmation;
* Identity Package generation.

Current baseline:

```text
M5Dial
+
WS1850S NFC
```

Validated capabilities:

```text
UID Reading
Profile Mapping
Identity Package Generation
FreeRTOS Runtime
```

Example:

```json
{
  "type":"identity_package",
  "profile":{
    "id":"claudio",
    "role":"owner"
  }
}
```

Status:

```text
Operational Baseline
```

---

## Physical Context Layer

One of the most important discoveries of the project.

Initial model:

```text
Presence
↓
Identity
↓
Cognition
```

Refined model:

```text
Presence
↓
Identity
↓
Physical Context
↓
Cognition
```

Physical artifacts become contextual markers.

Examples:

* identity cards;
* environment cards;
* intent cards;
* semantic objects;
* contextual anchors.

This transforms NFC from simple authentication into contextual interaction.

---

## Cognitive Layer

Question:

> What does this situation mean?

Current platform:

```text
AX630C
+
LLM Mate
+
StackFlow
```

Responsibilities:

* reasoning;
* context interpretation;
* multimodal processing;
* memory;
* decision making;
* future RAG capabilities.

Current status:

```text
Runtime Validated
Integration Phase Next
```

Major discoveries:

* Ubuntu 22.04 runtime validated;
* local LLM inference validated;
* multimodal AI services identified;
* TCP integration endpoint discovered;
* StackFlow protocol investigation completed.

---

## Ambient Transformation Layer

Question:

> How should the environment respond?

Current platform:

```text
PoE-P4
```

Responsibilities:

* environmental adaptation;
* display management;
* ambient orchestration;
* physical synchronization;
* contextual responses.

The Ambient Runtime Node transforms the environment itself.

---

## Expression Layer

Question:

> How does the environment express itself?

Current platforms:

```text
StackChan
Voice Pyramid
```

Future platforms:

```text
Atom Matrix
NanoC6 RGB Bar
Mini OLED
Wearable Devices
```

Responsibilities:

* embodiment;
* voice;
* visual feedback;
* emotional expression;
* environmental communication.

---

# StackFlow

StackFlow is not simply a messaging system.

It is not MQTT.

It is not middleware.

It is not a device protocol.

Current definition:

## Distributed Cognitive Coordination Fabric

StackFlow coordinates:

```text
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
```

across a distributed ecosystem of physical devices.

---

# Key Architectural Discoveries

## Discovery 01 — Hardware Before Assumptions

Many architectural decisions were only possible because real hardware was tested before assumptions were made.

Principle:

```text
Hardware First
Architecture Second
```

---

## Discovery 02 — Identity is More Than Authentication

The M5Dial evolved from:

```text
RFID Reader
```

into:

```text
Identity
+
Intent
+
Context Selection
```

---

## Discovery 03 — Physical Context

Context can be represented through physical artifacts.

Examples:

```text
Cards
Objects
Tokens
Markers
```

allowing context to become tangible.

---

## Discovery 04 — StackChan

Initial assumption:

```text
Robot + LLM
```

Current assessment:

```text
Physical AI Agent Platform
```

StackChan is not the Cognitive Runtime.

It is an embodiment and expression platform.

---

## Discovery 05 — Runtime State Language

Ambient Physical AI is developing a common expression language for runtime states.

Examples:

```text
Thinking
→ White Pulse

Responding
→ Blue

Alert
→ Orange

Error
→ Red
```

The goal is to make cognition visible.

---

# Accessibility and Human-Centered Design

An important idea emerged during project discussions involving accessibility and game design.

Modern games often implement dedicated color-blind modes:

```text
Deuteranopia
Protanopia
Tritanopia
```

Ambient Physical AI extends this idea beyond interfaces.

Future vision:

```text
Identity
↓
Accessibility Profile
↓
Environment Adaptation
↓
Personalized Expression
```

Potential adaptations:

* alternative color palettes;
* high-contrast displays;
* audio feedback;
* wearable haptic feedback;
* adaptive environmental responses.

The objective is simple:

> The environment adapts to the person.

---

# Wearable Computing

The ecosystem is also exploring wearable interaction.

Reference platform:

```text
M5StickC Plus 2
+
Vibration HAT
```

Future role:

```text
Wearable Runtime Node
```

Potential capabilities:

* silent notifications;
* haptic feedback;
* contextual alerts;
* accessibility support;
* ambient interaction.

Expression should not rely only on vision.

Future expression may combine:

```text
Visual
+
Audio
+
Haptic
```

---

# Current Operational Baselines

## Presence Layer

```text
AtomS3 Lite
+
VL53L0X
```

Status:

```text
Operational Baseline
```

---

## Identity Layer

```text
M5Dial
+
WS1850S NFC
```

Status:

```text
Operational Baseline
```

---

## Cognitive Runtime

```text
AX630C
+
LLM Mate
```

Status:

```text
Validated
```

---

## Ambient Runtime

```text
PoE-P4
```

Status:

```text
Active Development
```

---

# Repository Structure

```text
ambient-physical-ai/

├── firmware/
├── runtime/
├── docs/
├── assets/
├── hardware/
├── demos/
├── tools/
├── scripts/
└── .github/
```

Additional details can be found throughout the repository documentation.

---

# Competition

Project developed for:

```text
M5Stack Global Innovation Contest 2026
```

Target submission:

```text
27 July 2026
```

Current focus:

```text
Presence
↓
Identity
↓
Cognition Integration
```

---

# Roadmap

Near-term objective:

```text
Presence Event
↓
Identity Package
↓
AX630C Cognitive Runtime
↓
Decision
```

Future objective:

```text
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
Experience
```

---

# Design Principles

Ambient Physical AI follows a set of principles refined throughout the project:

* Demo First. Complexity Later.
* Explore First. Integrate Later.
* Hardware Before Assumptions.
* Architecture Before Integration.
* Local First.
* Distributed Cognition.
* Human-Centered Design.
* Real Systems Over Simulations.

---

# Final Vision

Ambient Physical AI is not a single device.

It is not a chatbot.

It is not an automation platform.

It is an investigation into how environments can perceive, understand and respond to people through distributed cognition embodied in the physical world.

The long-term goal is not to build smarter devices.

The long-term goal is to build environments that understand the people within them.

---

# Additional Architectural Notes

## Dual NFC Identity Strategy

Ambient Physical AI supports two complementary identity acquisition strategies.

### Strategy 1 — UID Mapping

Current operational baseline.

```text
NTAG216
↓
UID Read
↓
UID Mapping
↓
Profile
↓
Identity Package
```

Validated examples:

```text
8804DC32 → Claudio / owner

88048667 → Student / learner
```

Advantages:

* simple;
* robust;
* fast;
* independent of card content.

Status:

```text
VALIDATED
```

---

### Strategy 2 — NDEF Identity

Future evolution.

```text
NTAG216
↓
Read UID
↓
Read NDEF
↓
Identity Package
```

The system follows the rule:

```text
If NDEF exists
    Use NDEF

Else
    Use UID Mapping
```

This allows identity information to be carried directly by the card while preserving compatibility with the validated UID Mapping approach.

Status:

```text
PLANNED
```

---

# Expression Layer Prototypes

Although the Expression Layer has not yet entered the main integration path, several hardware platforms have already been validated independently.

Current laboratory platforms include:

```text
ESP32-S3
RGB Rings
RGB Bars
Atom Matrix
Mini OLED Displays
```

Characteristics:

```text
Wi-Fi Connected
Independent Runtime
Distributed Operation
```

These devices are capable of expressing system state through visual feedback patterns and may later become distributed Expression Nodes within the ecosystem.

Current classification:

```text
Expression Layer Prototypes
```

---

# Wearable Runtime Node

Ambient Physical AI is also exploring wearable interaction.

Reference platform:

```text
M5StickC Plus 2
+
Vibration HAT
```

Potential role:

```text
Wearable Runtime Node
```

Future capabilities:

```text
Silent Notifications
Haptic Feedback
Accessibility Support
Ambient Alerts
Personal Context Delivery
```

This extends the Expression Layer beyond visual and audio channels.

Future expression model:

```text
Visual
+
Audio
+
Haptic
```

The long-term objective is to allow the environment to communicate through the most appropriate modality for each individual.

---

# Advanced Expression Branch

Current ecosystem roadmap:

```text
Core Path

Presence
↓
Identity
↓
Cognition
↓
Ambient Runtime
↓
Expression
```

Additional expression platforms:

```text
Advanced Expression Branch

RGB Nodes
Wearable Node
Accessibility Adaptation
```

This branch investigates personalized and adaptive forms of environmental expression.

---

# Adaptive Environment Vision

One emerging principle of Ambient Physical AI is:

```text
The environment does not merely understand the person.

The environment also chooses how to express itself to that person.
```

Examples:

```text
Identity
↓
Accessibility Profile
↓
Adaptive Expression
```

Possible adaptations:

```text
Alternative Color Schemes
High Contrast Modes
Audio Feedback
Wearable Haptics
Context-Aware Visual Feedback
```

This principle is inspired by accessibility strategies commonly found in modern games and extends them to physical environments.
