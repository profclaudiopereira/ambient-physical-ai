# TECHNICAL_NOTE_ATOM_MATRIX_RGB_NODE_PROFILE_LIGHTING.md

# Ambient Physical AI

## Technical Note

### Atom Matrix RGB Node — Profile-Aware Ambient Lighting and Platform Calibration

Version: 1.0

Date: July 2026

---

# Purpose

This technical note documents the engineering work performed on the Atom Matrix RGB Node to align its behavior with the other RGB Expression Nodes of the Ambient Physical AI project.

The objective was to preserve the same semantic behavior, authenticated-user profiles, system states, and ambient-lighting intent already validated in the RGB Strip Node, while adapting the physical expression layer to the specific optical and electrical characteristics of the M5Stack Atom Matrix 5 × 5 LED display.

This document is intended to support:

- future README updates;
- maintenance and calibration work;
- reproducibility;
- architectural review;
- later evolution of visual effects, including rhythm and animation.

---

# Node Location

Firmware directory:

```text
firmware/nodes/expression-node/atom-matrix-rgb-node/
```

Main source directory:

```text
firmware/nodes/expression-node/atom-matrix-rgb-node/main/
```

---

# Hardware Baseline

The node uses:

```text
M5Stack Atom Matrix
ESP32-PICO-D4
5 × 5 integrated RGB LED matrix
25 addressable LEDs
```

The correct ESP-IDF target is:

```text
esp32
```

The Atom Matrix is not an ESP32-S3 device.

---

# Initial State

Before this work, the Atom Matrix RGB Node already had the basic Expression Layer structure:

```text
Application startup
        ↓
RGB controller initialization
        ↓
Wi-Fi station initialization
        ↓
UDP semantic receiver
        ↓
Semantic consumer
        ↓
Expression processor
        ↓
RGB effects
```

However, the behavior was still generic.

The previous mappings were approximately:

| Semantic state | Previous effect |
|---|---|
| Boot | LEDs off |
| Idle | LEDs off |
| Presence detected | Green |
| Identity authenticated | Blue |
| Processing | White |
| System error | Red |

The node did not yet distinguish authenticated users.

It also did not consume:

```json
payload.user_id
```

from the normalized semantic event.

---

# Objective of the Modification

The Atom Matrix Node had to reproduce the same semantic behavior already implemented in the RGB Strip Node.

The required authenticated profiles were:

| Profile | Visual temperature | Intensity | Dominant color |
|---|---|---:|---|
| Claudio | Neutral-cool | Low | Soft blue |
| Hermínio | Warm | Medium | Yellow / amber |
| Mariana | Neutral | Medium-high | Warm white |
| Visitor | Neutral | Medium | Soft cyan |

The required system states were:

| State | Ambient expression |
|---|---|
| Idle | Very soft blue |
| Presence detected | Soft yellow |
| Error | Persistent red with limited intensity |

The intended flow is:

```text
Presence detected
        ↓
Soft approach lighting
        ↓
Identity authenticated
        ↓
Apply authenticated profile
        ↓
Maintain ambient lighting
```

The selected profile remains active until:

- another authenticated user is received; or
- a new semantic event returns the environment to another state, such as idle.

---

# Files Modified

The following files were updated in the Atom Matrix firmware:

```text
main/
├── atom_matrix_rgb_node.c
├── expression_processor.c
├── expression_processor.h
├── rgb_effects.c
├── rgb_effects.h
└── semantic_consumer.c
```

The following existing modules were preserved:

```text
main/
├── rgb_controller.c
├── rgb_controller.h
├── semantic_receiver.c
├── semantic_receiver.h
├── wifi_station.c
└── wifi_station.h
```

The build configuration was also preserved:

```text
CMakeLists.txt
idf_component.yml
```

No hardware driver redesign was performed.

---

# Semantic Context Extension

## expression_processor.h

The Expression Processor API was extended to receive optional semantic context.

A new structure was introduced:

```c
typedef struct
{
    const char *user_id;
} semantic_context_t;
```

The previous API:

