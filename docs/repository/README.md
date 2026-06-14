# Repository Organization

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

This document describes the official repository organization used by the Ambient Physical AI project.

The repository structure is intentionally aligned with the physical architecture, cognitive architecture, and development workflow of the project.

---

# Repository Philosophy

The repository follows four core principles:

## Demo First

Working demonstrations have priority over architectural complexity.

## Hardware First

Repository structure should reflect real hardware rather than hypothetical components.

## Incremental Development

New folders and modules should be introduced only when justified by actual implementation needs.

## Documentation Driven

All significant architectural decisions, discoveries, and milestones should be documented.

---

# Top-Level Structure

```text
ambient-physical-ai/

├── assets/
├── demos/
├── docs/
├── firmware/
├── hardware/
├── runtime/
├── scripts/
├── tools/
└── README.md
```

---

# assets

Project visual assets.

Purpose:

* architecture illustrations;
* presentation material;
* competition media;
* screenshots;
* project photography.

## assets/cinematic

Promotional and presentation images.

## assets/demos

Assets associated with demonstrations.

## assets/diagrams

Architecture diagrams and technical illustrations.

## assets/photos

Photographs of hardware and development activities.

## assets/renders

Conceptual renderings and visualizations.

## assets/screenshots

Screenshots captured during development.

### assets/screenshots/firmware

Firmware logs, terminal captures and validation results.

---

# demos

Complete demonstration scenarios.

Purpose:

* competition demonstrations;
* integration testing;
* system validation.

## demos/context-aware-cognitive-lab

Experimental environment used to validate context-aware cognitive interactions.

### demos/context-aware-cognitive-lab/assets

Assets specific to the demonstration.

---

# docs

Official project documentation.

## docs/architecture

Architecture descriptions and system-level documentation.

## docs/demos

Documentation related to demonstrations.

## docs/discoveries

Important technical discoveries made during development.

Examples:

* hardware behavior;
* protocol findings;
* integration lessons learned.

## docs/hardware

Hardware-related documentation.

## docs/milestones

Project milestones and progress tracking.

## docs/notes

Technical development notes and engineering journal entries.

## docs/references

External references, articles, specifications and supporting material.

## docs/repository

Repository organization documentation.

## docs/snapshots

Architectural snapshots and consolidated project state summaries.

Purpose:

* preserve project context;
* support future onboarding;
* record major decisions.

## docs/tutorials

Step-by-step tutorials and educational material.

---

# firmware

ESP-IDF firmware projects.

```text
firmware/

├── shared/
└── nodes/
```

## firmware/shared

Shared firmware resources.

Reserved for reusable firmware components.

## firmware/nodes

Node-specific firmware implementations.

---

# firmware/nodes/presence-node

Presence Layer implementation.

Purpose:

* human presence detection;
* proximity awareness;
* distance measurement.

Hardware:

* CoreS3 Lite
* Unit Mini ToF-90

### components/vl53l0x

VL53L0X driver and integration layer.

### main

Application entry point and node logic.

---

# firmware/nodes/ambient-runtime-node

Ambient Runtime implementation.

Purpose:

* environmental sensing;
* contextual feedback;
* display control;
* ambient adaptation.

Hardware:

* PoE-P4

### components/env_iv

ENV-IV environmental sensor driver.

### components/oled_sh1107

SH1107 OLED display driver.

### main

Ambient Runtime application logic.

---

# hardware

Hardware documentation and organization.

```text
hardware/
```

Purpose:

* hardware inventory;
* validation records;
* integration notes;
* photographs.

## hardware/ax630c

AX630C documentation and experiments.

## hardware/cores3-lite

CoreS3 Lite documentation and investigations.

## hardware/experiments

Hardware experiments and temporary investigations.

## hardware/m5dial

M5Dial documentation.

## hardware/poe-p4

PoE-P4 documentation.

## hardware/sensors

Sensor documentation.

### hardware/sensors/tof

Mini ToF-90 (VL53L0X) documentation.

#### photos

Validation and investigation photographs.

## hardware/stackchan

StackChan documentation.

## hardware/voice

Voice subsystem documentation.

---

# runtime

Software executed by higher-level runtime environments.

```text
runtime/

├── ambient/
├── cognitive/
├── infrastructure/
└── shared/
```

## runtime/ambient

Ambient Runtime services.

Responsible for physical environment adaptation.

## runtime/cognitive

Cognitive Runtime services.

Primary target:

* AX630C

### runtime/cognitive/stackflow

StackFlow implementation.

Responsible for:

* distributed coordination;
* contextual information flow;
* runtime communication.

## runtime/infrastructure

Supporting infrastructure services.

Potential future examples:

* MQTT broker;
* local databases;
* orchestration services.

## runtime/shared

Shared runtime libraries and common resources.

---

# scripts

Utility scripts.

Examples:

* automation;
* maintenance;
* code generation;
* deployment support.

---

# tools

Development support tools.

Purpose:

* helper utilities;
* project-specific tooling;
* engineering productivity support.

---

# Architectural Alignment

The repository structure mirrors the conceptual architecture:

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

Repository modules should evolve in a way that preserves this alignment.

---

# Repository Evolution Rules

Create new folders only when:

* a real implementation exists;
* a documented need exists;
* the folder will immediately contain useful content.

Avoid:

* speculative structures;
* premature abstractions;
* unused modules.

Prefer:

* simplicity;
* traceability;
* maintainability;
* incremental growth.

---

# Current Implementation Status

Implemented:

* Presence Node
* Ambient Runtime Node
* VL53L0X component
* ENV-IV component
* SH1107 OLED component

Validated:

* VL53L0X hardware validation
* VL53L0X ESP-IDF detection
* AX630C bring-up
* StackFlow exploration

Planned:

* Identity Node
* Voice Node
* Expression Node
* Camera Node
* Auxiliary Node

---

**Ambient Physical AI**

*Distributed Cognitive Ecosystem Powered by StackFlow*
