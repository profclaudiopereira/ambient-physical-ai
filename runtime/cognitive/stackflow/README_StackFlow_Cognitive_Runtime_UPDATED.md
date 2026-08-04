# StackFlow Cognitive Runtime

## Ambient Physical AI

**Distributed Cognitive Ecosystem Powered by StackFlow**

---

# Overview

The **StackFlow Cognitive Runtime** is the semantic coordination layer of the Ambient Physical AI architecture.

It executes on the **AX630C + LLM Mate** platform and is responsible for transforming validated physical events into a normalized semantic representation that can be consumed by the remaining components of the ecosystem.

Rather than interacting directly with hardware peripherals, the Cognitive Runtime acts as the semantic core of the system.

Its responsibilities include:

- receiving validated runtime events;
- constructing the canonical runtime context;
- maintaining the current semantic state;
- generating normalized Semantic Events;
- distributing semantic information to runtime consumers;
- exposing semantic services for local runtime components and future MCP integrations.

The runtime is intentionally modular.

Each subsystem has a single well-defined responsibility, allowing the runtime to evolve without tightly coupling semantic processing to transport protocols or specific hardware devices.

---

# Relationship with M5Stack StackFlow

This repository is part of the **Ambient Physical AI** project and is implemented on top of the M5Stack StackFlow ecosystem.

The official StackFlow platform provides the embedded AI infrastructure, execution environment and supporting software for M5Stack AI hardware.

The implementation contained in this directory represents the Cognitive Runtime developed specifically for Ambient Physical AI and should not be interpreted as the StackFlow platform itself.

Within the Ambient Physical AI architecture, StackFlow provides the execution environment while the Cognitive Runtime implements the semantic coordination layer responsible for transforming validated physical events into normalized runtime knowledge.

---

# Purpose

The Cognitive Runtime separates semantic processing from device implementation.

Instead of allowing each hardware node to implement independent application logic, the runtime centralizes semantic interpretation inside the AX630C node and distributes normalized Semantic Events to the remaining components.

This approach provides:

- centralized semantic interpretation;
- decoupled hardware integration;
- reusable semantic services;
- normalized runtime contracts;
- independent integration adapters;
- reproducible engineering architecture.

---

# Design Principles

The current implementation follows several architectural principles that guided the development of the runtime.

## Semantic Centralization

Semantic interpretation belongs exclusively to the Cognitive Runtime.

Peripheral nodes remain responsible only for sensing, rendering or environmental interaction.

Examples:

- Identity Node authenticates users.
- Presence Node detects physical presence.
- Echo Pyramid renders speech.
- Ambient Runtime transforms the environment.

The Cognitive Runtime is responsible for interpreting these validated events and producing normalized semantic information.

---

## Separation of Responsibilities

Each module owns exactly one architectural responsibility.

Examples include:

| Component | Primary Responsibility |
|-----------|------------------------|
| Identity UDP Listener | Runtime ingress and pipeline coordination |
| Context Builder | Build the canonical Context Package |
| Context Registry | Maintain the Current Runtime Context |
| Semantic Event Generator | Generate normalized Semantic Events |
| Semantic Dispatcher | Route Semantic Events |
| Integration Adapters | Translate Semantic Events into device-specific protocols |
| Semantic Services | Expose runtime semantic information |

This separation intentionally minimizes coupling between runtime modules.

---

## Canonical Internal Contracts

The runtime operates through canonical semantic contracts instead of device-specific data structures.

The primary contracts are:

```text
Identity Package
        │
        ▼
Context Package
        │
        ▼
Semantic Event V1
```

Each contract represents a distinct stage of semantic abstraction.

Individual hardware implementations never communicate directly with each other through proprietary payloads.

---

## Adapter-Based Integration

Hardware-specific communication is isolated behind integration adapters.

Examples:

```text
Ambient Runtime Adapter

Echo Pyramid Adapter

StackChan Adapter

RGB Adapter
```

Adapters are responsible only for protocol translation and message delivery.

Semantic interpretation remains centralized inside the Cognitive Runtime.

---

## Observability Without Interference

Runtime observability is intentionally independent from semantic processing.

Execution state indicators do not participate in semantic decision making.

This separation ensures that failures in monitoring components never interrupt the semantic execution pipeline.

---

# Runtime Architecture

The Cognitive Runtime is organized as a layered architecture.

Each layer transforms information into a higher semantic abstraction while preserving a clear separation of responsibilities.

