# Wake Word Calibration and Build Recovery

## Ambient Physical AI

### Engineering Guide

**Document Type:** Engineering Guide

**Scope:** Echo Pyramid Voice Node, ESP-SR Calibration, Build Recovery, Reproducible Development Environment

**Status:** Calibration Procedure

**Baseline Branch:** `feature/ambient-runtime-node`

**Primary Firmware Path:**

```text
firmware/nodes/expression-node/echo-pyramid-voice-node/
```

---

# Purpose

This document consolidates the engineering procedures required to recover a reproducible build environment and to perform controlled Wake Word calibration for the Echo Pyramid Voice Node.

Unlike the Technical Notes that describe implemented functionality, this guide documents engineering procedures that may be repeated whenever the firmware, ESP-SR configuration or development environment must be validated.

The objectives are:

- reproduce a valid build environment;
- recover the ESP-SR configuration;
- validate Flash and partition settings;
- compare Wake Word operating modes;
- document a repeatable calibration methodology.

---

# Current Engineering Status

The complete voice interaction architecture has already been validated.

Current validated capabilities include:

```text
✓ WakeNet initialization

✓ MultiNet initialization

✓ Offline context recognition

✓ Context-change request generation

✓ AX630C integration

✓ Semantic event propagation

✓ Personalized voice playback
```

The remaining engineering activity concerns recognition quality rather than functional correctness.

Current project status:

```text
Core Voice Integration

VALIDATED

Wake Word Calibration

IN PROGRESS
```

This distinction is important.

The architecture is operational.

Calibration focuses on improving robustness, usability and reproducibility.

---

# AFE Performance Evaluation

One engineering experiment remains under evaluation.

The Audio Front-End operating mode was changed from:

```text
AFE_MODE_LOW_COST
```

to:

```text
AFE_MODE_HIGH_PERF
```

The objective is to determine whether the higher-performance processing pipeline improves Wake Word detection under identical operating conditions.

At the time this guide was prepared, the comparison had not yet been completed.

Consequently, no recommendation is made regarding the preferred operating mode.

Only controlled measurements should determine the final configuration.

---

# Flash Configuration

The validated hardware platform provides:

```text
8 MB Flash
```

Correct Flash configuration is mandatory before enabling ESP-SR model packaging.

The firmware configuration must report:

```text
Flash Size

8 MB
```

Any configuration reporting a smaller Flash device shall be considered invalid for the current project baseline.

---

# Partition Configuration

The firmware uses a custom partition table specifically designed for ESP-SR.

Conceptually:

```text
NVS

↓

PHY

↓

Factory Application

↓

Speech Models
```

The dedicated model partition allows WakeNet and MultiNet models to coexist with the firmware application without memory overlap.

Both Flash size and partition configuration must always be validated together.

---

# Configuration Verification

After saving the firmware configuration, verify the generated `sdkconfig`.

Typical verification commands:

```cmd
findstr /I "CONFIG_ESPTOOLPY_FLASHSIZE" sdkconfig

findstr /I "CONFIG_PARTITION_TABLE" sdkconfig
```

The generated configuration must indicate:

- 8 MB Flash;
- custom partition table;
- project partition file.

Any configuration indicating a 2 MB Flash device or the default single-application partition shall be corrected before continuing.

---

# Build Recovery

When rebuilding the firmware under Windows, UTF-8 support must be enabled before invoking ESP-IDF.

Recommended build session:

```cmd
set PYTHONUTF8=1
set PYTHONIOENCODING=utf-8
chcp 65001
```

Recommended recovery sequence:

```cmd
idf.py fullclean

idf.py reconfigure

idf.py build
```

This sequence ensures that stale build artifacts do not interfere with ESP-SR model packaging.

---

# Build Verification

A successful build shall confirm:

```text
✓ ESP32-S3 target

✓ 8 MB Flash

✓ Custom partition table

✓ ESP-SR models packaged

✓ WakeNet model available

✓ MultiNet model available
```

Any build reporting that no speech models were packaged shall be considered unsuccessful for the current engineering baseline.

The ESP-SR configuration should be reviewed before attempting another firmware flash.

---

# Controlled Calibration Principle

Wake Word calibration must follow a reproducible engineering methodology.

Only one configuration parameter should be modified during each evaluation cycle.

Recommended comparison strategy:

```text
Baseline Configuration

↓

Modify One Parameter

↓

Repeat Test

↓

Compare Results

↓

Record Measurements
```

Changing multiple parameters simultaneously prevents reliable identification of the factor responsible for an improvement or regression.

This principle shall be preserved throughout every calibration session.

---

# Controlled Calibration Procedure

Wake Word calibration shall always be performed under controlled and reproducible conditions.

The objective is to evaluate the influence of a single configuration parameter while keeping every other condition unchanged.

Recommended baseline:

```text
Same firmware

Same hardware

Same ESP-SR models

Same speaker

Same microphone distance

Same room

No intentional background noise

No concurrent TTS playback
```

