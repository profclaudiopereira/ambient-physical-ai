# DLight Driver

## Ambient Physical AI

### Native ESP-IDF Driver for the M5Stack DLight Sensor

---

# Overview

The `dlight` component provides a lightweight ESP-IDF driver for the M5Stack DLight unit.

Its sole responsibility is to acquire ambient illumination measurements through the I²C bus and return the measured value in lux.

The component intentionally remains stateless and independent from the remainder of the Ambient Runtime.

---

# Purpose

The purpose of this component is to provide a simple and reusable interface for ambient light measurements.

Current responsibilities include:

- communicating with the DLight sensor;
- triggering a light measurement;
- reading the measured value;
- converting the raw sensor value into lux;
- returning the result to the caller.

The component performs no application logic and stores no runtime state.

---

# Position within the Ambient Runtime

The DLight driver belongs to the device layer of the Ambient Runtime.

```text
Ambient Runtime
        │
        ▼
Application
        │
        ▼
dlight
        │
        ▼
ESP-IDF I²C Driver
        │
        ▼
M5Stack DLight
```

Higher-level runtime components consume only the measured illumination value.

They remain independent from the sensor communication protocol.

---

# Responsibilities

The `dlight` component performs only ambient light acquisition.

Current responsibilities include:

## Device Communication

Communicate with the DLight sensor using the ESP-IDF I²C Master driver.

---

## Measurement Trigger

Configure the sensor to start a new illumination measurement.

The current implementation uses:

```text
Continuous High Resolution Mode
```

through the command:

```text
0x10
```

---

## Measurement Acquisition

Wait for the measurement to complete before reading the sensor registers.

The current implementation performs the timing required by the device internally.

Higher-level components do not manage measurement timing.

---

## Lux Conversion

Convert the raw sensor value into physical illumination.

The conversion follows the sensor specification:

```text
Lux = Raw Value / 1.2
```

The returned value is expressed in lux.

---

# Measurement Flow

Each measurement follows the same execution sequence.

```text
Application
        │
        ▼
Create I²C Device
        │
        ▼
Configure Sensor
        │
        ▼
Start Measurement
        │
        ▼
Wait for Conversion
        │
        ▼
Read Raw Value
        │
        ▼
Convert to Lux
        │
        ▼
Release I²C Device
        │
        ▼
Return Result
```

The driver does not maintain persistent device handles between calls.

Each measurement is performed independently.

---

# Public API

The component exposes a single public function.

```c
esp_err_t dlight_read_lux(
    i2c_master_bus_handle_t bus,
    float *lux
);
```

Parameters:

| Parameter | Description |
|-----------|-------------|
| `bus` | I²C bus used to communicate with the sensor. |
| `lux` | Output parameter receiving the measured illumination. |

Return value:

- `ESP_OK` on success.
- Standard ESP-IDF error codes on failure.

---

# Dependencies

The component depends on:

- ESP-IDF I²C Master Driver;
- FreeRTOS (measurement delay).

No dependencies on other Ambient Runtime components exist.

The I²C bus is supplied by the caller.

---

# Build

The component is built automatically as part of the Ambient Runtime.

Typical build sequence:

```bash
cd firmware/nodes/ambient-runtime-node

idf.py build

idf.py flash

idf.py monitor
```

No additional configuration is required.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| I²C communication | Validated |
| Sensor configuration | Validated |
| Measurement acquisition | Validated |
| Lux conversion | Validated |
| Runtime integration | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Engineering Notes

The driver intentionally avoids maintaining internal state.

Each measurement is performed independently by:

- creating a temporary I²C device handle;
- executing a complete measurement cycle;
- releasing the device handle before returning.

This approach keeps the implementation simple, deterministic and reusable.

The component also performs the measurement delay internally, ensuring that callers receive a completed measurement without managing sensor timing.

---

# Related Components

The DLight driver commonly operates together with:

| Component | Relationship |
|-----------|--------------|
| `tab5_platform` | Provides the I²C bus used by the sensor. |
| `pahub` | Selects the external I²C channel connected to the DLight unit. |
| `ambient_console` | Displays the measured illumination. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/components/pahub/README.md` | External I²C multiplexer. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md` | Runtime Console. |

---

# Conclusion

The `dlight` component provides a compact and reusable interface for acquiring ambient illumination measurements within the Ambient Runtime.

By encapsulating the complete measurement cycle and exposing only a simple API that returns illumination in lux, the component remains independent from application logic, communication infrastructure and runtime services, making it easy to integrate and maintain throughout the Ambient Physical AI ecosystem.