# RGB Strip Node V1

> Ambient Physical AI\
> **Expression Layer -- RGB Strip Node**

The RGB Strip Node is the first validated **physical expression device**
of the Ambient Physical AI ecosystem. It converts high-level Semantic
Events produced by the Cognitive Runtime into observable RGB light
patterns.

------------------------------------------------------------------------

# Purpose

Unlike the Identity Node, this firmware **does not identify users** and
**does not make decisions**.

Its responsibility is to express the **current semantic state of the
system**.

Examples:

-   idle
-   presence_detected
-   identity_authenticated
-   processing
-   system_error

The same semantic event always produces the same visual behavior,
regardless of who triggered it.

------------------------------------------------------------------------

# Position in the Architecture

``` text
                 Ambient Physical AI

 Presence Layer
        │
        ▼
 Identity Layer (M5Dial)
        │
        ▼
 Cognitive Runtime (AX630C)
        │
        ▼
 Semantic Dispatcher
        │
        ├──────────────► StackChan
        │
        ├──────────────► Ambient Runtime (Tab5)
        │
        └──────────────► RGB Strip Node  ← this firmware
                                │
                                ▼
                      Physical RGB Expression
```

------------------------------------------------------------------------

# Internal Software Architecture

``` text
Wi-Fi Station
      │
      ▼
Semantic Receiver (UDP :5555)
      │
      ▼
Semantic Consumer
      │
      ▼
Expression Processor
      │
      ▼
Semantic RGB Effects
      │
      ▼
WS2812 Driver
```

Each layer has a single responsibility, making the firmware modular and
easy to extend.

------------------------------------------------------------------------

# Communication Contract

Incoming message:

``` json
{
  "type":"semantic_event",
  "event":"identity_authenticated",
  "target":"rgb_strip"
}
```

The node reacts only to semantic events addressed to `rgb_strip`.

------------------------------------------------------------------------

# Repository

``` text
rgb-strip-node/
├── main/
├── components/
├── CMakeLists.txt
├── sdkconfig.defaults
└── README.md
```

------------------------------------------------------------------------

# Build

``` bash
idf.py set-target esp32
idf.py build
idf.py flash
idf.py monitor
```

------------------------------------------------------------------------

# Validation Status

The following end-to-end workflow has been successfully validated:

``` text
M5Dial
   ↓
Identity Package
   ↓
AX630C Cognitive Runtime
   ↓
Semantic Event Generator
   ↓
Semantic Dispatcher
   ↓
RGB Strip Notifier
   ↓
RGB Strip Node
   ↓
WS2812 Physical Expression
```

Validated capabilities:

-   ESP-IDF firmware
-   Wi-Fi Station
-   UDP Semantic Receiver
-   Semantic Consumer
-   Expression Processor
-   WS2812 control
-   End-to-end Cognitive Runtime integration

------------------------------------------------------------------------

# Future Evolution

Future work focuses on richer semantic behavior rather than architecture
changes:

-   automatic transition from `identity_authenticated` to `ready`/`idle`
-   richer animations
-   effect priorities
-   brightness adaptation
-   additional semantic states

The communication architecture has already been validated.

------------------------------------------------------------------------

# Project

This firmware is part of the **Ambient Physical AI** project and
represents the first validated implementation of the Expression Layer.
