# M003 — VL53L0X ESP-IDF Integration

**Date:** 31/05/2026

## Objective

Integrate the Mini ToF-90 (VL53L0X) sensor with the official ESP-IDF firmware of the Ambient Physical AI project.

---

## Hardware

* M5Dial
* Mini ToF-90
* VL53L0X

---

## Sprint 1 — Device Detection

### Goal

Validate communication between ESP-IDF and the VL53L0X sensor.

### Result

Successful device detection.

Observed output:

```text
VL53L0X detected
VL53L0X detected
VL53L0X detected
```

### Validated Components

* ESP-IDF 5.4.2
* I2C Bus
* GPIO 13 (SDA)
* GPIO 15 (SCL)
* VL53L0X Component
* Device Registration
* Register Access

### Conclusion

The Presence Node successfully communicates with the VL53L0X sensor.

This represents the first real sensor operating under the official Ambient Physical AI firmware architecture.

---

## Status

M003 Sprint 1

Completed

---

## Next Step

Sprint 2 — Distance Measurement
