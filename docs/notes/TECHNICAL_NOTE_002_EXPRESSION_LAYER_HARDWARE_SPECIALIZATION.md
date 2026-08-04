
# TECHNICAL_NOTE_002_EXPRESSION_LAYER_HARDWARE_SPECIALIZATION.md

# Expression Layer Hardware Specialization

## Ambient Physical AI

### Runtime State RGB Node and Wearable Haptic Node

---

# Objective

This technical note documents the engineering consolidation of two complementary Runtime State presentation nodes in the Ambient Physical AI Expression Layer:

- the **Runtime State RGB Node**;
- the **Wearable Haptic Node**.

The note records the complete hardware reassignment performed between the two nodes, the technical motivation behind the change, the implementation consequences, the communication model adopted by the Cognitive Runtime, the validation performed on real hardware and the final repository organization.

This document is intended to be the authoritative technical reference for the hardware specialization decision shared by both firmware projects.

---

# Scope

This note covers:

- the original hardware allocation;
- the evaluation of the M5StickS3 with the Vibrator HAT;
- the GPIO and hardware compatibility investigation;
- the reassignment of devices between the two Runtime State nodes;
- the final hardware configuration;
- the reuse of the common Runtime State firmware pipeline;
- the multi-destination Runtime State distribution implemented in the Cognitive Runtime;
- the validation of synchronized visual and haptic presentation;
- the impact on firmware directories, READMEs and future repository cleanup.

This note does not redesign the Runtime State protocol, the Semantic Event pipeline or the broader Ambient Physical AI architecture.

---

# Background

The Runtime State Indicator was originally implemented as a visual Expression Layer node using:

- M5StickC Plus2;
- integrated display;
- external WS2812 RGB bar;
- UDP Runtime State reception;
- asynchronous local presentation scheduling.

Separately, an experimental wearable firmware already existed outside the main Ambient Physical AI repository. That firmware used:

- M5StickC Plus2;
- official M5Stack Vibrator HAT;
- integrated display;
- GPIO-based vibration control;
- a local movement-detection use case.

The wearable firmware had already validated the physical operation of the Vibrator HAT, including:

- GPIO initialization;
- motor enable and disable behavior;
- short vibration pulses;
- integrated display operation;
- M5Unified initialization.

The engineering objective was to introduce a new official **Wearable Haptic Node** into the Ambient Physical AI Expression Layer while preserving the validated Runtime State contract already used by the visual indicator.

---

# Initial Integration Strategy

The first integration strategy attempted to preserve the original Runtime State RGB Node hardware and use the M5StickS3 as the wearable platform.

The initial allocation was therefore:

| Node | Initial Hardware |
|---|---|
| Runtime State RGB Node | M5StickC Plus2 + WS2812 RGB bar |
| Wearable Haptic Node | M5StickS3 + Vibrator HAT |

The wearable firmware was created by reusing the validated Runtime State pipeline from the existing RGB node:

- `wifi_station`;
- `semantic_receiver`;
- `semantic_consumer`;
- `expression_processor`;
- Runtime State vocabulary;
- Runtime State Scheduler;
- display presentation logic;
- UDP port `5555`;
- target `runtime_state_indicator`.

The visual WS2812 output was removed from the wearable firmware and the previously validated vibrator driver was introduced.

---

# Runtime State Contract Preservation

The same normalized Runtime State contract was preserved for both devices:

```json
{
  "type": "runtime_state",
  "target": "runtime_state_indicator",
  "state": "responding"
}
```

The following architectural properties remained unchanged:

- transport: UDP;
- destination port: `5555`;
- Runtime State vocabulary;
- message type validation;
- target validation;
- asynchronous queue;
- minimum local presentation times;
- expression processing;
- visual color mapping.

The hardware specialization therefore did not create a new protocol or a separate semantic vocabulary.

The distinction between nodes exists only at the physical presentation layer.

---

# Wearable Haptic Integration Investigation

## Functional Software Validation

During the first M5StickS3 wearable integration, the following subsystems operated correctly:

- ESP-IDF build for `esp32s3`;
- M5Unified board detection;
- integrated display initialization;
- Wi-Fi Station connection;
- static IPv4 configuration;
- UDP Runtime State reception;
- JSON validation;
- Runtime State queuing;
- Runtime State scheduling;
- state-to-color mapping;
- state label rendering.

The wearable successfully received Runtime State messages and displayed states such as:

- `idle`;
- `responding`.

