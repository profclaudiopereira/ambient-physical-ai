# Presence Node V1

## Ambient Physical AI

### Presence Layer Runtime Node

The Presence Node V1 is responsible for measuring physical proximity and generating the first presence signals for the Ambient Physical AI ecosystem.

This node is intentionally simple and focused only on distance measurement at this stage.

---

# Baseline Hardware

```text
AtomS3 Lite
+
Unit Mini ToF-90 / VL53L0X
```

---

# Purpose

The Presence Node provides:

```text
Distance Measurement
↓
Presence Detection
↓
Presence Event
↓
Future Identity Node Trigger
```

---

# Project Status

```text
Presence Node V1
VALIDATED
```

Current milestone:

```text
PRESENCE_NODE_V1_MILESTONE_001
Serial distance measurement validated
```

---

# Hardware Configuration

```text
AtomS3 Lite
Unit Mini ToF-90
VL53L0X
Grove HY2.0-4P
```

I2C configuration:

```text
SDA = GPIO2
SCL = GPIO1
Address = 0x29
```

---

# Validated Results

VL53L0X identity registers:

```text
MODEL_ID    = 0xEE
MODULE_TYPE = 0xAA
REVISION_ID = 0x10
```

Distance measurement validated in serial monitor:

```text
Distance: 55 mm
Distance: 50 mm
Distance: 53 mm
Distance: 54 mm
Distance: 259 mm
Distance: 179 mm
```

---

# Validated Features

```text
I2C communication ....... PASS
VL53L0X detection ....... PASS
Sensor initialization ... PASS
Distance measurement .... PASS
Serial output ........... PASS
```

---

# Build

```bash
idf.py build
```

---

# Flash and Monitor

```bash
idf.py flash monitor
```

---

# Current Scope

This firmware currently provides only:

```text
Distance measurement in millimeters
Serial logging
Basic VL53L0X initialization
```

No UI, MQTT, StackFlow or Identity Node communication is implemented yet.

---

# Next Steps

Planned next milestone:

```text
Presence Node V1.1
```

Goals:

```text
Add threshold-based presence detection
Print PRESENT / NOT_PRESENT events
Debounce distance readings
Simulate Presence → Identity flow
Prepare future StackFlow/MQTT integration
```

---

# Architecture Decision

The M5Dial remains the Identity Gateway.

```text
Identity Node V1 = M5Dial + NFC
Presence Node V1 = AtomS3 Lite + VL53L0X
```

---

# Deferred

Not part of this milestone:

```text
MQTT
StackFlow
AX630C integration
Camera
AtomS3R Cam vision
M5Dial ToF integration
```

---

# Related Documentation

```text
docs/notes/PRESENCE_NODE_V1_BRINGUP_001.md
firmware/nodes/identity-node/README.md
```
