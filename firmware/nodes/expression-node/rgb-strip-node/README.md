# RGB Strip Node V1

> **Ambient Physical AI**\
> **Expression Layer --- RGB Strip Node**

## Overview

The RGB Strip Node is the first physical expression device of the
Ambient Physical AI ecosystem.

Its responsibility is not to identify users or make decisions. It
receives Semantic Events from the Cognitive Runtime and converts them
into visible RGB patterns representing the current semantic state.

## Status

``` text
RGB_STRIP_NODE_V1
VALIDATED
```

## Architecture

``` text
Wi-Fi
  ↓
UDP Receiver (:5555)
  ↓
Semantic Consumer
  ↓
Expression Processor
  ↓
RGB Effects
  ↓
WS2812
```

## Semantic Events

Current examples:

-   idle
-   presence_detected
-   identity_authenticated
-   processing
-   system_error

The RGB Strip expresses **system semantic state**, not user identity.

## Validated End-to-End Flow

``` text
M5Dial
    ↓
Identity Package
    ↓
AX630C Cognitive Runtime
    ↓
Semantic Dispatcher
    ↓
RGB Strip Notifier
    ↓
RGB Strip Node
    ↓
Physical RGB Expression
```

## Build

``` bash
idf.py set-target esp32
idf.py build
idf.py flash
idf.py monitor
```

## Future Work

-   Automatic transition from authentication to ready/idle
-   Richer animations
-   Priority management
-   Additional semantic states
