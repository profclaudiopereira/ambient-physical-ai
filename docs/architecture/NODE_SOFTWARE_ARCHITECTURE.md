# NODE_SOFTWARE_ARCHITECTURE.md

# Ambient Physical AI

## Standard Software Architecture for ESP-IDF Nodes

**Status:** Draft V1.0\
**Project:** Ambient Physical AI --- Distributed Cognitive Ecosystem
Powered by StackFlow

------------------------------------------------------------------------

# Purpose

This document defines the reference software architecture for every
ESP-IDF node in the Ambient Physical AI ecosystem.

The objective is to ensure all firmware follows the same engineering
principles, making the system easier to understand, maintain, validate
and extend.

------------------------------------------------------------------------

# Design Principles

-   Separation of concerns
-   Single responsibility per layer
-   Hardware abstraction
-   Modular design
-   Testability
-   Reusability

------------------------------------------------------------------------

# Reference Architecture

``` text
Application
    │
    ▼
Receiver
    │
    ▼
Consumer
    │
    ▼
Processor
    │
    ▼
Domain Effects / Services
    │
    ▼
Hardware Controller
    │
    ▼
ESP-IDF Drivers
    │
    ▼
Hardware
```

------------------------------------------------------------------------

# Layer Responsibilities

## Application

-   app_main()
-   startup
-   task creation
-   supervision

## Receiver

Receives raw external data.

Examples: - UDP - NFC - Radar - UART

## Consumer

Parses, validates and filters data.

Produces internal events.

## Processor

Implements node behaviour.

Examples: - Ambient Processor - Expression Processor - Identity
Processor - Presence Processor

## Domain Effects / Services

Represents semantic actions independently of hardware.

Example:

``` c
rgb_effect_identity_authenticated();
```

## Hardware Controller

Abstracts the physical device.

Examples: - RGB Controller - Display Controller - Audio Controller

## ESP-IDF Drivers

Vendor/ESP-IDF drivers only.

------------------------------------------------------------------------

# Example Mapping

## Ambient Runtime

``` text
Application
 ↓
UDP Receiver
 ↓
Semantic Consumer
 ↓
Ambient Processor
 ↓
Display / Sensors
```

## RGB Strip Node

``` text
Application
 ↓
UDP Receiver (future)
 ↓
Semantic Consumer (future)
 ↓
Expression Processor (future)
 ↓
RGB Effects
 ↓
RGB Controller
 ↓
led_strip
```

## Identity Node

``` text
Application
 ↓
NFC Receiver
 ↓
Identity Consumer
 ↓
Identity Processor
 ↓
Display / Buzzer
```

## Presence Node

``` text
Application
 ↓
Radar Receiver
 ↓
Presence Processor
 ↓
UDP Sender
```

------------------------------------------------------------------------

# Architectural Rules

-   Layers communicate only with adjacent layers.
-   Hardware details never leak into upper layers.
-   Semantic meaning is independent of hardware implementation.
-   New nodes should follow this reference architecture unless a
    documented exception exists.

------------------------------------------------------------------------

# Benefits

-   Consistent architecture
-   Easier maintenance
-   Reusable components
-   Lower coupling
-   Better documentation
-   Better reproducibility

------------------------------------------------------------------------

# Status

This document defines the reference software architecture for ESP-IDF
firmware in the Ambient Physical AI project and should evolve as new
reusable patterns are validated.
