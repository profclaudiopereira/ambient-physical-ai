# Echo Pyramid + AtomS3R Arduino Bring-up

## Ambient Physical AI

### Expression Layer Hardware Characterization Laboratory

---

## Objective

This laboratory documents the first hardware characterization of the **Echo Pyramid + AtomS3R** platform using the official Arduino examples and libraries provided by M5Stack.

This is **not** the implementation of the Ambient Physical AI Expression Layer.

Its purpose is to validate and understand the hardware before developing the official ESP-IDF implementation.

---

# Hardware

| Component            | Description                                        |
| -------------------- | -------------------------------------------------- |
| Echo Pyramid         | Voice interaction and expression hardware platform |
| AtomS3R              | ESP32-S3 host controller                           |
| USB-C (AtomS3R)      | Firmware upload and serial monitor                 |
| USB-C (Echo Pyramid) | Normal operating power supply                      |

---

# Arduino Libraries

The following official libraries were used during this laboratory:

| Library        | Purpose                       |
| -------------- | ----------------------------- |
| M5Unified      | Device initialization         |
| M5GFX          | Graphics support              |
| M5Echo-Pyramid | Echo Pyramid hardware control |

---

# Official Hardware Baseline

Before developing custom firmware, the official M5Stack firmware was validated.

| Item      | Value                                |
| --------- | ------------------------------------ |
| Firmware  | Echo Pyramid Xiaozhi Voice Assistant |
| Version   | v2.2.6                               |
| Publisher | M5Stack                              |
| Status    | Successfully validated               |

The XiaoZhi firmware was used exclusively to validate the hardware platform.

It is **not** part of the Ambient Physical AI architecture.

---

# Laboratory Results

---

## A01 — RGB Validation

### Example

```
A01_rgb_validation/A01_rgb_validation.ino
```

### Result

| Item                       | Status |
| -------------------------- | ------ |
| Compilation                | ✅      |
| Upload                     | ✅      |
| RGB Control                | ✅      |
| RGB Animation              | ✅      |
| Echo Pyramid Communication | ✅      |

### Observation

The AtomS3R successfully initialized and controlled the Echo Pyramid RGB LEDs using the official M5Echo-Pyramid library.

---

## A03 — Record & Playback Validation

### Example

```
A03_record_play_validation/A03_record_play_validation.ino
```

### Result

| Item               | Status |
| ------------------ | ------ |
| Compilation        | ✅      |
| Upload             | ✅      |
| RGB Rainbow Effect | ✅      |
| Touch Trigger      | ✅      |
| Microphone Capture | ✅      |
| Speaker Playback   | ✅      |
| Audio Codec        | ✅      |

---

## Volume Characterization

The official example initializes the codec with:

```cpp
ep.codec().setVolume(50);
```

Experimental evaluation on the laboratory hardware produced the following results:

| Volume | Result                |
| ------ | --------------------- |
| 50     | Playback too low      |
| 70     | Clear and comfortable |
| 100    | Audible but distorted |

For this hardware unit, **volume level 70** produced the best playback quality.

---

# Partial Findings

---

## Touch Characterization

Touch input was successfully validated **indirectly** using the official Record & Playback example.

The touch pads correctly triggered audio recording.

However, the low-level touch interface has **not** yet been fully characterized.

Exploratory observations:

| Attempt           | Result                                                          |
| ----------------- | --------------------------------------------------------------- |
| `getTouchValue()` | Function does not exist                                         |
| `getTouchRaw()`   | API available but returned only zero during exploratory testing |
| `isPressed(1..4)` | Successfully used by the official example                       |

Further investigation is still required before using touch as an official Ambient Physical AI input interface.

---

## AudioStage

The official **AudioStage** example was inspected but intentionally not validated during this laboratory.

Important architectural observations:

| Feature                | Observation                                   |
| ---------------------- | --------------------------------------------- |
| FreeRTOS               | Dedicated audio task                          |
| SPIFFS                 | WAV playback                                  |
| Custom Flash Partition | PCM recording                                 |
| Beat Detection         | RGB synchronized with audio energy            |
| Complexity             | More advanced than the current bring-up scope |

The example is considered an excellent architectural reference for the future ESP-IDF implementation.

---

# Engineering Conclusions

The Echo Pyramid + AtomS3R platform has been successfully validated as a candidate hardware platform for the Ambient Physical AI Expression Layer.

Validated capabilities:

* RGB expression
* Touch-triggered interaction
* Microphone capture
* Speaker playback
* Audio codec initialization
* Official Arduino library support
* Official M5Stack firmware baseline

---

# Next Phase

The next development phase will migrate from Arduino to **ESP-IDF**.

The first ESP-IDF milestone will validate:

```
AtomS3R Boot
        │
        ▼
Echo Pyramid Initialization
        │
        ▼
RGB Control
        │
        ▼
Wi-Fi Connectivity
        │
        ▼
Reception of Semantic Events
        │
        ▼
Expression Reaction
```

---

# Architectural Note

The Arduino examples were used exclusively for hardware characterization.

The official Ambient Physical AI implementation will be developed using **ESP-IDF**.

The Expression Layer shall consume only **Semantic Events** produced by the Cognitive Runtime.

It shall not consume:

* Identity Packages
* 5W Context Packages
* Current Runtime Context
* Decision Engine internal state

Those remain private cognitive representations inside the Cognitive Runtime.


Additional engineering observations are documented in:

docs/discoveries/echo-pyramid-atoms3r-characterization.md