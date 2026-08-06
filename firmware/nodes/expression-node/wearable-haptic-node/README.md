# Wearable Haptic Node

## Ambient Physical AI

### Expression Layer

------------------------------------------------------------------------

# Repository Overview

The **Wearable Haptic Node** is a dedicated Expression Layer node
responsible for presenting the internal execution state of the Cognitive
Runtime through **visual and haptic feedback**.

Unlike the Runtime State RGB Node, which emphasizes visual indication
using an RGB light bar, the Wearable Haptic Node provides tactile
confirmation using the official **M5Stack Vibrator HAT** while
preserving the same Runtime State communication contract.

## Mission

Provide synchronized Runtime State feedback through the TFT display and
a discrete haptic pulse.

## Hardware Platform

-   M5StickC Plus2
-   ESP32
-   Integrated TFT display
-   Official M5Stack Vibrator HAT
-   Wi-Fi Station Mode

## Expression Layer Architecture

``` text
                 Cognitive Runtime (AX630C)
                           │
                     Runtime State
                           │
          ┌────────────────┴────────────────┐
          │                                 │
          ▼                                 ▼
 Runtime State RGB Node           Wearable Haptic Node
      (M5StickS3)                 (M5StickC Plus2)

 Display + WS2812                Display + Vibrator HAT
```

Both nodes consume exactly the same Runtime State messages while
exposing different interaction modalities.

## Runtime State Pipeline

Semantic Dispatcher → RuntimeStateNotifier → UDP → Semantic Receiver →
Semantic Consumer → Expression Processor → Display + Haptic

## Haptic Feedback Policy

Only the **RESPONDING** Runtime State generates a single vibration
pulse.

  Runtime State    Display       Haptic
  --------------- --------- ----------------
  boot                ✓           ---
  idle                ✓           ---
  listening           ✓           ---
  thinking            ✓           ---
  responding          ✓      ✓ Single Pulse

## Network Communication

-   UDP / Port 5555
-   Default IP: 192.168.77.208

## Firmware Architecture

-   semantic_receiver
-   semantic_consumer
-   expression_processor
-   vibrator
-   wifi_station

## Build

``` bash
idf.py set-target esp32
idf.py build
idf.py flash
idf.py monitor
```

## Validation

-   Runtime State reception
-   Wi-Fi connectivity
-   Runtime State display
-   Single haptic pulse for `responding`
-   Synchronized operation with the Runtime State RGB Node

## Engineering Notes

The Wearable Haptic Node was introduced during the consolidation of the
Ambient Physical AI Expression Layer as the dedicated tactile
interaction node while preserving the common Runtime State protocol.

## Related Documentation

-   `docs/notes/TECHNICAL_NOTE_EXPRESSION_LAYER_HARDWARE_SPECIALIZATION.md`
    (planned)
-   Runtime State RGB Node README
-   Ambient Physical AI System Architecture

## Future Work

-   Profile-dependent haptic patterns
-   Personalized wearable notifications
-   Accessibility-oriented haptic feedback
