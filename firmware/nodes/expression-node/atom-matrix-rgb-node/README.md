# Atom Matrix RGB Node

> **Ambient Physical AI**\
> **Expression Layer Runtime Node**

The Atom Matrix RGB Node is a production firmware component of the
**Ambient Physical AI** project. It receives semantic events from the
distributed runtime and expresses the current system state using the
integrated 5×5 WS2812 RGB LED matrix of the M5Stack Atom Matrix.

------------------------------------------------------------------------

# Objectives

-   Receive semantic events over UDP.
-   Process events through the Expression Layer.
-   Convert semantic states into visual feedback.
-   Provide a lightweight, reproducible RGB expression node.

------------------------------------------------------------------------

# Hardware

  Component   Description
  ----------- ----------------------------------
  MCU         ESP32
  Board       M5Stack Atom Matrix
  Display     Integrated 5×5 WS2812 RGB Matrix
  LED GPIO    GPIO 27
  Framework   ESP-IDF

------------------------------------------------------------------------

# Software Architecture

``` text
Semantic Event
        │
        ▼
Semantic Receiver
        │
        ▼
Semantic Consumer
        │
        ▼
Expression Processor
        │
        ▼
RGB Effects
        │
        ▼
RGB Controller
        │
        ▼
5×5 RGB Matrix
```

Each software module has a single responsibility, making the node easy
to maintain and consistent with the Ambient Physical AI architecture.

------------------------------------------------------------------------

# Repository Structure

``` text
firmware/
└── nodes/
    └── expression-node/
        └── atom-matrix-rgb-node/
            ├── CMakeLists.txt
            ├── sdkconfig.defaults
            └── main/
                ├── atom_matrix_rgb_node.c
                ├── semantic_receiver.c
                ├── semantic_consumer.c
                ├── expression_processor.c
                ├── rgb_effects.c
                ├── rgb_controller.c
                └── wifi_station.c
```

------------------------------------------------------------------------

# Runtime Flow

``` text
Wi-Fi Connection
        │
        ▼
UDP Semantic Event
        │
        ▼
Expression Processing
        │
        ▼
RGB Effect Selection
        │
        ▼
LED Matrix Update
```

------------------------------------------------------------------------

# Supported Semantic Events

Current implementation supports:

-   boot
-   idle
-   presence_detected
-   identity_authenticated
-   processing
-   error

Future semantic states will follow the official Ambient Physical AI
semantic specification.

------------------------------------------------------------------------

# Build Requirements

-   ESP-IDF v5.4.x
-   Python environment configured by ESP-IDF
-   USB connection to the Atom Matrix

------------------------------------------------------------------------

# Build

``` bash
idf.py build
```

------------------------------------------------------------------------

# Flash

``` bash
idf.py -p COMx flash
```

------------------------------------------------------------------------

# Serial Monitor

``` bash
idf.py monitor
```

------------------------------------------------------------------------

# Validation Procedure

1.  Build the firmware.
2.  Flash the device.
3.  Connect to the configured Wi-Fi network.
4.  Verify UDP reception.
5.  Trigger semantic events.
6.  Confirm the RGB matrix changes according to the received event.

------------------------------------------------------------------------

# Current Status

  Item                  Status
  --------------------- --------------
  Firmware              ✅ Validated
  Wi-Fi                 ✅ Validated
  UDP Communication     ✅ Validated
  Semantic Processing   ✅ Validated
  RGB Matrix            ✅ Validated
  Integration           ✅ Ready

------------------------------------------------------------------------

# Documentation

This README is intended as the repository entry point.

Detailed engineering documentation, architectural specifications and
integration documents are maintained separately under the project's
documentation directories.

------------------------------------------------------------------------

# License

Part of the **Ambient Physical AI** project.

Distributed under the repository license.