```text
                 StackFlow Cognitive Runtime

                     Runtime Ingress Layer
                              │
                              ▼
                  Semantic Modeling Layer
                              │
                              ▼
                   Semantic State Layer
                              │
                              ▼
                 Semantic Abstraction Layer
                              │
                              ▼
                  Semantic Distribution Layer
                              │
                              ▼
                  Runtime Integration Layer

────────────────────────────────────────────────────

              Independent Observability Layer
```

Each layer performs one specific responsibility and communicates through stable runtime contracts.

The implementation intentionally avoids allowing transport protocols, hardware drivers or presentation logic to cross semantic boundaries.

---

# High-Level Execution Flow

The current validated runtime execution flow is illustrated below.

```text
Identity Package
        │
        ▼
Identity UDP Listener
        │
        ▼
Context Builder
        │
        ▼
Context Registry
        │
        ├──────────────┐
        │              │
        ▼              ▼
Semantic Services   MCP Server
        │
        ▼
Semantic Event Generator
        │
        ▼
Semantic Dispatcher
        │
 ┌──────┼───────────────┬───────────────┐
 ▼      ▼               ▼               ▼
StackChan  Ambient Runtime  RGB Layer  Echo Pyramid
```

This execution flow represents the current validated implementation of the StackFlow Cognitive Runtime.

The following sections describe each architectural layer and its responsibilities in detail.

---

# Canonical Runtime Contracts

The StackFlow Cognitive Runtime is organized around three canonical runtime contracts.

These contracts represent successive levels of semantic abstraction.

Rather than allowing each subsystem to exchange device-specific payloads, every stage transforms information into a more stable and reusable representation.

The complete semantic pipeline is therefore defined by the following contracts:

```text
Identity Package
        │
        ▼
Context Package
        │
        ▼
Semantic Event V1
```

Each contract has a distinct responsibility within the runtime.

---

# Identity Package

The **Identity Package** is the runtime ingress contract.

It is produced by the Identity Layer after successful user authentication and represents a validated physical identity event.

The Cognitive Runtime never performs NFC authentication itself.

Identity validation remains entirely outside the StackFlow runtime.

The runtime assumes that every received Identity Package already represents a validated authentication event.

Typical information includes:

- authenticated profile;
- user identifier;
- user role;
- active environment;
- NFC information;
- source node.

A simplified representation is shown below.

```text
Identity Package

Profile
Role
Environment
NFC Information
Source
```

The Identity Package is intentionally close to the originating hardware because it represents the interface between the physical world and the semantic runtime.

Its responsibility ends once the runtime constructs the canonical Context Package.

---

# Context Package

The **Context Package** is the canonical semantic representation maintained by the Cognitive Runtime.

Unlike the Identity Package, it is no longer tied to a particular hardware device.

Instead, it represents the current semantic understanding of the runtime.

The Context Builder transforms every validated Identity Package into a normalized Context Package.

Current implementation:

```text
Context Package

schema_version
package_type

who
where
when
what
why

source
```

The Context Package follows a structured 5W semantic model.

| Section | Responsibility |
|----------|----------------|
| **Who** | Active identity currently known by the runtime. |
| **Where** | Current semantic environment and location information. |
| **When** | Runtime timestamps associated with the current context. |
| **What** | Current validated runtime activity. |
| **Why** | Semantic intent and future reasoning information. |

The Context Package becomes the semantic source of truth for the entire Cognitive Runtime.

No downstream component consumes the original Identity Package directly.

Instead, all semantic processing begins from the Context Package.

---

# Current Runtime Context

The Context Registry maintains exactly one active Context Package.

This object is referred to throughout the runtime as the **Current Runtime Context**.

Its purpose is to represent the current semantic state of the Cognitive Runtime.

```text
Validated Context Package
            │
            ▼
Current Runtime Context
```

The registry intentionally stores only the latest validated semantic state.

It does not maintain:

- historical sessions;
- event history;
- multiple simultaneous contexts;
- persistent storage.

Whenever a new validated Context Package is generated, it replaces the previous runtime state.

This design keeps the semantic state deterministic, lightweight and immediately available to all runtime components.

---

# Shared Runtime State

The Current Runtime Context is shared by multiple runtime subsystems.

```text
                 Current Runtime Context
                          │
          ┌───────────────┴───────────────┐
          │                               │
          ▼                               ▼
Semantic Services                 MCP Runtime
```

