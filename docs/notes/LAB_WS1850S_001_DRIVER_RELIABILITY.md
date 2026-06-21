# LAB_WS1850S_001 — Driver Reliability Investigation

## Status

Open Investigation

Non-Blocking

---

# Purpose

Document residual WS1850S NFC driver behavior observed during Identity Console V1 FreeRTOS stabilization.

This investigation is not currently blocking project progress.

Identity Console V1 is operational and approved as the current baseline.

---

# Context

During the Identity Console V1 FreeRTOS refactor, occasional NFC communication errors were observed.

Examples:

```text
ESP_ERR_INVALID_STATE

I2C transaction failed

NFC polling failed: ESP_ERR_INVALID_STATE
```

These errors appeared intermittently during runtime.

---

# Important Observation

The errors do not currently prevent system operation.

Validated functionality remains operational:

```text
WS1850S Detection ............ VALIDATED
UID Reading .................. VALIDATED
Profile Mapping .............. VALIDATED
Identity Package ............. VALIDATED
Card Removal Detection ....... VALIDATED
FreeRTOS Runtime ............. VALIDATED
```

Typical recovery sequence:

```text
Error detected
↓
Backoff
↓
NFC reinitialization
↓
Polling resumes
↓
UID reading resumes
```

The system continues operating normally after recovery.

---

# Investigations Performed

The following hypotheses were investigated:

```text
FreeRTOS task separation
CPU core assignment
Polling frequency
Recovery strategy
I2C mutex usage
WS1850S driver behavior
```

Results:

```text
FreeRTOS was not identified as the root cause.

Global mutex around M5.update() worsened behavior.

Aggressive bus reconstruction was not beneficial.

Driver-level improvements reduced error frequency.
```

---

# Driver Improvements Implemented

Implemented during stabilization:

```text
Explicit timeout handling

Validation of register access return values

Removal of aggressive reinitialization inside UID reads

Controlled retry acquisition

Fast UID acquisition strategy
```

These changes improved overall runtime behavior.

---

# Remaining Hypotheses

## Hypothesis A

Internal M5Dial I2C interactions involving:

```text
Touch
RTC
Power Management
WS1850S
```

---

## Hypothesis B

Residual limitations of the current WS1850S driver implementation.

---

## Hypothesis C

Interaction between:

```text
M5Unified
and
low-level WS1850S access
```

---

# Current Position

The issue is classified as:

```text
Residual Reliability Investigation
```

and not as:

```text
Project Blocker
```

---

# Future Investigation

Future work may include:

```text
WS1850S protocol analysis

Logic analyzer captures

Driver hardening

M5Unified interaction review

Long-duration stress testing
```

---

# Project Decision

Identity Console V1 remains approved as the current operational baseline.

No additional architectural changes are planned at this time.

The FreeRTOS runtime architecture remains the official architecture for Identity Console V1.

---

# Related Components

```text
identity-node

WS1850S

M5Dial V1.1

ESP-IDF

M5Unified
```
