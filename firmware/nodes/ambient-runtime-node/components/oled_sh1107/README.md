# M5Stack Unit OLED SH1107 — ESP-IDF corrective package

This package replaces only the custom `oled_sh1107` component implementation.

It does **not** introduce Arduino, M5GFX, LovyanGFX or Arduino-ESP32 into the
project. The component remains native ESP-IDF and continues using
`driver/i2c_master.h`.

## Root cause addressed

The previous driver configured:

```c
0xA8, 0x3F
```

which enables only 64 multiplex rows.

The M5Stack Unit OLED uses the SH1107 controller in a native 64 x 128
organization. The official M5GFX `Panel_SH110x` initialization uses:

```c
0xA8, 0x7F
```

The incomplete multiplex configuration can produce a partially mapped display
where changing the column offset merely moves clipping from one side to the
other.

This package also aligns the remaining initialization sequence and page/column
address order with the official M5Stack implementation.

## Files to replace

Copy:

```text
oled_sh1107.h
oled_sh1107.c
```

over:

```text
firmware/nodes/ambient-runtime-node/components/oled_sh1107/
```

Do not change:

```text
oled_context_presenter
semantic_event_receiver
ambient_context_snapshot
main.cpp
PaHub routing
```

## First validation

Temporarily call this after OLED initialization and PaHub channel selection:

```c
oled_sh1107_print_geometry_test();
```

Expected content:

```text
1234567890
ABCDEFGHIJ
|        |
CONTEXT
```

Inspect both sides:

- the first `1`, `A` and `|` must be complete;
- the final `0`, `J` and `|` must be complete;
- `CONTEXT` must be complete.

Keep:

```c
#define OLED_COL_OFFSET 32
```

Do not continue offset experiments.

## Returning to the runtime presenter

After the geometry test passes, restore the normal call:

```c
oled_context_presenter_render(...);
```

The existing presenter API and its 16-page layout remain compatible.

## Build

From the project root:

```bash
idf.py fullclean
idf.py build
idf.py flash monitor
```

A normal `idf.py build` may be sufficient, but `fullclean` is recommended for
this one driver replacement so no stale component object remains.

## Rollback

Keep a copy of the previous two files before replacing them. No other component
needs to be reverted if this test does not behave as expected.