This shared semantic state allows independent runtime modules to operate without directly depending on transport layers or hardware-specific implementations.

---

# Semantic Event V1

The **Semantic Event V1** is the canonical communication contract used by the Cognitive Runtime.

While the Context Package represents semantic state, the Semantic Event represents semantic communication.

The Semantic Event Generator transforms the Current Runtime Context into one or more normalized Semantic Events.

Current implementation supports two canonical event types.

```text
identity_authenticated

context_changed
```

Each event carries normalized semantic information rather than device-specific payloads.

Typical semantic information includes:

- normalized role;
- access level;
- runtime capabilities;
- environment;
- runtime activity;
- semantic state.

The Semantic Event therefore becomes the standard communication object exchanged between runtime components.

---

# Authorization Normalization

One important responsibility of the Semantic Event Generator is authorization normalization.

The runtime converts source-specific roles into normalized semantic roles before distributing events.

For example:

```text
Identity Layer
        │
        ▼
owner
learner
visitor
researcher
technician
        │
        ▼
Semantic Authorization Model
        │
        ▼
normalized role
access level
capabilities
```

This normalization ensures that downstream consumers receive a consistent authorization model regardless of how identities were originally represented.

Consequently, integration adapters and future runtime services never need to reinterpret Identity Layer roles.

---

# Semantic Context vs Semantic Events

Although closely related, the Context Package and Semantic Event V1 serve different architectural purposes.

| Context Package | Semantic Event |
|-----------------|----------------|
| Represents semantic state. | Represents semantic communication. |
| Stored inside the Context Registry. | Generated on demand. |
| Shared by local runtime services. | Distributed to runtime consumers. |
| Updated whenever runtime context changes. | Generated whenever semantic communication is required. |

This distinction is fundamental to the StackFlow architecture.

The runtime always maintains semantic state independently from semantic communication.

---

# Runtime Contracts Summary

The complete semantic transformation performed by the Cognitive Runtime can therefore be summarized as follows.

```text
Validated Identity

        │

        ▼

Identity Package

        │

        ▼

Context Builder

        │

        ▼

Context Package

        │

        ▼

Current Runtime Context

        │

        ▼

Semantic Event Generator

        │

        ▼

Semantic Event V1

        │

        ▼

Semantic Dispatcher
```

The following section describes the internal runtime layers responsible for implementing this execution pipeline.

---

# Runtime Layers

The StackFlow Cognitive Runtime is intentionally organized as a layered architecture.

Each layer owns one well-defined engineering responsibility and communicates with adjacent layers through stable runtime contracts.

This separation minimizes coupling, improves maintainability and allows new capabilities to be introduced without redesigning the entire runtime.

The current implementation consists of six logical layers.

```text
                 StackFlow Cognitive Runtime

                     Runtime Ingress Layer
                              │
                              ▼
                  Semantic Modeling Layer
                              │
                              ▼
                   Semantic State Layer
                              │
                              ▼
                 Semantic Abstraction Layer
                              │
                              ▼
                  Semantic Distribution Layer
                              │
                              ▼
                  Runtime Integration Layer

────────────────────────────────────────────────────

              Independent Observability Layer
```

Each layer is described below.

---

# Runtime Ingress Layer

## Purpose

The Runtime Ingress Layer is responsible for receiving validated runtime events and initiating the semantic processing pipeline.

Current implementation:

```text
identity_udp_listener.py
```

This module is the primary execution entry point of the Cognitive Runtime.

Although originally introduced as a UDP listener, its current responsibility is significantly broader.

The Runtime Ingress Layer is responsible for:

- receiving Identity Packages;
- receiving authorized Context Change Requests;
- coordinating semantic pipeline execution;
- updating the Current Runtime Context;
- generating Semantic Events;
- dispatching Semantic Events;
- refreshing Ambient Context information;
- initiating personalized voice interaction;
- publishing Runtime State;
- starting the embedded MCP server.

It therefore functions as the runtime coordinator for the complete semantic pipeline.

---

## Runtime Inputs

The current implementation accepts two runtime contracts.

```text
Identity Package

Context Change Request
```

Identity Packages originate from the Identity Layer.

Context Change Requests originate from authorized runtime interactions, such as voice-driven environment changes.

Each input follows a dedicated execution path while sharing the same semantic state maintained by the Context Registry.

---

# Semantic Modeling Layer