```c
int expression_processor_process(const char *event_type);
```

was replaced by:

```c
int expression_processor_process(
    const char *event_type,
    const semantic_context_t *context
);
```

This allows the node to preserve the normalized semantic event while also receiving profile information.

The context is optional.

Events such as:

```text
boot
idle
presence_detected
processing
system_error
```

can be processed with:

```c
context == NULL
```

---

# Application Entry Point Update

## atom_matrix_rgb_node.c

Because the Expression Processor now accepts a semantic context, local calls in the application entry point were updated.

Examples:

```c
expression_processor_process("boot", NULL);
```

```c
expression_processor_process("idle", NULL);
```

```c
expression_processor_process("system_error", NULL);
```

No change was made to the initialization sequence.

The startup flow remains:

```text
RGB controller
        ↓
Boot expression
        ↓
Wi-Fi station
        ↓
Wait for network
        ↓
Idle expression
        ↓
Semantic receiver
```

---

# Semantic Payload Processing

## semantic_consumer.c

The Semantic Consumer was updated to inspect the optional event payload.

Supported event format:

```json
{
  "type": "semantic_event",
  "event": "identity_authenticated",
  "target": "rgb_strip",
  "payload": {
    "user_id": "mariana"
  }
}
```

The implementation now:

1. validates the JSON document;
2. validates `type`;
3. validates `event`;
4. validates `target`;
5. reads the optional `payload`;
6. extracts `payload.user_id`;
7. creates a `semantic_context_t`;
8. forwards the event and context to the Expression Processor.

Conceptual flow:

```text
UDP JSON
        ↓
cJSON parse
        ↓
Validate semantic_event
        ↓
Read event
        ↓
Read target
        ↓
Read payload.user_id
        ↓
semantic_context_t
        ↓
expression_processor_process()
```

The payload remains optional.

This preserves compatibility with semantic events that do not carry profile information.

---

# Target Compatibility

The Atom Matrix Node preserves the same target contract used by the RGB Strip Node.

Accepted targets:

```text
rgb_strip
expression_layer
```

Although the physical device is now the Atom Matrix, the existing target was preserved intentionally to avoid breaking the validated Cognitive Runtime path.

A future architecture review may introduce device-specific or capability-based target names, but that change was not required for this replication task.

---

# Profile Selection

## expression_processor.c

The Expression Processor now treats:

```text
identity_authenticated
```

as a profile-aware semantic event.

The authenticated identifier is obtained from:

```c
context->user_id
```

The current profile mapping is:

```text
claudio
    ↓
rgb_effects_profile_claudio()

herminio
    ↓
rgb_effects_profile_herminio()

mariana or student
    ↓
rgb_effects_profile_mariana()

visitor or guest
    ↓
rgb_effects_profile_visitor()
```

Unknown or absent user identifiers fall back to:

```text
Visitor profile
```

This fallback guarantees deterministic behavior and prevents an unsupported profile from leaving the expression layer in an undefined visual state.

---

# RGB Effects Organization

## rgb_effects.h

The interface now exposes:

### Primitive effects

```c
rgb_effects_red()
rgb_effects_green()
rgb_effects_blue()
rgb_effects_white()
rgb_effects_off()
```

### Semantic state effects

```c
rgb_effects_boot()
rgb_effects_idle()
rgb_effects_presence_detected()
rgb_effects_identity_authenticated()
rgb_effects_processing()
rgb_effects_error()
```

### Profile-specific effects

```c
rgb_effects_profile_claudio()
rgb_effects_profile_herminio()
rgb_effects_profile_mariana()
rgb_effects_profile_visitor()
```

This separation preserves a clean boundary between:

- basic physical color output;
- system-state expression;
- authenticated-user expression.

---

# Initial Profile Replication

The first Atom Matrix implementation reused the same numerical RGB values previously validated in the RGB Strip Node.

Initial values included:

```text
Claudio  = (0, 1, 4)
Hermínio = (8, 4, 0)
Mariana  = (12, 10, 7)
Visitor  = (0, 8, 8)
```

