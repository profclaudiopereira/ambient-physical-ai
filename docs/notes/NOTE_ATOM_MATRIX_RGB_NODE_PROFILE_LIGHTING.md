# NOTE_ATOM_MATRIX_RGB_NODE_PROFILE_LIGHTING

# Ambient Physical AI

## Engineering Technical Note

### Atom Matrix RGB Node — Profile-Aware Ambient Lighting and Platform Calibration

**Version:** 2.0

**Date:** July 2026

---

# Purpose

This technical note documents the engineering decisions, implementation changes and platform calibration work performed during the evolution of the Atom Matrix RGB Node.

Unlike the node README, which describes the operational behavior of the firmware, this document records the engineering rationale behind the implementation, the architectural decisions that guided the work and the calibration methodology adopted for the M5Stack Atom Matrix platform.

The objective is to preserve engineering knowledge for future maintenance, reproducibility and architectural evolution.

---

# Scope

This document focuses exclusively on the engineering work related to:

- semantic profile-aware ambient lighting;
- Expression Layer architectural evolution;
- platform-specific RGB calibration;
- engineering validation;
- future maintenance.

Operational information such as firmware organization, build instructions and runtime behavior is documented in the node README.

---

# Engineering Context

The Atom Matrix RGB Node is one of the hardware implementations that compose the **Expression Layer** of the Ambient Physical AI architecture.

Its responsibility is to translate normalized Semantic Events into deterministic visual expressions using the integrated 5 × 5 RGB matrix of the M5Stack Atom Matrix.

The node performs no cognitive processing.

It only consumes semantic information already produced by the Cognitive Runtime.

During the initial implementation, the node already contained the complete Expression Layer processing pipeline.

```text
Application Startup
        ↓
RGB Controller
        ↓
Wi-Fi Station
        ↓
Semantic Receiver
        ↓
Semantic Consumer
        ↓
Expression Processor
        ↓
RGB Effects
```

The firmware architecture itself was already considered stable.

The engineering work documented in this note focused on improving semantic expressiveness while preserving the validated software architecture.

---

# Engineering Objective

The objective of this engineering activity was not to redesign the firmware.

Instead, the implementation sought to preserve the validated semantic behavior already adopted throughout the Expression Layer while adapting the visual output to the optical characteristics of the Atom Matrix hardware.

The work introduced profile-aware ambient lighting without modifying the existing communication architecture.

More specifically, the implementation introduced:

- authenticated user profile selection;
- semantic context support;
- platform-specific visual calibration;
- deterministic fallback behavior;
- centralized RGB calibration values.

The existing communication pipeline, networking modules and hardware abstraction layers were intentionally preserved.

---

# Initial Implementation

Before this engineering work, the firmware already supported semantic state visualization.

The available semantic states were:

| Semantic State | Visual Expression |
|---------------|-------------------|
| Boot | Generic startup indication |
| Idle | Generic idle indication |
| Presence Detected | Presence indication |
| Identity Authenticated | Generic authenticated indication |
| Processing | Processing indication |
| System Error | Error indication |

Although functionally correct, the implementation still treated every authenticated user identically.

The node did not distinguish individual profiles and therefore could not produce personalized ambient lighting.

The normalized Semantic Event payload was also ignored.

Consequently, authenticated profile information produced by the Cognitive Runtime was not yet available to the visual expression pipeline.

---

# Engineering Strategy

Rather than redesigning the firmware, the implementation extended the existing architecture while preserving every validated software layer.

The engineering strategy followed four principles.

## Preserve the Existing Architecture

The software organization was maintained without introducing new processing layers or modifying module responsibilities.

The existing processing pipeline remained unchanged.

```text
Semantic Receiver
        ↓
Semantic Consumer
        ↓
Expression Processor
        ↓
RGB Effects
        ↓
RGB Controller
```

---

## Extend the Semantic Contract

Instead of creating new event types, the existing normalized Semantic Event was extended with optional contextual information.

This allowed authenticated profile information to be propagated without changing the communication protocol.

---

## Preserve Hardware Independence

Semantic meaning remains independent from hardware implementation.

Only the visual calibration layer became platform dependent.

This allows different RGB devices to express identical semantic meaning while using hardware-specific RGB values.

---

## Maintain Backward Compatibility

All existing Semantic Events remain valid.

Events that do not contain profile information continue to be processed normally.

Only authenticated events optionally consume additional semantic context.

No existing communication path required modification.

---

# Implementation Overview

The engineering work affected only the modules directly involved in semantic interpretation and visual mapping.

Modified modules:

```text
expression_processor
rgb_effects
semantic_consumer
atom_matrix_rgb_node
```

The following infrastructure modules remained architecturally unchanged:

```text
wifi_station
semantic_receiver
rgb_controller
```

Likewise, no modification was required to the hardware abstraction layer or to the network communication architecture.

The implementation therefore represents an evolutionary enhancement of the Expression Layer rather than a redesign of the firmware.

---

# Semantic Context Extension

One of the most important architectural improvements introduced during this engineering activity was the addition of an optional semantic context to the Expression Processor.

Previously, semantic processing depended exclusively on the normalized event name.

The processing interface evolved from:

```c
int expression_processor_process(const char *event_type);
```

to:

```c
int expression_processor_process(
    const char *event_type,
    const semantic_context_t *context
);
```

The new semantic context currently contains:

```c
typedef struct
{
    const char *user_id;
} semantic_context_t;
```

The context is intentionally optional.

Semantic events such as:

- boot
- idle
- presence_detected
- processing
- system_error

continue to operate without additional information.

Only events requiring authenticated identity information consume the semantic context.