## Purpose

The Semantic Modeling Layer transforms validated runtime inputs into canonical semantic representations.

Current implementation:

```text
context_builder.py
```

This layer performs semantic normalization.

Rather than preserving transport-oriented payloads, it constructs a standardized Context Package used throughout the remainder of the runtime.

The Context Builder does not perform:

- event routing;
- transport;
- device communication;
- semantic distribution.

Its sole responsibility is to produce the canonical Context Package.

---

## Context Builder

The Context Builder generates a complete semantic representation following the 5W model.

```text
Who

Where

When

What

Why
```

This representation becomes the canonical runtime context used by every downstream component.

Human-readable messages remain available exclusively for debugging and compatibility with previous engineering laboratories.

They are no longer considered the primary output of this module.

---

# Semantic State Layer

## Purpose

The Semantic State Layer maintains the current semantic state of the runtime.

Current implementation:

```text
context_registry.py
```

The Context Registry stores exactly one active Context Package.

```text
Current Runtime Context
```

This object represents the current semantic state of the Cognitive Runtime.

Whenever a new validated Context Package is produced, the previous runtime state is replaced.

The registry intentionally avoids:

- historical persistence;
- event history;
- multiple simultaneous contexts;
- session management.

Its responsibility is limited to maintaining the latest validated semantic state.

---

## Shared Runtime Context

The Context Registry acts as the shared semantic memory of the runtime.

```text
                 Current Runtime Context
                          │
          ┌───────────────┴───────────────┐
          │                               │
          ▼                               ▼
Semantic Services                 MCP Runtime
```

The registry isolates semantic knowledge from transport protocols and hardware-specific implementations.

Every runtime component requiring semantic state retrieves it through the registry instead of directly accessing incoming runtime messages.

---

# Semantic Abstraction Layer

## Purpose

The Semantic Abstraction Layer transforms runtime state into reusable semantic information.

Current implementation:

```text
semantic_services.py

semantic_event_generator.py
```

This layer has two complementary responsibilities.

The first is exposing semantic information through stable local runtime services.

The second is transforming semantic state into normalized Semantic Events.

---

## Semantic Services

Semantic Services provide a stable local interface over the Current Runtime Context.

Available services include:

```text
get_runtime_status()

get_current_context()

get_current_identity()

get_current_environment()

get_current_activity()
```

These services intentionally avoid transport-specific logic.

Instead, they expose semantic information already maintained by the Context Registry.

Future MCP integrations are expected to consume these services without modifying their internal implementation.

---

## Voice Decision

The Semantic Services layer is also responsible for selecting personalized identity voice messages.

An important architectural principle of the current implementation is preserved throughout the runtime.

```text
Cognitive Runtime

decides

what should be spoken

↓

Expression Layer

renders

the resulting audio
```

Expression Nodes never determine speech content.

They receive already validated runtime decisions produced by the Cognitive Runtime.

---

## Semantic Event Generator

The Semantic Event Generator transforms the Current Runtime Context into normalized Semantic Event V1 objects.

Current implementation supports two canonical semantic events.

```text
identity_authenticated

context_changed
```

One important responsibility of this module is authorization normalization.

Identity-specific roles are converted into runtime authorization information before events are distributed.

The resulting Semantic Events contain:

- normalized role;
- access level;
- runtime capabilities;
- semantic context;
- runtime activity.

The Semantic Event Generator owns semantic meaning only.

It does not implement transport protocols or device communication.

---

# Semantic Distribution Layer

## Purpose

The Semantic Distribution Layer is responsible for delivering normalized semantic information to the remaining components of the Ambient Physical AI ecosystem.

Unlike previous runtime layers, this layer no longer creates or modifies semantic meaning.

Its responsibility is limited to routing already validated Semantic Event V1 objects.

Current implementation:

```text
semantic_dispatcher.py
```

---

## Semantic Dispatcher

The Semantic Dispatcher is the central routing component of the Cognitive Runtime.

Its responsibility is intentionally minimal.

It does **not**:

- generate semantic events;
- modify semantic events;
- interpret runtime decisions;
- implement transport protocols;
- communicate directly with hardware devices.

Instead, it performs only one operation:

```text
Receive

↓

Validate

↓

Route
```

Each registered integration adapter receives exactly the same normalized Semantic Event.

This guarantees that every runtime consumer operates on a common semantic representation rather than proprietary payloads.

---

## Adapter Registration

