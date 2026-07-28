# AtomS3R LCD Bring-up V2

## What changed from V1

V2 corrects the integration package in four important ways:

1. `main.c` is already supplied with exactly one `display_manager_init()` call.
2. Both Echo Pyramid and LCD backlight use the ESP-IDF 5.4 new I2C master API.
3. The LCD initialization uses the command subset shared by the AtomS3R
   GC9107 and ST7735 production revisions.
4. Missing FreeRTOS and error-handling includes were corrected.

## Installation

In this project folder:

```text
firmware/nodes/expression-node/echo-pyramid-voice-node/main/
```

make a backup of the current files and then replace:

```text
main.c
CMakeLists.txt
display_manager.c
display_manager.h
lcd_driver.c
lcd_driver.h
```

Do not create `idf_component.yml`.

## Build

From the project root:

```bat
idf.py fullclean
idf.py build
```

Only after a successful build:

```bat
idf.py -p COMx flash monitor
```

Replace `COMx` with the AtomS3R port.

## Expected LCD sequence

```text
BLACK
WHITE
RED
GREEN
BLUE
BLACK
```

The test finishes and the existing application continues with Wi-Fi, UDP and
Echo Pyramid RGB operation.

## What to report

Send the complete build output if compilation fails.

After flash, report:

- whether the backlight turned on;
- which colors appeared;
- whether the entire screen was filled;
- whether colors were correct;
- whether Wi-Fi connected;
- whether Echo Pyramid RGB still reacted to UDP;
- all lines tagged `atoms3r_lcd` or `display_manager`.

## Hardware revision note

M5Stack records a production change on 2026-05-14 from GC9107 to ST7735.
A unit purchased in May 2026 may be near that transition. V2 therefore avoids
controller-specific gamma tables during the first validation and uses only the
shared command path. A lit backlight without valid full-screen colors is a
diagnostic result, not a reason to modify unrelated firmware.
