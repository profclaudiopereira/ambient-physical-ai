# Ambient Physical AI

## Distributed Cognitive Ecosystem Powered by StackFlow

Ambient Physical AI is a research and engineering project that explores how modern AI, contextual awareness, edge computing and physical environments can be integrated into a distributed cognitive ecosystem.

The project combines embedded systems, Edge AI, Large Language Models (LLMs), multimodal interaction, contextual sensing and ambient computing concepts to create environments capable of perceiving, understanding and adapting to human presence and intent.

---

# Vision

The project is inspired by the ideas of **Mark Weiser**, the creator of Ubiquitous Computing, and by subsequent developments in:

* Context-Aware Computing
* Ambient Intelligence
* Edge AI
* Physical AI
* Distributed Cognitive Systems

Rather than creating a single intelligent device, Ambient Physical AI explores how intelligence can emerge from the collaboration of multiple specialized nodes distributed throughout an environment.

The ultimate objective is to make computing less visible and more integrated into everyday experiences.

---

# Architectural Principles

Ambient Physical AI follows a layered cognitive model:

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
Experience
```

Each layer contributes a specific capability to the ecosystem.

---

# Reference Architecture

```text
PERSON
    ↓
Unit Mini ToF
    ↓ I2C
M5Dial
    ↓ Wi-Fi
AX630C + LLM Mate
    ↓ Ethernet
PoE-P4
```

Supporting nodes:

```text
StackChan
Voice Pyramid + AtomS3R
Atom Matrix
CoreS3 Lite
```

Environmental peripherals:

```text
ENV-IV
Unit Mini OLED
Future I2C Sensors
```

---

# Runtime Nodes

## Presence Node

Responsible for detecting human presence and physical activity.

Current implementation:

```text
Unit Mini ToF-90
```

Functions:

* presence detection;
* proximity awareness;
* physical activity sensing.

---

## Identity Node

Responsible for user identification and contextual personalization.

Current implementation:

```text
M5Dial V1.1
+
NFC
```

Functions:

* NFC identification;
* contextual mode selection;
* user intent input.

---

## Cognitive Runtime Node

The cognitive core of the architecture.

Current implementation:

```text
AX630C
+
LLM Mate
```

Responsibilities:

* contextual reasoning;
* memory management;
* multimodal processing;
* StackFlow Runtime execution;
* future RAG integration.

Validated capabilities include:

* Ubuntu 22.04;
* Ethernet networking;
* SSH access;
* local AI inference;
* StackFlow Runtime.

---

## Ambient Runtime Node

Responsible for transforming the physical environment according to contextual decisions.

Current implementation:

```text
PoE-P4
```

Responsibilities:

* environmental adaptation;
* contextual feedback;
* sensor aggregation;
* display control;
* future lighting and ambient orchestration.

Current local architecture:

```text
PoE-P4
    ↓
PCA9548A
    ├── ENV-IV
    ├── Unit Mini OLED
    └── Future I2C Devices
```

---

## Expression Node

Responsible for giving a physical presence to the cognitive system.

Current implementation:

```text
StackChan
```

Responsibilities:

* visual expression;
* embodiment;
* contextual interaction;
* emotional communication.

---

## Voice Node

Responsible for conversational interaction.

Current implementation:

```text
Voice Pyramid
+
AtomS3R
```

Responsibilities:

* speech input;
* speech output;
* conversational experiences;
* multimodal interaction.

---

# StackFlow

StackFlow acts as the distributed coordination fabric of the ecosystem.

Its purpose is to connect runtime nodes and enable the flow of:

* events;
* context;
* decisions;
* environmental actions.

Conceptually:

```text
Presence
    ↓
Identity
    ↓
Context Package
    ↓
Cognitive Runtime
    ↓
Ambient Runtime
    ↓
Experience
```

---

# Repository Structure

Detailed repository organization documentation:

```text
docs/repository/README.md
```

```text
ambient-physical-ai/

├── firmware/
├── runtime/
├── hardware/
├── docs/
├── demos/
├── assets/
├── tools/
├── scripts/
└── README.md
```

---

# Current Status

Validated:

* Presence Layer foundations
* VL53L0X hardware validation
* VL53L0X ESP-IDF detection
* StackChan exploration
* AX630C bring-up
* StackFlow protocol discovery
* Local LLM inference
* Context injection experiments

In Progress:

* Ambient Runtime Node bring-up
* Identity Node implementation
* Distributed runtime integration

Planned:

* Long-term memory layer
* Context retrieval services
* Multi-node StackFlow orchestration
* Competition demonstration environment

---

# Competition

This project is being developed as part of the preparation for the:

**M5Stack Global Innovation Contest 2026**

Current internal milestone:

```text
Target Submission Date
27 July 2026
```

---

# Philosophy

Ambient Physical AI is not a collection of connected devices.

It is an exploration of how physical environments can become context-aware, cognitively coordinated and capable of meaningful interaction.

The goal is to create environments that do not merely react to commands, but that perceive, understand and adapt to the people who inhabit them.

---

## Author

Claudio Pereira

Embedded Systems Engineer • Electronics Engineer • Professor

---

**Ambient Physical AI**

*Distributed Cognitive Ecosystem Powered by StackFlow*