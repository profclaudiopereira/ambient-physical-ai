# NOTE_002 — SH1107 Display Geometry Validation

## Ambient Physical AI

### Engineering Technical Note

---

# Overview

This technical note documents the engineering investigation that led to the validated initialization sequence for the SH1107 controller used by the M5Stack Unit OLED integrated into the Ambient Runtime.

The purpose of this document is to preserve the engineering rationale behind the current implementation.

It complements the component documentation and should not be considered a replacement for the `oled_sh1107` component README.

---

# Scope

This investigation applies exclusively to the `oled_sh1107` ESP-IDF component located at:

```text
firmware/
└── nodes/
    └── ambient-runtime-node/
        └── components/
            └── oled_sh1107/
```

No modifications were required in higher-level runtime components.

---

# Background

During the integration of the Mini OLED display into the Ambient Runtime, the display initialized successfully but presented incomplete rendering.

Changing the column offset merely shifted the clipped region from one side of the display to the other.

This behavior indicated that the problem was not related to text rendering, page addressing or I²C communication, but rather to the controller initialization sequence.

---

# Observed Symptoms

The observed behavior included:

- incomplete rendering near the display borders;
- clipped characters;
- horizontal displacement when changing column offsets;
- successful I²C communication;
- successful display initialization.

The symptoms were reproducible across multiple firmware builds.

---

# Investigation

The initialization sequence implemented in the custom ESP-IDF driver was compared against the official initialization adopted by the M5Stack implementation.

Special attention was given to:

- multiplex configuration;
- page addressing;
- column addressing;
- initialization command order.

---

# Root Cause

The previous driver configured:

```c
0xA8, 0x3F
```

which enables only 64 multiplex rows.

The M5Stack Unit OLED uses the SH1107 controller in its native **64 × 128** organization.

The official M5GFX `Panel_SH110x` implementation initializes the controller using:

```c
0xA8, 0x7F
```

Using the incomplete multiplex configuration produced an incorrect internal display mapping.

As a consequence, adjusting the column offset merely moved the clipping region rather than correcting the geometry.

---

# Validated Solution

The solution consisted of aligning the custom ESP-IDF driver with the validated initialization sequence used by the official M5Stack implementation.

The adopted changes included:

- corrected multiplex configuration;
- validated initialization order;
- validated page addressing;
- validated column addressing.

No changes were required in:

- `oled_context_presenter`;
- `semantic_event_receiver`;
- `ambient_context_snapshot`;
- `main.cpp`;
- PaHub routing.

Only the SH1107 driver implementation required modification.

---

# Validation Procedure

Geometry validation was performed using the dedicated diagnostic routine:

```c
oled_sh1107_print_geometry_test();
```

Expected output:

```text
1234567890
ABCDEFGHIJ
|        |
CONTEXT
```

Validation criteria:

- the first `1`, `A` and `|` must be completely visible;
- the final `0`, `J` and `|` must be completely visible;
- `CONTEXT` must be rendered without clipping.

After successful validation, the runtime returned to its normal execution path:

```c
oled_context_presenter_render(...);
```

---

# Display Configuration

The validated implementation retains:

```c
#define OLED_COL_OFFSET 32
```

Further offset experimentation is not recommended unless new hardware validation is performed.

---

# Runtime Integration

After geometry validation, the SH1107 driver integrates into the Ambient Runtime as illustrated below.

```text
Cognitive Runtime
        │
        ▼
semantic_event_receiver
        │
        ▼
ambient_context_snapshot
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
Mini OLED
```

The SH1107 driver remains responsible only for low-level display services.

Presentation logic belongs exclusively to the `oled_context_presenter` component.

---

# Build

After replacing the driver implementation, rebuild the firmware using the standard ESP-IDF workflow.

```bash
idf.py fullclean
idf.py build
idf.py flash monitor
```

Although a normal rebuild may be sufficient, `fullclean` is recommended after replacing low-level driver files to avoid stale component objects.

---

# Engineering Decision

The current SH1107 initialization sequence has been validated against the official M5Stack implementation and adopted as the baseline for the Ambient Runtime.

Future modifications to the initialization sequence should only be performed after hardware validation demonstrating a measurable engineering benefit.

---

# Related Components

The investigation documented in this note is directly related to:

```text
components/
├── oled_sh1107/
├── oled_context_presenter/
└── pahub/
```

---

# Related Documentation

For permanent component documentation, refer to:

```text
components/oled_sh1107/README.md
```

For the Ambient Runtime architecture:

```text
firmware/nodes/ambient-runtime-node/README.md
```

---

# Conclusion

This investigation identified the root cause of the display geometry issue observed during the integration of the M5Stack Unit OLED into the Ambient Runtime.

By aligning the SH1107 initialization sequence with the validated M5Stack implementation, the display geometry was restored without requiring modifications to higher-level runtime components.

This document preserves the engineering rationale behind the adopted solution and serves as a historical reference for future maintenance of the SH1107 driver.