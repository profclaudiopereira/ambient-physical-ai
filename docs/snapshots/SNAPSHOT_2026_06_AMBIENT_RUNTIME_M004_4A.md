# SNAPSHOT_2026_06_AMBIENT_RUNTIME_M004_4A.md

## Ambient Physical AI

### Milestone

M004 — Ambient Runtime Node Bringup

### Status

IN PROGRESS

---

## Objective

Validate the PoE-P4 as the first operational Ambient Runtime Node of the Ambient Physical AI architecture.

---

## Hardware Configuration

### Core Components

```text
PoE-P4 (ESP32-P4)
    │
    └── PaHub v2.1 (PCA9548A)
            │
            └── Channel 0
                    │
                    └── ENV-IV
```

---

## Display Investigation Status

Display validation was temporarily suspended after instability was observed.

Symptoms:

* USB connect/disconnect loop
* COM port instability
* FPC cable heating
* PoE-P4 heating

After removing the FPC cable:

* USB stabilized
* COM port stabilized
* ESP-ROM boot messages returned
* Heating disappeared

Current classification:

```text
Display Investigation
=
Parallel Activity
```

```text
Ambient Runtime Bringup
=
Primary Activity
```

---

## M004.1 — ESP32-P4 Bringup

Status:

```text
COMPLETED
```

Validation:

* ESP32-P4 boot successful
* Serial communication stable
* Flash successful
* Runtime execution successful

Observed output:

```text
Ambient Runtime Node
Ambient Physical AI
ESP32-P4 Bringup

Ambient Runtime Alive
```

Git Commit:

```text
ae5fc28
Validate PoE-P4 ESP32-P4 bringup
```

---

## M004.2 — PaHub Detection

Status:

```text
COMPLETED
```

Detected:

```text
0x70
```

Validation:

```text
PoE-P4
    ↓
I2C
    ↓
PCA9548A
```

Git Commit:

```text
Detect PaHub PCA9548A on PoE-P4 I2C bus
```

---

## M004.3 — Channel Selection

Status:

```text
COMPLETED
```

Selected:

```text
PaHub Channel 0
```

Detected:

```text
0x44
0x70
0x76
```

Interpretation:

```text
0x44 → SHT40
0x70 → PCA9548A
0x76 → BMP280
```

Validation:

```text
PoE-P4
    ↓
PaHub
    ↓
ENV-IV
```

---

## M004.4A — Environmental Readings

Status:

```text
COMPLETED
```

Component Created:

```text
components/env_iv
```

Files:

```text
env_iv.c
env_iv.h
CMakeLists.txt
```

Implemented:

```text
SHT40 Temperature
SHT40 Humidity
```

Observed readings:

```text
Temp: 30.29 C
Hum : 72.20 %
```

Additional readings:

```text
Temp: 30.30 C
Hum : 72.24 %
```

```text
Temp: 30.32 C
Hum : 72.23 %
```

Validation achieved:

```text
Physical Environment
    ↓
ENV-IV
    ↓
PaHub
    ↓
PoE-P4
    ↓
Ambient Runtime Node
```

Git Commit:

```text
9cdb029
Read SHT40 temperature and humidity from ENV-IV
```

---

## Current Architectural State

```text
Presence Layer
    ↓
Presence Node
    ✓ Operational

Ambient Layer
    ↓
Ambient Runtime Node
    ✓ Operational
```

Current Ambient Runtime capabilities:

```text
✓ ESP32-P4 Runtime
✓ I2C Communication
✓ PCA9548A Multiplexing
✓ Environmental Temperature
✓ Environmental Humidity
```

---

## Next Milestones

### M004.4B

Implement BMP280 driver.

Expected outputs:

```text
Temperature
Humidity
Pressure
```

---

### M004.5

Integrate Unit Mini OLED.

Expected display:

```text
Ambient Runtime OK

Temp : XX.X C
Hum  : XX.X %
Press: XXXX.X hPa
```

---

## Conclusion

The Ambient Runtime Layer has transitioned from architectural concept to operational firmware.

For the first time, the Ambient Runtime Node is collecting real environmental data through physical sensors and processing them on the PoE-P4 platform.

This represents the first concrete implementation of the Ambient Runtime Layer within the Ambient Physical AI architecture.
