# LAB_WS1850S_001_DRIVER_RELIABILITY

## Ambient Physical AI

### Identity Node V1

### Investigation Report

**Date:** 2026-06-22

**Status:** OPEN

**Priority:** Medium

**Classification:** Non-Blocking Reliability Investigation

---

# Objective

Investigate intermittent reliability issues observed during operation of the WS1850S NFC controller on the M5Dial Identity Node V1.

The goal of this investigation is to understand the root cause of residual NFC communication failures while preserving the current FreeRTOS architecture.

This investigation is independent from the FreeRTOS runtime stabilization effort.

---

# Background

During the Identity Console V1 FreeRTOS refactoring, occasional NFC communication failures were observed.

Typical errors:

```text
I2C transaction failed
ESP_ERR_INVALID_STATE
```

The failures occurred during NFC polling operations.

Despite the errors:

* UI remained operational
* Touch remained operational
* Encoder remained operational
* Display remained operational
* Recovery succeeded
* NFC functionality eventually recovered

Therefore the issue was classified as:

```text
NON-BLOCKING
```

---

# Hardware Configuration

## Device

```text
M5Dial V1.1
```

## NFC Controller

```text
WS1850S
```

## Firmware

```text
Identity Console V1
FreeRTOS Runtime
```

---

# Initial Symptoms

Observed during:

```text
NFC polling
UID acquisition
Card presence detection
```

Example logs:

```text
E (...) i2c.master: I2C transaction failed

W (...) NFC polling failed: ESP_ERR_INVALID_STATE
```

---

# Investigation Timeline

## Phase 1 — FreeRTOS Architecture

### Hypothesis

FreeRTOS tasks were causing state corruption.

### Actions

Implemented:

```text
UI Task
NFC Task
Identity Event Queue
```

### Result

No evidence of runtime corruption.

Architecture remained stable.

### Conclusion

```text
Hypothesis rejected
```

---

## Phase 2 — Core Affinity

### Hypothesis

Cross-core execution caused instability.

### Actions

Tested:

```text
UI Core 1
NFC Core 0
```

and later:

```text
UI Core 1
NFC Core 1
```

### Result

Errors persisted.

### Conclusion

```text
Hypothesis rejected
```

---

## Phase 3 — NFC Polling Frequency

### Hypothesis

Aggressive polling caused I2C instability.

### Actions

Multiple polling intervals tested:

```text
250 ms
500 ms
1000 ms
1500 ms
```

### Result

Errors persisted.

### Conclusion

```text
Hypothesis rejected
```

---

## Phase 4 — WS1850S Driver Review

### Finding 1

Timeout handling inside:

```text
ws1850s_transceive()
```

was incomplete.

### Fix

Added explicit timeout detection.

---

### Finding 2

Multiple register operations ignored return values.

### Fix

Added return validation.

---

### Finding 3

The driver reinitialized WS1850S during every UID read.

Previous behavior:

```text
UID read
↓
ws1850s_init()
↓
REQA
↓
Anti-collision
```

### Fix

Initialization restricted to:

```text
NFC_INIT
```

state only.

---

### Finding 4

Timeouts were treated as NFC failures.

### Fix

Changed behavior:

```text
ESP_ERR_TIMEOUT
↓
No Card Present
```

instead of:

```text
ESP_ERR_TIMEOUT
↓
NFC Failure
```

---

# Fast NFC Acquisition Investigation

## Problem

UID acquisition was inconsistent.

Observed behavior:

```text
Card detected
↓
UID read failed
↓
Retry
↓
Eventually succeeds
```

---

## Solution

Implemented:

```text
Fast NFC Acquisition Retry
```

Features:

* Multiple acquisition attempts
* Controlled retry intervals
* NFC reacquisition strategy
* Reduced transient failures

---

# Results

Validated cards:

```text
8804DC32
→ Claudio / owner

88048667
→ Student / learner
```

Unknown cards:

```text
8804D4BF
→ Unknown / visitor

88045E34
→ Unknown / visitor

8804913C
→ Unknown / visitor
```

All handled correctly.

---

# Current Behavior

## Functional

### NFC Detection

PASS

### UID Reading

PASS

### Profile Mapping

PASS

### Unknown UID Handling

PASS

### Identity Package Generation

PASS

### Card Removal Detection

PASS

### Recovery

PASS

---

# Remaining Errors

Occasionally observed:

```text
ESP_ERR_INVALID_STATE

I2C transaction failed
```

Typical sequence:

```text
NFC polling
↓
I2C transaction failed
↓
ESP_ERR_INVALID_STATE
↓
Recovery Level 1
↓
Recovery Level 2
↓
NFC INIT
↓
WS1850S reinitialized
↓
NFC operational again
```

---

# Current Assessment

The residual errors:

* do not crash the firmware;
* do not freeze the UI;
* do not prevent UID acquisition;
* do not prevent profile mapping;
* do not prevent recovery.

The issue is therefore classified as:

```text
RELIABILITY ISSUE
NOT A FUNCTIONAL BLOCKER
```

---

# Open Hypotheses

## H1 — Internal M5Dial I2C Interaction

Possible shared access between:

```text
Touch controller
RTC
Power management
WS1850S
```

---

## H2 — WS1850S Driver Limitation

Low-level transaction timing issue.

---

## H3 — M5Unified Interaction

Potential interaction between:

```text
M5Unified
WS1850S raw driver
ESP-IDF I2C driver
```

---

# Recommended Future Investigation

## LAB_WS1850S_002

Suggested scope:

### Transaction Logging

Add detailed tracing around:

```text
REQA
Anti-collision
FIFO operations
IRQ polling
```

---

### Bus Ownership Audit

Review:

```text
M5Unified
Touch
WS1850S
```

for shared internal I2C access.

---

### Alternative Driver Strategy

Evaluate:

```text
M5Unified NFC APIs
```

if available.

---

# Final Conclusion

The Identity Console V1 FreeRTOS stabilization effort was successful.

The remaining issue is isolated to NFC driver reliability and should be investigated separately.

Current status:

```text
Functional Stability ........ PASS
Runtime Stability ........... PASS
Identity Pipeline ........... PASS
NFC Reliability ............. ACCEPTABLE
Driver Reliability .......... INVESTIGATE
```

The system is considered suitable for continued development of the Ambient Physical AI Identity Layer.