This confirmed that the common Runtime State pipeline was portable and hardware-independent above the presentation boundary.

---

## Vibrator HAT Behavior

When the Vibrator HAT was used with the M5StickS3 during the first integration attempt, the motor remained continuously active in the normal idle condition.

Several software-level possibilities were investigated:

- active-high versus active-low motor control;
- repeated `responding` presentation;
- repeated `vibrator_alert()` calls;
- state queue behavior;
- Expression Processor behavior;
- GPIO reconfiguration after M5Unified initialization;
- initialization order;
- forced motor-off operations;
- clean rebuilds;
- target changes;
- GPIO reset and output reconfiguration.

The Runtime State queue and Expression Processor were reviewed and confirmed not to be reapplying the `responding` state continuously.

The vibrator driver was also tested independently using explicit timing transitions.

The test sequence:

```text
LOW
  ↓
delay
  ↓
HIGH
  ↓
delay
  ↓
LOW
```

produced a single vibration interval followed by motor shutdown on the validated original hardware configuration.

This confirmed that:

- the driver logic was valid;
- GPIO-based control was functional;
- the motor could be pulsed and stopped correctly;
- the continuous vibration was not caused by the Runtime State Scheduler.

---

# Hardware Compatibility Finding

The investigation showed that the official Vibrator HAT and the original wearable firmware were already validated as a physical pair with the M5StickC Plus2.

The M5StickS3 introduced a different external connector arrangement and different board-level pin usage.

Although the Runtime State firmware itself ran correctly on the M5StickS3, the direct use of the existing Vibrator HAT configuration introduced unnecessary hardware compatibility risk.

At the same time, the M5StickS3 proved suitable for the RGB presentation role after adapting the external WS2812 data GPIO to the Grove connector used in the final hardware assembly.

This led to a more robust and maintainable engineering decision: specialize each hardware platform according to the peripheral configuration already validated or naturally compatible with it.

---

# Device Reassignment

A deliberate device exchange was performed between the two nodes.

## Before the Reassignment

| Node | Hardware |
|---|---|
| Runtime State RGB Node | M5StickC Plus2 + WS2812 RGB bar |
| Wearable Haptic Node | M5StickS3 + Vibrator HAT |

## After the Reassignment

| Node | Final Hardware |
|---|---|
| Runtime State RGB Node | M5StickS3 + WS2812 RGB bar |
| Wearable Haptic Node | M5StickC Plus2 + official Vibrator HAT |

This device exchange is the central engineering decision documented by this note.

The reassignment was not a protocol redesign and did not change the role of the Cognitive Runtime.

It was a hardware specialization decision that aligned each Expression Layer node with the peripheral configuration that provided the highest practical reliability.

---

# Final Architecture

```text
                     Cognitive Runtime (AX630C)
                                │
                    Normalized Runtime State
                                │
                 Multi-destination UDP delivery
                                │
             ┌──────────────────┴──────────────────┐
             │                                     │
             ▼                                     ▼
 Runtime State RGB Node                  Wearable Haptic Node
      M5StickS3                            M5StickC Plus2
           +                                      +
    WS2812 RGB bar                         Vibrator HAT
             │                                     │
             ▼                                     ▼
 Display + synchronized RGB             Display + haptic pulse
```

Both nodes consume the same Runtime State message.

Neither node performs:

- AI inference;
- semantic reasoning;
- authorization;
- context generation;
- Cognitive Runtime orchestration.

Each node is a specialized physical presentation endpoint.

---

# Runtime State RGB Node

## Final Hardware

- M5StickS3;
- ESP32-S3;
- integrated display;
- external WS2812 RGB bar;
- Grove connection;
- static IPv4 address `192.168.77.207`;
- UDP port `5555`.

## Hardware Adaptation

The original RGB firmware used GPIO `32` for the external WS2812 bar.

On the M5StickS3, this GPIO could not be used for the RMT output in the final configuration and caused initialization failure followed by watchdog resets.

The log showed:

```text
rmt: GPIO 32 is not usable, maybe conflict with others
```

The WS2812 data output was reassigned to GPIO `9`, matching the Grove-connected hardware arrangement used by the M5StickS3 assembly.

After the GPIO correction:

- the display initialized;
- the WS2812 controller initialized;
- Wi-Fi connected;
- the node acquired `192.168.77.207`;
- the Runtime State consumer started;
- the UDP receiver listened on port `5555`;
- `idle` and `responding` were presented correctly.

## Final Responsibility