Suggested evaluation sequence:

```text
20 Wake Word attempts

↓

Record successful detections

↓

Record false activations

↓

Measure comfortable speaking distance

↓

Repeat using identical conditions
```

The collected data should be documented before changing any additional parameter.

---

# AFE Comparison

The primary comparison currently under evaluation involves the Audio Front-End operating mode.

Example methodology:

```text
Configuration A

AFE_MODE_LOW_COST

↓

20 Attempts

↓

Record Results

↓

Configuration B

AFE_MODE_HIGH_PERF

↓

20 Attempts

↓

Record Results
```

Only retain the higher-performance mode if it provides a clear improvement while preserving:

- firmware stability;
- memory availability;
- MultiNet operation;
- audio playback reliability.

Engineering decisions shall be based on measured results rather than subjective perception.

---

# Additional Parameters for Review

If Wake Word performance remains unsatisfactory after the AFE comparison, additional parameters may be evaluated individually.

Recommended investigation order:

1. Confirm the active AFE mode.
2. Confirm WakeNet model loading.
3. Confirm MultiNet model loading.
4. Review Voice Activity Detection (VAD).
5. Evaluate microphone gain.
6. Measure captured PCM amplitude.
7. Validate microphone/reference channel ordering.
8. Review Acoustic Echo Cancellation (AEC).
9. Evaluate WakeNet sensitivity.
10. Review command-window duration.

Only one parameter should be modified during each validation cycle.

---

# Audio Channel Validation

The firmware processes both microphone and playback reference signals.

Conceptually:

```text id="cvb1fd"
Microphone

↓

Audio Front-End

↓

WakeNet

↓

MultiNet
```

The playback reference is required to support acoustic echo cancellation during simultaneous speech playback.

The following characteristics should be verified whenever audio behavior appears abnormal:

- channel ordering;
- synchronization;
- sample rate;
- frame size;
- playback reference integrity.

Permanent changes to Acoustic Echo Cancellation should not be introduced without validating both speech recognition and TTS playback.

---

# Accessibility Considerations

Wake Word calibration shall not be optimized exclusively for ideal laboratory conditions.

Validation should include users presenting different speaking characteristics.

Recommended scenarios include:

- softer voices;
- higher and lower vocal pitch;
- different speaking distances;
- slower and faster speech;
- mild ambient noise;
- users with articulation differences.

The engineering objective is comfortable interaction rather than maximum sensitivity under ideal conditions.

---

# Calibration Completion Criteria

Wake Word calibration may be considered complete only after all of the following conditions are satisfied.

```text id="8n0n5g"
✓ Reproducible firmware build

✓ Correct ESP-SR model packaging

✓ Reliable firmware flashing

✓ Stable Wake Word detection

✓ Controlled false activations

✓ Stable MultiNet recognition

✓ Stable TTS playback

✓ Complete end-to-end validation
```

The final validation shall include the complete interaction sequence.

```text id="5dr8pz"
Identity Authentication

↓

Personalized Greeting

↓

Wake Word

↓

Offline Context Recognition

↓

Context Change Request

↓

Context Validation

↓

Semantic Event

↓

Distributed Propagation
```

Calibration shall only be considered complete after the entire workflow has been revalidated successfully.

---

# Engineering Recommendations

The following engineering practices should be preserved throughout future development.

- Maintain implementation as the authoritative engineering reference.
- Preserve reproducible build procedures.
- Keep Flash and partition configuration synchronized.
- Validate ESP-SR model packaging after every significant configuration change.
- Record calibration results instead of relying on subjective impressions.
- Preserve modular component responsibilities.
- Revalidate the complete interaction pipeline whenever speech recognition parameters are modified.

These practices improve long-term maintainability and reduce the risk of introducing regressions during future firmware evolution.

---

# Related Documentation

This engineering guide complements the following documents:

- `README.md`
- `TECHNICAL_NOTE_PERSONALIZED_WELCOME_INTEGRATION.md`
- `ECHO_PYRAMID_VOICE_INTEGRATION_MILESTONE.md`
- `ECHO_PYRAMID_WAKE_WORD_CONTEXT_INTEGRATION_TECHNICAL_NOTE.md`

Together, these documents describe:

- firmware architecture;
- engineering decisions;
- voice interaction implementation;
- runtime integration;
- calibration methodology;
- reproducible development procedures.

---

# Engineering Conclusion

The Echo Pyramid Voice Node has reached a validated functional baseline for bidirectional voice interaction.

The remaining engineering work is focused on calibration, reproducibility and usability refinement rather than architectural changes.

Maintaining disciplined calibration procedures, reproducible build environments and isolated parameter evaluation will ensure that future improvements preserve the validated engineering baseline while increasing recognition reliability and user accessibility.

---

# Guide Status

```text
WAKE_WORD_CALIBRATION_AND_BUILD_RECOVERY

Status

ACTIVE ENGINEERING GUIDE
```