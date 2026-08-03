# WS1850S Component

## Ambient Physical AI

### Reusable NFC Reader Driver

## Purpose

The `ws1850s` component provides the hardware abstraction layer used by
the Identity Node to communicate with the WS1850S RFID/NFC controller
through the ESP-IDF I²C Master driver.

It intentionally encapsulates low-level NFC controller operations while
leaving identity resolution, profile mapping, semantic processing and
network communication to higher software layers.

## Current Status

``` text
WS1850S Component

VALIDATED
```

## Responsibilities

-   Controller initialization
-   Register read/write operations
-   Bit-mask manipulation
-   ISO/IEC 14443 Type A polling (REQA)
-   Anti-collision (Cascade Level 1)
-   Four-byte UID acquisition
-   Communication timeout handling
-   Controller error propagation

## Architecture

``` text
Identity Node
    ↓
ws1850s Component
    ↓
ESP-IDF I2C Master Driver
    ↓
WS1850S Controller
    ↓
ISO/IEC 14443 Type A Card
```

## Public API

Defined in `include/ws1850s.h`.

## Design Principles

The component is intentionally limited to NFC controller communication.

It does not implement identity resolution, profile mapping, JSON
generation, UDP communication or Identity Package generation.

## Repository Structure

``` text
components/ws1850s/
├── CMakeLists.txt
├── ws1850s.c
└── include/ws1850s.h
```

## Integration

Consumed by the Identity Node as a reusable NFC driver.

## Current Limitations

Current implementation supports ISO/IEC 14443 Type A cards with
four-byte UIDs obtained through Cascade Level 1.