The Runtime State RGB Node provides:

- Runtime State label;
- display background color;
- synchronized WS2812 color;
- visual Cognitive Runtime status.

It does not provide haptic feedback.

---

# Wearable Haptic Node

## Final Hardware

- M5StickC Plus2;
- ESP32 target;
- integrated display;
- official M5Stack Vibrator HAT;
- GPIO `26`;
- static IPv4 address `192.168.77.208`;
- UDP port `5555`.

## Firmware Origin

The Wearable Haptic Node is a new official Ambient Physical AI firmware node.

Its implementation reuses the validated Runtime State architecture from the visual Runtime State indicator while incorporating the already validated Vibrator HAT driver from the original wearable experiment.

The final firmware preserves:

- Wi-Fi Station mode;
- static IP configuration;
- Runtime State UDP receiver;
- Runtime State message validation;
- Runtime State queue;
- local presentation scheduler;
- normalized Runtime State vocabulary;
- display color and label mapping.

The WS2812 hardware boundary was removed and replaced with the haptic output driver.

## Haptic Policy

The wearable intentionally does not vibrate for every Runtime State.

The final policy is:

| Runtime State | Display | Vibration |
|---|---:|---:|
| idle | yes | no |
| presence | yes | no |
| listening | yes | no |
| thinking | yes | no |
| responding | yes | one short pulse |
| alert | yes | no |
| error | yes | no |
| offline | yes | no |
| learning | yes | no |

The `responding` state produces:

- blue display background;
- `RESPONDING` label;
- one short vibration pulse;
- explicit motor shutdown after the pulse.

This behavior was validated on real hardware.

---

# Cognitive Runtime Multi-Destination Distribution

The original `runtime_state_notifier.py` sent the Runtime State message to one destination:

```text
192.168.77.207:5555
```

The notifier was extended to support multiple Runtime State destinations.

The final reference configuration sends the same payload to:

```text
192.168.77.207:5555
192.168.77.208:5555
```

The notifier now supports:

```text
RUNTIME_STATE_HOSTS
```

for comma-separated multi-destination configuration while preserving compatibility with the original:

```text
RUNTIME_STATE_HOST
```

The precedence is:

```text
RUNTIME_STATE_HOSTS
        ↓ when absent
RUNTIME_STATE_HOST
        ↓ when absent
default Runtime State destination list
```

The same serialized JSON payload is transmitted sequentially through the same UDP socket to both nodes.

No transformation is performed per destination.

---

# Synchronization Model

The two nodes are synchronized at the event-distribution level.

The Cognitive Runtime publishes the same Runtime State to both devices in the same notifier operation.

The devices then apply their local presentation policies independently.

Example:

```text
Runtime State: responding
        │
        ├── Runtime State RGB Node
        │      display: RESPONDING
        │      color: blue
        │      WS2812: blue
        │
        └── Wearable Haptic Node
               display: RESPONDING
               color: blue
               vibration: one short pulse
```

The system does not require clock synchronization between the devices.

The practical synchronization derives from:

- a common Runtime State source;
- identical payloads;
- the same UDP publication cycle;
- matching local presentation mappings.

---

# Validation Performed

## Runtime State RGB Node

Validated:

- ESP32-S3 target;
- M5StickS3 board detection;
- display initialization;
- WS2812 operation on GPIO `9`;
- static IP `192.168.77.207`;
- UDP port `5555`;
- Runtime State queue;
- `idle` presentation;
- `responding` presentation;
- synchronized display and RGB bar.

## Wearable Haptic Node

Validated:

- ESP32 target;
- M5StickC Plus2 display;
- Vibrator HAT on GPIO `26`;
- static IP `192.168.77.208`;
- UDP port `5555`;
- Runtime State queue;
- `idle` without vibration;
- `responding` with one short pulse;
- motor shutdown after the pulse.

## Cognitive Runtime

Validated:

- Python syntax;
- multi-destination Runtime State notifier;
- delivery to both nodes;
- compatibility with `RUNTIME_STATE_HOST`;
- support for `RUNTIME_STATE_HOSTS`;
- `responding` distribution;
- `idle` distribution.

## End-to-End Validation

The following end-to-end tests passed:

### Responding

```text
Runtime State RGB Node:
- RESPONDING displayed
- blue visual state
- WS2812 synchronized

Wearable Haptic Node:
- RESPONDING displayed
- blue visual state
- single vibration pulse
- motor stopped after pulse
```

### Idle