Integration adapters are registered dynamically.

```text
Semantic Dispatcher
        │
        ▼
register_adapter()
        │
        ▼
Runtime Adapter
```

The dispatcher therefore has no knowledge of:

- network protocols;
- hardware devices;
- message serialization;
- transport implementations.

It depends only on the adapter interface.

This significantly reduces coupling between semantic processing and hardware integration.

---

## Fault Isolation

Each adapter executes independently.

Failures occurring during message delivery are isolated to the corresponding adapter.

```text
Semantic Dispatcher
        │
        ├────► Adapter A
        │
        ├────► Adapter B
        │
        ├────► Adapter C
        │
        └────► Adapter D
```

A delivery failure never interrupts semantic processing or prevents other adapters from receiving the same Semantic Event.

This behavior intentionally preserves runtime robustness.

---

# Runtime Integration Layer

## Purpose

The Runtime Integration Layer connects the semantic runtime to the remaining nodes of the Ambient Physical AI ecosystem.

Every integration component follows the same architectural principle.

```text
Semantic Event

↓

Adapter

↓

Transport Protocol

↓

External Runtime
```

Semantic interpretation always remains inside the Cognitive Runtime.

Adapters perform only protocol translation and message delivery.

---

# Ambient Runtime Adapter

Current implementation:

```text
ambient_runtime_notifier.py
```

The Ambient Runtime Adapter delivers normalized Semantic Events to the Ambient Runtime running on the Tab5.

Current transport:

```text
UDP
```

The adapter preserves the semantic information produced by the runtime.

It forwards:

- event type;
- payload;
- semantic context;
- source information.

without introducing additional semantic interpretation.

The Ambient Runtime therefore receives the same canonical semantic representation generated inside the AX630C Cognitive Runtime.

---

# Echo Pyramid Adapter

Current implementation:

```text
echo_pyramid_adapter.py
```

The Echo Pyramid integration provides two independent communication channels.

## Semantic Command Channel

The first channel delivers compact semantic commands.

```text
Semantic Event

↓

Echo Pyramid Adapter

↓

UDP

↓

Echo Pyramid
```

Typical commands include:

```text
WELCOME

RED

GREEN

BLUE

OFF
```

These commands represent semantic actions already interpreted by the Cognitive Runtime.

---

## Voice Audio Channel

Speech delivery follows a completely independent execution path.

```text
Speech Text

↓

TTS Engine

↓

PCM Audio

↓

TCP

↓

Echo Pyramid
```

The Cognitive Runtime determines the speech content.

The configured text-to-speech engine running on the AX630C generates PCM audio.

The Echo Pyramid receives only validated audio and is responsible exclusively for playback.

Speech synthesis remains configurable through deployment settings and is intentionally independent from any specific TTS engine.

---

## Architectural Principle

The Echo Pyramid never decides semantic behavior.

Instead:

```text
Cognitive Runtime

↓

Semantic Decision

↓

Speech Selection

↓

Echo Pyramid

↓

Audio Rendering
```

This separation preserves a clear distinction between semantic reasoning and user interaction.

---

# StackChan Adapter

Current implementation:

```text
stackchan_notifier.py
```

The StackChan Adapter encapsulates Semantic Events for future StackChan integration.

Current execution mode remains intentionally conservative.

```text
Dry Run
```

The adapter prepares runtime notifications without introducing additional semantic processing.

Future production integration will remain based on normalized Semantic Events rather than direct hardware coupling.

---

# RGB Expression Adapter

Current implementation:

```text
rgb_strip_notifier.py
```

The RGB adapter receives Semantic Events and translates them into expression-layer lighting commands.

The adapter does not determine colors or semantic meaning.

It only converts normalized runtime information into the communication protocol required by the RGB node.

This preserves the same separation of responsibilities adopted throughout the runtime.

---

# Cognitive Runtime Console Integration

Current implementation:

```text
cognitive_runtime_console_notifier.py
```

The Cognitive Runtime Console integration provides a dedicated observability
channel between the AX630C and the M5Stack CoreS3 Lite Runtime Console.

Validated implementation characteristics:

- UART transport through `/dev/ttyS1`;
- 115200 bit/s, 8N1, no flow control;
- newline-delimited UTF-8 JSON (`runtime_status`);
- complete runtime snapshots;
- integration with `identity_udp_listener.py`;
- Python standard library only (no PySerial dependency).