The semantic behavior worked correctly.

The node:

- received the normalized event;
- extracted `user_id`;
- selected the correct profile;
- changed the 5 × 5 matrix;
- remained operational on the network.

However, visual comparison showed that identical numerical RGB values did not produce identical perceived colors across the two hardware platforms.

---

# Visual Validation

The Atom Matrix and RGB Strip were tested simultaneously.

The observed differences included:

## Claudio

Expected:

```text
Soft blue
Low intensity
```

Observed on Atom Matrix:

```text
Very weak or nearly invisible illumination
```

## Hermínio

Expected:

```text
Yellow / amber
Medium intensity
```

Observed on Atom Matrix:

```text
Excessively red appearance
```

## Mariana

Expected:

```text
Warm white
Medium-high intensity
```

Observed on Atom Matrix:

```text
More orange than the RGB Strip reference
```

These results confirmed that semantic equivalence does not guarantee optical equivalence when different LED assemblies are used.

---

# Platform-Specific Calibration

A second version of `rgb_effects.c` was created specifically for the Atom Matrix.

The calibration values were centralized at the beginning of the file.

This avoids scattering raw RGB values throughout the effect functions.

Current calibration block:

```c
#define PROFILE_CLAUDIO_R      0
#define PROFILE_CLAUDIO_G      2
#define PROFILE_CLAUDIO_B     10

#define PROFILE_HERMINIO_R    10
#define PROFILE_HERMINIO_G     8
#define PROFILE_HERMINIO_B     0

#define PROFILE_MARIANA_R     12
#define PROFILE_MARIANA_G     11
#define PROFILE_MARIANA_B      9

#define PROFILE_VISITOR_R      0
#define PROFILE_VISITOR_G     10
#define PROFILE_VISITOR_B     12
```

These values are specific to:

```text
Atom Matrix 5 × 5
```

They must not be copied automatically to the RGB Strip Node.

---

# Calibrated Profiles

## Claudio

```text
RGB = (0, 2, 10)
```

Purpose:

- increase blue visibility;
- preserve low intensity;
- maintain neutral-cool visual temperature.

## Hermínio

```text
RGB = (10, 8, 0)
```

Purpose:

- increase green contribution;
- reduce red dominance;
- move the visual result toward yellow / amber.

## Mariana

```text
RGB = (12, 11, 9)
```

Purpose:

- preserve warm white;
- reduce excessive orange appearance;
- keep medium-high perceived intensity.

## Visitor

```text
RGB = (0, 10, 12)
```

Purpose:

- preserve soft cyan;
- increase visibility on the matrix;
- maintain a neutral welcoming expression.

---

# Semantic State Values

The semantic state values remain conservative.

```c
#define RGB_LEVEL_VERY_LOW 2
#define RGB_LEVEL_LOW      4
#define RGB_LEVEL_MEDIUM   8
```

Current states:

## Idle

```text
RGB = (0, 0, 2)
```

Very soft blue.

## Presence detected

```text
RGB = (4, 4, 0)
```

Soft yellow.

## Processing

```text
RGB = (4, 4, 4)
```

Neutral low-intensity white.

## Error

```text
RGB = (4, 0, 0)
```

Persistent red with limited intensity.

---

# Calibration Design Principle

The project now distinguishes between:

```text
Semantic equivalence
```

and:

```text
Platform calibration
```

The semantic meaning is identical across all RGB nodes.

Example:

```text
Claudio
        ↓
Neutral-cool soft blue
```

The numerical RGB values may differ by hardware platform.

This is intentional.

Factors that can affect perceived color include:

- LED manufacturer and production bin;
- package characteristics;
- diffuser geometry;
- current regulation;
- LED density;
- viewing angle;
- surrounding material;
- distance from the observer;
- camera exposure;
- ambient lighting.

Therefore, the correct engineering objective is:

```text
same perceived ambient expression
```

rather than:

```text
same raw RGB numbers
```

---

# Future Calibration Procedure

Future adjustments should be made only in the calibration block at the beginning of:

