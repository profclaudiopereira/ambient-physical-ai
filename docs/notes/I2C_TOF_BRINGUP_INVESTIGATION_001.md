# I2C ToF Bringup Investigation 001

Date: 31/05/2026

## Objective

Validate communication between the Presence Node and the M5Stack Unit Mini ToF-90° sensor (VL53L0X).

---

## Hardware

### Sensor

* M5Stack Unit Mini ToF-90°
* VL53L0X

### Test Platforms

* CoreS3 Lite
* M5Dial

---

## Firmware

Project:

```text
firmware/nodes/presence-node
```

ESP-IDF:

```text
v5.4.2
```

---

## Test 01 — CoreS3 Lite

Configuration:

```text
SDA = GPIO2
SCL = GPIO1
```

Result:

```text
ESP_ERR_TIMEOUT
```

Observation:

The I2C bus initialized correctly but the sensor was not detected.

---

## Test 02 — M5Dial

Configuration:

```text
SDA = GPIO13
SCL = GPIO15
```

Result:

```text
No I2C devices found
```

Observation:

The sensor did not respond at address 0x29.

---

## Current Assessment

The issue is not currently considered to be:

* ESP-IDF installation
* build system
* flashing process
* runtime startup

The issue is likely related to one of:

* Grove port configuration
* sensor initialization requirements
* M5-specific hardware enable sequence
* cabling
* sensor hardware condition

---

## Decision

Do not block project progress on this issue.

Proceed with independent validation using official examples before continuing VL53L0X integration in ESP-IDF.

---

## Status

Investigation Open

Milestone:

Presence Node v0.2

## Cross Validation

### Platform

M5Dial

### Environment

Arduino IDE

### Firmware

VL53L0X validation sketch

### Result

```text
Distance: 45 mm
Distance: 42 mm
Distance: 46 mm
Distance: 43 mm
Distance: 43 mm
Distance: 45 mm
Distance: 44 mm
```

### Conclusion

The Mini ToF-90 sensor was successfully validated.

Confirmed operational:

* VL53L0X sensor
* Grove cable
* M5Dial Grove interface
* sensor power supply
* distance measurement

The issue observed during the ESP-IDF tests is therefore not related to hardware failure.

Current assessment:

The remaining investigation scope is limited to:

* ESP-IDF I2C configuration
* M5-specific initialization sequence
* Grove port enable/configuration
* platform-specific integration details

### Status

Hardware Validation: Completed

Milestone 002: Completed