The Runtime Console is intentionally independent from the semantic execution
pipeline. Its responsibility is to publish runtime observability without
participating in semantic interpretation, routing or decision making.

---

# Independent Runtime Services

Not every runtime communication belongs to the Semantic Event pipeline.

Some services intentionally operate outside semantic distribution.

---

# Runtime State Channel

Current implementation:

```text
runtime_state_notifier.py
```

The Runtime State Channel provides execution observability.

Unlike Semantic Events, Runtime State messages describe the internal execution status of the Cognitive Runtime.

Typical states include:

```text
idle

thinking

responding

error
```

These messages are delivered to the dedicated Runtime State Indicator.

```text
Runtime State

↓

Runtime State Notifier

↓

UDP

↓

StickC Plus2
```

This channel is intentionally independent from semantic communication.

Its purpose is monitoring rather than semantic coordination.

Failures in Runtime State delivery never interrupt the semantic execution pipeline.

---

# Ambient Context Service

Current implementation:

```text
services/
└── ambient_context/
```

This service refreshes profile-aware contextual information presented by the Ambient Runtime.

Its execution occurs alongside the semantic pipeline but does not participate in Semantic Event generation.

For implementation details, deployment instructions, communication contracts and validation procedures, see:

```text
services/ambient_context/README.md
```

---

# Embedded MCP Runtime

The Cognitive Runtime starts the embedded MCP server within the same execution process.

This design allows both runtime components to share the same Current Runtime Context.

```text
                Current Runtime Context
                         │
        ┌────────────────┴────────────────┐
        │                                 │
        ▼                                 ▼
Semantic Runtime                  Embedded MCP Runtime
```

The MCP runtime therefore accesses semantic information without requiring duplicated transport layers or synchronization mechanisms.

This shared-memory architecture keeps semantic services consistent across both runtime environments.

---

# Complete Runtime Architecture

The complete StackFlow Cognitive Runtime architecture currently implemented by Ambient Physical AI is summarized below.

```text
                     StackFlow Cognitive Runtime

                 Runtime Ingress Layer
                          │
                          ▼
              Semantic Modeling Layer
                          │
                          ▼
               Semantic State Layer
                          │
                          ▼
           Semantic Abstraction Layer
                          │
                          ▼
           Semantic Distribution Layer
                          │
                          ▼
             Runtime Integration Layer
        ┌────────────┬────────────┬────────────┬────────────┐
        ▼            ▼            ▼            ▼
 StackChan     Ambient Runtime   RGB Layer   Echo Pyramid

──────────────────────────────────────────────────────────────

             Independent Runtime Services

 Runtime State   Runtime Console   Ambient Context   Embedded MCP
```

The following sections describe repository organization, validation procedures, engineering evidence and future development roadmap.

---

# Repository Organization

The StackFlow Cognitive Runtime is organized to separate semantic processing, runtime services, integration adapters and engineering artifacts.

Current directory structure:

```text
runtime/
└── cognitive/
    └── stackflow/
        │
        ├── README.md
        │
        ├── identity_udp_listener.py
        ├── context_builder.py
        ├── context_registry.py
        ├── semantic_event.py
        ├── semantic_event_generator.py
        ├── semantic_dispatcher.py
        ├── semantic_services.py
        │
        ├── ambient_runtime_notifier.py
        ├── echo_pyramid_adapter.py
        ├── rgb_strip_notifier.py
        ├── runtime_state_notifier.py
        ├── cognitive_runtime_console_notifier.py
        ├── stackchan_notifier.py
        │
        ├── stackchan_mcp_server.py
        ├── semantic_mcp_tools.py
        │
        ├── services/
        │   └── ambient_context/
        │
        ├── labs/
        │
        └── notes/
```

The repository organization intentionally mirrors the runtime architecture described in the previous sections.

---

# Core Runtime Components

The following modules constitute the semantic core of the Cognitive Runtime.

| Module | Responsibility |
|----------|----------------|
| `identity_udp_listener.py` | Runtime ingress and pipeline coordination |
| `context_builder.py` | Builds the canonical Context Package |
| `context_registry.py` | Maintains the Current Runtime Context |
| `semantic_services.py` | Exposes semantic runtime services |
| `semantic_event_generator.py` | Produces normalized Semantic Events |
| `semantic_dispatcher.py` | Routes Semantic Events to adapters |

Together these modules implement the semantic core of StackFlow.

---

