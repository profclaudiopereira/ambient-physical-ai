# ToF Sensor

## Presence Layer

### Ambient Physical AI

---

# Overview

Este documento descreve o sensor oficial da Presence Layer do projeto Ambient Physical AI.

Hardware:

```text
M5Stack Unit Mini ToF-90°
```

Sensor:

```text
VL53L0X
```

Papel arquitetural:

```text
Presence Layer
```

---

# Hardware Specifications

| Item          | Value          |
| ------------- | -------------- |
| Sensor        | VL53L0X        |
| Interface     | I2C            |
| Address       | 0x29           |
| Range         | 3 cm ~ 200 cm  |
| Resolution    | 1 mm           |
| Field of View | 25°            |
| Orientation   | Forward Facing |
| Technology    | Time-of-Flight |

---

# Architectural Role

O sensor pertence à camada:

## Presence Layer

Fluxo arquitetural:

```text
Presence
↓
Identity
↓
Cognition
↓
Ambient Transformation
↓
Expression
```

Pergunta respondida:

> Existe alguém aqui?

---

# Responsibilities

## Current Responsibilities

```text
Presence detection
Distance measurement
Approach detection
Presence events
```

---

## Future Responsibilities

```text
Occupancy estimation
Presence confidence
Multi-sensor fusion
Context activation
Ambient triggers
```

---

# Approved Presence Node V1

A arquitetura oficial aprovada para a Presence Layer é:

```text
AtomS3 Lite
+
Unit Mini ToF-90
```

Status:

```text
APPROVED BASELINE
```

---

# Why AtomS3 Lite

Após investigações envolvendo:

```text
CoreS3 Lite
M5Dial
AtomS3R Cam
AtomS3 Lite
```

a plataforma escolhida foi:

```text
AtomS3 Lite
```

Motivos:

```text
VL53L0X validated
I2C stable
Simple hardware
Low cost
Clear architectural separation
Dedicated Presence Node
```

---

# Validation History

## Validation 01

Platform:

```text
M5Dial
Arduino
```

Result:

```text
Distance: 45 mm
Distance: 42 mm
Distance: 46 mm
Distance: 43 mm
```

Validation:

```text
PASS
```

Confirmed:

```text
Sensor
Cable
Power
Distance measurement
```

---

## Validation 02

Platform:

```text
AtomS3 Lite
ESP-IDF
```

I2C Detection:

```text
Address: 0x29
```

Identity Registers:

```text
MODEL_ID    = 0xEE
MODULE_TYPE = 0xAA
REVISION_ID = 0x10
```

Validation:

```text
PASS
```

---

## Validation 03

Distance Measurement

Observed values:

```text
Distance: 55 mm
Distance: 50 mm
Distance: 53 mm
Distance: 54 mm
Distance: 259 mm
Distance: 179 mm
Distance: 39 mm
```

Validation:

```text
PASS
```

---

# Presence Layer Status

Current status:

```text
Hardware Validation ........ COMPLETED
Device Detection ........... COMPLETED
Distance Measurement ....... COMPLETED
Presence Layer Baseline .... COMPLETED
```

Classification:

```text
Operational Baseline
```

---

# Firmware

Official firmware:

```text
firmware/nodes/presence-node-v1
```

Legacy investigations:

```text
firmware/nodes/presence-node-legacy
```

---

# Current Roadmap

## Presence Node V1.1

Next planned features:

```text
Presence Threshold
Presence Debounce
PRESENT event
NOT_PRESENT event
Presence State Machine
```

---

## Presence → Identity Integration

Target flow:

```text
Distance detected
↓
Presence Event
↓
Identity Node
↓
NFC Identification
```

---

# Future Evolution

Future platform under evaluation:

```text
AtomS3R Cam
```

Potential role:

```text
Presence + Vision Node
```

This is not part of Presence Node V1.

---

# Photos

## Hardware Reference

```text
assets/photos/unit_mini_tof90_front.jpeg
```

---

## Presence Node V1

```text
assets/photos/atoms3_lite_tof_validation.jpeg
```

(when available)

---

# Guiding Principle

The ToF sensor is not responsible for identity.

The ToF sensor is responsible only for:

```text
Presence
```

Identity remains the responsibility of:

```text
M5Dial
+
WS1850S NFC
```

This separation is a fundamental architectural decision of Ambient Physical AI.
