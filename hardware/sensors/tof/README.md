# ToF Sensor

## Presence Layer

---

# Hardware

M5Stack Unit Mini ToF-90°

Sensor: VL53L0X
Interface: I2C
Address: 0x29
Range: 3 cm ~ 200 cm
Field of View: 25°
Role: Presence Layer

---

# Architectural Role

This sensor belongs to the:

## Presence Layer

Conceptual flow:

```text
Presence
→ Identity
→ Cognition
→ Ambient Transformation
→ Experience
```

Its purpose is to detect the existence and proximity of people or objects within the environment.

---

# Characteristics

* Time-of-Flight (ToF)
* I2C communication
* Range: 3 cm ~ 200 cm
* Resolution: 1 mm
* Fast response
* 90° forward-facing orientation

---

# Usage in Ambient Physical AI

Initial responsibilities:

* presence detection;
* distance measurement;
* environmental awareness;
* contextual triggering.

Future responsibilities:

* occupancy estimation;
* approach detection;
* contextual activation;
* ambient interaction triggers.

---

# Integration Target

Initial node:

```text
presence-node
```

Platform:

```text
CoreS3 Lite
```

---

# Status

Hardware acquired.

Integration planned for:

Presence Node v0.2

## Validation

The Mini ToF-90 sensor was independently validated before integration into the Ambient Physical AI runtime.

### Validation Platform

* M5Dial
* ESP32-S3
* Arduino IDE

### Validation Sketch

```text
validation/arduino/vl53l0x_validation.ino
```

### Result

Successful distance measurements were obtained:

```text
Distance: 45 mm
Distance: 42 mm
Distance: 46 mm
Distance: 43 mm
Distance: 43 mm
Distance: 45 mm
Distance: 44 mm
```

### Validation Outcome

Confirmed operational:

* VL53L0X sensor
* Grove cable
* Grove interface
* power supply
* distance measurement

### Architectural Impact

This validation confirms that the hardware layer is operational.

Any remaining issues observed during ESP-IDF integration are therefore considered software integration challenges and not hardware failures.

### Status

Hardware Validation: Completed

Milestone 002: Completed

---

# Photos

## Hardware Reference

```text
photos/2026-05-31_unit-mini-tof-90_front.jpeg
```

Front view of the M5Stack Unit Mini ToF-90 sensor.

---

## Investigation Setup

```text
photos/2026-05-31_cores3-lite_tof-investigation.jpeg
```

Initial ESP-IDF integration and I2C investigation setup using CoreS3 Lite.

This setup was used during the early bring-up phase of the Presence Layer.

---

## Validation Setup

```text
photos/2026-05-31_m5dial_tof-validation.jpeg
```

Validation setup using M5Dial.

This configuration successfully validated:

* VL53L0X sensor
* Grove cable
* Grove interface
* power supply
* distance measurement

Observed measurements:

```text
Distance: 45 mm
Distance: 42 mm
Distance: 46 mm
Distance: 43 mm
```

---

# ESP-IDF Integration

Firmware:

```text
firmware/nodes/presence-node
```

Platform:

```text
ESP-IDF 5.4.2
```

Result:

```text
VL53L0X detected
```

The sensor is successfully detected by the official Presence Node firmware.

Current status:

```text
M002 Hardware Validation
COMPLETED

M003 Sprint 1
Device Detection
COMPLETED

M003 Sprint 2
Distance Measurement
NEXT
```



