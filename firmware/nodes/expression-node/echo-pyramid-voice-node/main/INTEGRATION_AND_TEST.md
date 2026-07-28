# AtomS3R LCD Bring-up V1

## Scope

This package validates only the AtomS3R internal 128 × 128 LCD.

It does not modify or redesign:

- Wi-Fi;
- UDP semantic events;
- Echo Pyramid RGB;
- Echo Pyramid touch;
- Echo Pyramid audio;
- Cognitive Runtime or StackFlow.

## Files

Copy these files into:

```text
firmware/nodes/expression-node/echo-pyramid-voice-node/main/
```

Replace:

```text
lcd_driver.c
lcd_driver.h
display_manager.c
display_manager.h
CMakeLists.txt
```

## Required call in `main.c`

The firmware must call the display manager once during startup:

```c
#include "display_manager.h"
```

Inside `app_main()`, after the basic platform initialization and before the
main runtime loop or blocking receive loop:

```c
display_manager_init();
```

Do not add the call more than once.

## Build

```bat
idf.py build
```

## Flash and monitor

```bat
idf.py -p COMx flash monitor
```

Replace `COMx` with the AtomS3R serial port.

## Expected visual sequence

```text
BLACK
  ↓
WHITE
  ↓
BLUE
  ↓
BLACK
```

Expected logs include:

```text
I (...) display_manager: Starting AtomS3R LCD bring-up test
I (...) atoms3r_lcd: Initializing AtomS3R 128x128 LCD
I (...) atoms3r_lcd: AtomS3R LCD initialized
I (...) display_manager: LCD test color: WHITE
I (...) display_manager: LCD test color: BLUE
I (...) display_manager: LCD bring-up color sequence completed
```

## Validation reporting

Record exactly what occurred:

1. Did the backlight turn on?
2. Did the panel show white?
3. Did the panel show blue?
4. Was the full 128 × 128 area filled?
5. Were colors correct or swapped?
6. Did the existing Echo Pyramid Wi-Fi/UDP/RGB behavior remain functional?
7. Copy any `atoms3r_lcd` or `display_manager` error logs.

## Important revision note

This first version follows the current official AtomS3R documentation, which
identifies the TFT as ST7735. Because hardware purchased near a controller
transition may behave differently, a lit backlight with no valid colors is
useful diagnostic evidence; do not repeatedly change unrelated firmware.