```text
main/rgb_effects.c
```

Recommended process:

```text
1. Select one profile.
2. Authenticate the corresponding user.
3. Observe RGB Strip and Atom Matrix simultaneously.
4. Change only the Atom Matrix PROFILE_* values.
5. Rebuild and flash.
6. Compare again under the same ambient lighting.
7. Record the final accepted values.
```

The effect functions should not be rewritten during routine calibration.

Example:

```c
int rgb_effects_profile_claudio(void)
{
    return rgb_controller_set_all(
        PROFILE_CLAUDIO_R,
        PROFILE_CLAUDIO_G,
        PROFILE_CLAUDIO_B
    );
}
```

Only the `PROFILE_CLAUDIO_*` definitions should normally change.

---

# Future Rhythm and Animation Work

The current implementation is intentionally static.

Each semantic event produces a stable ambient color.

Future rhythm or animation work may introduce:

- fade-in;
- fade-out;
- breathing;
- low-frequency pulsing;
- transition interpolation;
- profile-specific temporal behavior.

Such work should preserve the existing separation:

```text
Semantic Consumer
        ↓
Expression Processor
        ↓
RGB Effects
        ↓
RGB Controller
```

Recommended future extension:

```text
rgb_effects
        ↓
animation or rhythm engine
        ↓
rgb_controller
```

The Cognitive Runtime should continue sending semantic meaning rather than low-level animation commands.

---

# Cognitive Runtime Dependency

No additional Cognitive Runtime modification was required specifically for the Atom Matrix replication.

The Atom Matrix consumes the same event already generated for the RGB Strip path:

```json
{
  "type": "semantic_event",
  "event": "identity_authenticated",
  "target": "rgb_strip",
  "payload": {
    "user_id": "claudio"
  }
}
```

The relevant Cognitive Runtime file previously modified for this integration is:

```text
runtime/cognitive/stackflow/rgb_strip_notifier.py
```

That file was updated earlier to propagate:

```text
payload.user_id
```

The Atom Matrix reuses that validated semantic path.

---

# Validation Result

The following points were validated:

```text
Firmware build
    PASS

ESP32 target
    PASS

Flash operation
    PASS after serial retry / lower-speed procedure

Wi-Fi startup
    PASS

Semantic receiver startup
    PASS

identity_authenticated reception
    PASS

payload.user_id extraction
    PASS

Profile selection
    PASS

Atom Matrix LED output
    PASS

Visual profile calibration
    IMPLEMENTED — subject to final human fine adjustment
```

---

# Final Architecture

```text
Identity Node
        ↓
Identity Package
        ↓
AX630C Cognitive Runtime
        ↓
rgb_strip_notifier.py
        ↓
UDP Semantic Event
        ↓
Atom Matrix Semantic Receiver
        ↓
Semantic Consumer
        ↓
payload.user_id
        ↓
Expression Processor
        ↓
Profile-Specific RGB Effect
        ↓
Atom Matrix RGB Controller
        ↓
5 × 5 Ambient Expression
```

---

# Final Status

The Atom Matrix RGB Node now:

- follows the same semantic architecture as the other RGB Expression Nodes;
- supports authenticated-user profile selection;
- reproduces the same intended profile colors;
- uses platform-specific calibration;
- preserves backward compatibility;
- keeps hardware control isolated;
- centralizes calibration values;
- is prepared for future rhythm and animation evolution.

Current status:

```text
ATOM MATRIX RGB NODE
PROFILE-AWARE SEMANTIC INTEGRATION: COMPLETE

PLATFORM CALIBRATION: IMPLEMENTED

FINAL VISUAL FINE-TUNING: OPTIONAL
```

---

# Summary of Modified Behavior

```text
Idle
→ very soft blue

Presence detected
→ soft yellow

Claudio
→ calibrated soft blue

Hermínio
→ calibrated yellow / amber

Mariana
→ calibrated warm white

Visitor
→ calibrated soft cyan

System error
→ limited persistent red
```

The node is functionally complete and ready for commit.