# Integration Components

The following modules connect the runtime to external systems.

| Module | Responsibility |
|----------|----------------|
| `ambient_runtime_notifier.py` | Tab5 integration |
| `echo_pyramid_adapter.py` | Semantic commands and voice delivery |
| `rgb_strip_notifier.py` | Expression Layer integration |
| `stackchan_notifier.py` | StackChan integration |
| `runtime_state_notifier.py` | Runtime observability |
| `cognitive_runtime_console_notifier.py` | Cognitive Runtime Console observability over UART |

These modules intentionally remain independent from semantic processing.

---

# Runtime Services

Additional runtime services are organized separately from the semantic pipeline.

Current services include:

```text
services/
└── ambient_context/
```

These services extend runtime functionality without modifying the semantic execution pipeline.

---

# Laboratory Artifacts

The directory

```text
labs/
```

contains engineering investigation artifacts produced during runtime development.

These artifacts are intentionally preserved for:

- engineering reproducibility;
- protocol investigation;
- MCP experiments;
- validation history;
- future research.

Files contained in this directory are **not** part of the production execution pipeline.

---

# Runtime Validation

The current implementation has been validated incrementally through integration with real hardware.

Validated runtime components include:

```text
Identity Package Reception ............. PASS

Context Builder ........................ PASS

Context Registry ....................... PASS

Semantic Services ...................... PASS

Semantic Event Generator ............... PASS

Semantic Dispatcher .................... PASS

Ambient Runtime Adapter ................ PASS

Echo Pyramid Adapter ................... PASS

Runtime State Channel .................. PASS
Cognitive Runtime Console ............... PASS

Shared Runtime Context ................. PASS
```

---

# Runtime Validation Flow

Current validation follows the execution sequence below.

```text
Identity Node

↓

Identity Package

↓

AX630C Cognitive Runtime

↓

Context Package

↓

Semantic Event

↓

Runtime Adapters

↓

External Runtime Nodes
```

Every stage of this pipeline has been validated independently before being integrated into the complete runtime.

---

# Engineering Principles

The current implementation follows the engineering principles adopted throughout Ambient Physical AI.

## Semantic Before Transport

Semantic interpretation always precedes transport.

Hardware protocols never participate in semantic reasoning.

---

## Single Responsibility

Each runtime module owns one architectural responsibility.

No module simultaneously performs:

- semantic modeling;
- routing;
- protocol translation;
- hardware communication.

---

## Stable Runtime Contracts

The runtime communicates through stable semantic contracts.

Current contracts include:

```text
Identity Package

Context Package

Semantic Event V1
```

This allows runtime modules to evolve independently while preserving interoperability.

---

## Loose Coupling

Hardware integrations occur exclusively through adapters.

The semantic runtime remains independent from:

- UDP;
- TCP;
- MCP transport;
- RGB protocols;
- audio protocols;
- device-specific implementations.

---

## Fault Isolation

Peripheral communication failures never interrupt semantic processing.

Integration adapters report delivery failures without affecting runtime execution.

This behavior preserves semantic consistency even during temporary network or hardware failures.

---

# Current Runtime Status

The StackFlow Cognitive Runtime currently represents the validated semantic baseline of Ambient Physical AI.

Implemented capabilities include:

- semantic runtime coordination;
- Cognitive Runtime Console UART observability;
- canonical Context Package generation;
- Current Runtime Context management;
- Semantic Event generation;
- semantic event distribution;
- integration adapters;
- runtime observability;
- embedded MCP runtime;
- Ambient Context refresh services.

The implementation is intentionally focused on semantic correctness, modularity and reproducibility.

---

# Future Work

The following capabilities are planned for future development and should not be interpreted as implemented functionality.

Examples include:

- additional semantic services;
- expanded MCP capabilities;
- richer Context Package enrichment;
- long-term context persistence;
- additional runtime integrations;
- semantic decision services;
- distributed runtime coordination.

These topics belong to future engineering milestones and remain outside the scope of the validated implementation documented in this README.

---

# References

Official M5Stack StackFlow documentation:

https://docs.m5stack.com/en/stackflow/overview

Ambient Physical AI repository:

```text
runtime/cognitive/stackflow/
```

This README documents the validated implementation currently maintained in this directory.


# Related Documentation

For the engineering evolution of this subsystem, validation history, laboratory investigations and implementation milestones, see:

```text
ENGINEERING_HISTORY.md
```