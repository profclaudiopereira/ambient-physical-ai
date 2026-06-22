# PRESENCE_NODE_V1_BRINGUP_001

## Ambient Physical AI

## Presence Node V1 Bring-up

**Date:** 2026-06-22
**Status:** VALIDATED

---

# Decision

Presence Node V1 baseline:

```text
AtomS3 Lite
+
Unit Mini ToF-90 / VL53L0X
```

Identity Node V1 remains:

```text
M5Dial
+
NFC
```

The ToF sensor is no longer a mandatory responsibility of the M5Dial.

---

# Hardware Validated

```text
AtomS3 Lite
Unit Mini ToF-90
VL53L0X
Grove HY2.0-4P
I2C SDA = GPIO2
I2C SCL = GPIO1
```

---

# Validation Results

VL53L0X detected at:

```text
0x29
```

Registers validated:

```text
MODEL_ID    = 0xEE
MODULE_TYPE = 0xAA
REVISION_ID = 0x10
```

Distance measurement validated:

```text
Distance: 55 mm
Distance: 50 mm
Distance: 53 mm
Distance: 54 mm
Distance: 259 mm
Distance: 179 mm
```

---

# Status

```text
VL53L0X detected ........ PASS
I2C communication ....... PASS
Sensor initialization ... PASS
Distance measured ....... PASS
Serial output ........... PASS
```

---

# Conclusion

Presence Node V1 hardware and serial distance measurement are validated.

This confirms the approved architecture:

```text
Presence Node V1 = AtomS3 Lite + ToF
Identity Node V1 = M5Dial + NFC
```

---

# Next Steps

Tomorrow:

1. Convert the experiment into a clean Presence Node V1 firmware baseline.
2. Add simple presence threshold logic.
3. Print presence events:

```text
PRESENT
NOT_PRESENT
```

4. Simulate Presence → Identity flow.
5. Prepare future integration path with StackFlow/MQTT.

---

# Deferred

Not part of this validation:

```text
MQTT
StackFlow
AX630C
Camera
AtomS3R Cam vision
M5Dial ToF integration
```