```text
Runtime State RGB Node:
- IDLE displayed
- green visual state

Wearable Haptic Node:
- IDLE displayed
- green visual state
- no vibration
```

---

# Engineering Benefits

The final specialization provides the following benefits:

- uses each hardware platform with the peripheral arrangement best suited to it;
- preserves the validated Runtime State protocol;
- avoids introducing device-specific message contracts;
- eliminates unnecessary coupling between visual and haptic presentation;
- allows both nodes to evolve independently;
- maintains a common Cognitive Runtime integration path;
- reduces competition-time hardware risk;
- improves demonstration clarity;
- reinforces the distributed Physical AI architecture.

---

# Repository Impact

## Runtime State RGB Node Directory

Current directory:

```text
firmware/nodes/expression-node/stickc-plus2-rgb-node/
```

The directory retains its historical name during the competition preparation phase.

Its firmware and README now describe the **Runtime State RGB Node implemented on M5StickS3**.

The preserved directory name avoids unnecessary repository-history disruption and minimizes the risk of broken references before the competition.

A post-competition cleanup may rename the directory to:

```text
runtime-state-rgb-node/
```

## Wearable Haptic Node Directory

New directory:

```text
firmware/nodes/expression-node/wearable-haptic-node/
```

This directory contains the new official wearable firmware based on:

- M5StickC Plus2;
- Vibrator HAT;
- shared Runtime State protocol;
- haptic feedback during `responding`.

## Shared Documentation

Both firmware READMEs shall reference this note as the authoritative explanation of:

- the hardware exchange;
- the final node specialization;
- the shared Runtime State contract;
- the dual-destination Cognitive Runtime distribution.

---

# Maintenance Guidance

Future changes should preserve the following boundaries:

## Shared Behavior

Keep synchronized across both nodes:

- Runtime State vocabulary;
- JSON contract;
- target name;
- UDP port;
- Runtime State Scheduler policies;
- display labels;
- color convention.

## Node-Specific Behavior

Keep isolated:

### Runtime State RGB Node

- WS2812 driver;
- M5StickS3 hardware initialization;
- RGB output GPIO;
- visual effects.

### Wearable Haptic Node

- vibrator driver;
- M5StickC Plus2 hardware initialization;
- GPIO `26`;
- haptic policy.

## Cognitive Runtime

The Cognitive Runtime should continue to publish one normalized Runtime State payload to a configurable destination list.

It should not contain device-specific presentation logic.

---

# Risks and Trade-offs

## Preserved Historical Directory Name

Trade-off:

- the RGB directory name still references M5StickC Plus2;
- the current firmware uses M5StickS3.

Mitigation:

- explicit Repository Note in the README;
- reference to this technical note;
- planned post-competition rename.

## UDP Delivery

UDP does not guarantee delivery or acknowledgment.

For the current local Expression Layer demonstration, this remains acceptable because:

- messages are short;
- the network is controlled;
- presentation is transient;
- nodes are local;
- the architecture already uses UDP for Runtime State distribution.

Future production evolution may add:

- delivery monitoring;
- sequence numbers;
- health reporting;
- optional acknowledgment.

## Sequential Multi-Destination Send

The notifier sends to the configured hosts sequentially.

This introduces a negligible local timing difference and avoids unnecessary concurrency complexity.

For the current demonstration and local network, this trade-off is appropriate.

---

# Final Decision

The Ambient Physical AI Expression Layer now contains two complementary Runtime State presentation nodes:

```text
Runtime State RGB Node
Hardware: M5StickS3 + WS2812 RGB bar
Role: visual Runtime State presentation
IP: 192.168.77.207

Wearable Haptic Node
Hardware: M5StickC Plus2 + Vibrator HAT
Role: visual and tactile Runtime State presentation
IP: 192.168.77.208
```

The two devices were intentionally exchanged between the original node assignments to achieve the most reliable hardware specialization.

Both nodes consume the same Runtime State protocol and are updated by the same Cognitive Runtime notifier.

The final architecture is validated.

---

# Status

```text
Engineering Decision
────────────────────
Expression Layer Hardware Specialization

Runtime State RGB Node
──────────────────────
M5StickS3 + WS2812

Wearable Haptic Node
────────────────────
M5StickC Plus2 + Vibrator HAT

Runtime State Distribution
──────────────────────────
Multi-destination UDP

Validation
──────────
End-to-end validated on real hardware

Status
──────
VALIDATED ENGINEERING BASELINE
```