This approach preserves backward compatibility while allowing future semantic attributes to be introduced without modifying the event-processing contract.

---

# Semantic Consumer Evolution

The Semantic Consumer became responsible for interpreting the optional semantic payload before forwarding the event to the Expression Processor.

The current processing sequence is:

```text
UDP Packet
        ↓
JSON Parsing
        ↓
Semantic Event Validation
        ↓
Target Validation
        ↓
Optional Payload Inspection
        ↓
Semantic Context Construction
        ↓
Expression Processor
```

The implementation validates:

- message type;
- semantic event;
- target;
- optional payload.

When available, the authenticated user identifier is extracted from:

```json
payload.user_id
```

and forwarded through the semantic context.

Events without payload continue to be processed normally.

---

# Authenticated Profile Selection

The Expression Processor now treats the semantic event:

```text
identity_authenticated
```

as a profile-aware event.

Instead of producing a generic authenticated indication, the node selects one of the validated ambient lighting profiles.

Current mappings are:

| Identifier | Selected Profile |
|------------|------------------|
| claudio | Claudio |
| herminio | Hermínio |
| mariana | Mariana |
| student | Mariana |
| visitor | Visitor |
| guest | Visitor |

Unknown identifiers intentionally fall back to the Visitor profile.

This deterministic fallback guarantees that every authenticated event always produces a valid visual response.

---

# RGB Effects Organization

The RGB Effects module was reorganized into three abstraction levels.

## Primitive Effects

These functions directly represent basic RGB colors and remain available primarily for diagnostics and internal composition.

```text
Red
Green
Blue
White
Off
```

---

## Semantic Effects

These functions represent system states rather than physical colors.

```text
Boot

Idle

Presence Detected

Identity Authenticated

Processing

System Error
```

The Expression Processor never manipulates RGB values directly.

Instead, it selects semantic effects that encapsulate all visual decisions.

---

## Authenticated Profiles

Authenticated-user expressions are intentionally separated from generic semantic states.

Current profiles include:

```text
Claudio

Hermínio

Mariana

Visitor
```

This separation allows future profile evolution without affecting semantic processing or hardware control.

---

# Platform Calibration

Although the semantic behavior is identical across the Expression Layer, different RGB devices exhibit different optical characteristics.

For this reason, the Atom Matrix firmware introduced a dedicated calibration layer.

Instead of reproducing identical numerical RGB values, the implementation reproduces the same perceived ambient expression.

The calibrated RGB values are centralized within the RGB Effects module.

This design provides several engineering advantages.

- calibration values remain isolated;
- semantic processing remains unchanged;
- hardware control remains unchanged;
- future adjustments affect only one section of the firmware.

The visual behavior therefore becomes platform-specific while the semantic meaning remains platform-independent.

---

# Visual Validation

The Atom Matrix implementation was validated by comparing its visual output against the already validated RGB Strip Node.

Although identical semantic events produced correct functional behavior, visual comparison demonstrated that identical RGB values generated different perceived colors.

Observed differences included:

- insufficient blue intensity for the Claudio profile;
- excessive red contribution in the Hermínio profile;
- warmer-than-expected white for the Mariana profile.

These observations confirmed that semantic equivalence does not imply optical equivalence.

The calibration values were therefore adjusted specifically for the Atom Matrix hardware until equivalent ambient perception was achieved.

---

# Engineering Principles

The resulting implementation follows several engineering principles.

## Semantic Independence

Semantic meaning is independent of hardware.

The Cognitive Runtime communicates only semantic information.

No hardware-specific RGB values are propagated through the communication protocol.

---

## Platform-Specific Calibration

Visual calibration belongs exclusively to the target hardware.

Different RGB devices may require different numerical values while preserving identical semantic behavior.

---

## Deterministic Processing

Every supported Semantic Event produces one deterministic visual response.

Unknown profiles always produce the validated Visitor profile.

Unsupported semantic targets are ignored.

This behavior prevents undefined visual states.

---

## Layer Isolation

Each software layer owns one engineering responsibility.

```text
Semantic Receiver
        ↓
Semantic Consumer
        ↓
Expression Processor
        ↓
RGB Effects
        ↓
RGB Controller
```

No layer bypasses another.

This organization greatly simplifies maintenance, testing and future evolution.

---

# Validation Results

The engineering activity successfully validated:

- firmware compilation;
- ESP32 target configuration;
- Wi-Fi connectivity;
- UDP semantic communication;
- Semantic Event validation;
- semantic context propagation;
- authenticated profile selection;
- platform-specific RGB calibration;
- deterministic fallback behavior;
- integration with the validated Expression Layer.

No architectural redesign of the firmware was required.

The implementation represents an evolutionary enhancement built upon the existing validated architecture.

---

# Future Engineering Evolution

The current implementation intentionally produces stable ambient expressions.

Future engineering work may extend the visual layer with:

- fade transitions;
- breathing effects;
- low-frequency pulsing;
- profile-specific animation timing;
- configurable brightness policies;
- additional semantic context attributes.

These future enhancements should remain confined to the RGB Effects layer, preserving the validated separation between semantic processing, visual mapping and hardware control.

---

# Engineering Conclusion

The work documented in this note transformed the Atom Matrix RGB Node from a generic semantic indicator into a profile-aware ambient expression device while preserving the validated Expression Layer architecture.

The implementation introduced semantic context support, authenticated-user profile selection and hardware-specific visual calibration without modifying the communication protocol, network architecture or hardware abstraction layers.

As a result, the Atom Matrix RGB Node now behaves consistently with the other validated Expression Layer nodes while maintaining platform-specific calibration and preserving the modular software architecture adopted throughout the Ambient Physical AI project.