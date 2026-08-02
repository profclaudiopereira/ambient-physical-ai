# Wake Word Detector

## Echo Pyramid Voice Node

### Expression Layer — Local Speech Recognition

The **Wake Word Detector** is responsible for local offline speech recognition within the Echo Pyramid Voice Node.

It encapsulates the complete ESP-SR speech recognition pipeline, including Wake Word detection and constrained command recognition.

The component exposes a compact public API while hiding all ESP-SR implementation details from the remainder of the firmware.

This design allows the application to interact with speech recognition exclusively through callbacks and lifecycle control functions.

---

# Purpose

The Wake Word Detector provides the local voice-input interface of the Echo Pyramid Voice Node.

Its primary responsibilities are:

- continuously monitor the microphones;
- detect the configured Wake Word;
- temporarily enable command recognition;
- recognize predefined context commands;
- notify the application through callbacks;
- manage the speech recognition lifecycle.

The component intentionally performs **local speech recognition only**.

It does **not**:

- communicate with the Cognitive Runtime;
- send network packets;
- validate user identity;
- authorize commands;
- modify the active context.

Those responsibilities remain outside this component.

---

# Responsibilities

The component is responsible for:

```text
Microphone Monitoring

↓

WakeNet Detection

↓

Command Window

↓

MultiNet Recognition

↓

Application Callback
```

The remainder of the firmware reacts only to the generated callbacks.

---

# Public API

The public interface intentionally exposes only the operations required by the application.

Initialization:

```c
esp_err_t wake_word_detector_start(
    wake_word_detected_cb_t wake_callback,
    speech_command_detected_cb_t command_callback);
```

The application supplies two callback functions.

One callback is invoked when the Wake Word is detected.

The second callback is invoked when a valid speech command is recognized. :contentReference[oaicite:0]{index=0}

---

## Runtime Control

The detector can temporarily suspend microphone acquisition without destroying the internal ESP-SR objects.

```c
esp_err_t wake_word_detector_pause(void);
```

Resume operation:

```c
esp_err_t wake_word_detector_resume(void);
```

According to the public API, resuming microphone acquisition also clears stale buffered audio before normal operation continues. :contentReference[oaicite:1]{index=1}

This behavior avoids unnecessary reconstruction of the Audio Front-End and recognition models while allowing other firmware components to temporarily own the audio subsystem.

---

## Status Functions

Two helper functions expose the current operational state.

```c
bool wake_word_detector_is_started(void);
```

Returns whether the detector has already been initialized.

```c
bool wake_word_detector_is_listening(void);
```

Returns whether microphone acquisition is currently enabled. :contentReference[oaicite:2]{index=2}

These functions simplify runtime coordination without exposing internal implementation details.

---

# Callback Model

The component follows an event-driven architecture.

```text
Application

↓

wake_word_detector_start()

↓

Register Callbacks

↓

Speech Recognition

↓

Application Notification
```

The application remains responsible for deciding how to react after receiving each callback.

The detector never performs application-specific actions.

---

# Wake Word Detection

WakeNet continuously analyzes microphone input while the detector is active.

```text
Microphones

↓

Audio Front-End

↓

WakeNet

↓

Wake Callback
```

The Wake Word callback informs the application that a valid activation phrase has been recognized.

The callback provides:

- Wake Word index;
- model index. :contentReference[oaicite:3]{index=3}

No semantic processing is performed inside the detector.

---

# Command Recognition

After WakeNet activates the listening window, MultiNet performs constrained speech recognition.

```text
WakeNet

↓

Listening Window

↓

MultiNet

↓

Command Callback
```

The command callback provides:

- recognized command identifier;
- recognition probability. :contentReference[oaicite:4]{index=4}

The interpretation of the command remains the responsibility of higher application layers.

---

# High-Level Architecture

```text
                Wake Word Detector

                     Public API
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
        ▼                 ▼                 ▼
   Lifecycle         WakeNet         MultiNet
 Control API        Detection       Recognition
        │                 │                 │
        └─────────────────┼─────────────────┘
                          │
                     Callback API
                          │
                     Application
```

The component encapsulates the complete speech recognition pipeline while exposing a minimal and stable interface to the rest of the firmware.

---

# Lifecycle Management

The Wake Word Detector behaves as a long-lived runtime service.

The application controls its execution through a small lifecycle interface.

```text
Not Initialized

↓

Start

↓

Listening

↓

Pause

↓

Resume

↓

Listening
```

This model avoids repeated allocation and destruction of ESP-SR resources during normal operation.

Instead, speech recognition remains initialized while microphone acquisition can be temporarily suspended when required. :contentReference[oaicite:0]{index=0}

---

# State Management

The component exposes two runtime state queries.

```text
Started

↓

Initialized and operational
```

and

```text
Listening

↓

Microphone acquisition enabled
```

This separation allows the application to distinguish between:

- a detector that has never been initialized;
- an initialized detector with microphone capture temporarily suspended;
- a fully operational detector.

The public interface intentionally exposes state information without revealing implementation details. :contentReference[oaicite:1]{index=1}

---

# Integration with the Audio Subsystem

The Wake Word Detector operates on audio samples supplied by the local audio infrastructure.

Conceptually:

```text
Microphones
        │
        ▼
Audio Bridge
        │
        ▼
Wake Word Detector
        │
        ▼
WakeNet
        │
        ▼
MultiNet
```

The detector consumes processed audio and generates recognition events.

It does not perform speaker playback or direct audio hardware management.

This separation allows the speech recognition pipeline to remain independent from playback functionality.

---

# Integration with the Application

The component communicates exclusively through callbacks.

```text
Application

↓

Register Callbacks

↓

Speech Recognition

↓

Callback Invocation

↓

Application Decision
```

Typical application responsibilities include:

- updating the local user interface;
- opening the command recognition window;
- generating context-change requests;
- initiating communication with the Cognitive Runtime.

The detector itself remains unaware of these higher-level operations.

---

# Component Boundaries

The Wake Word Detector intentionally owns only speech recognition.

It does not implement:

- networking;
- JSON generation;
- UDP transmission;
- authentication;
- authorization;
- semantic interpretation;
- context management.

This clear separation preserves the modular architecture of the Echo Pyramid Voice Node.

---

# Design Decisions

The implementation follows several engineering principles.

## Encapsulation

ESP-SR implementation details remain private.

The application interacts only with the public API.

---

## Event-Driven Architecture

Recognition events are propagated through callbacks rather than polling.

This reduces coupling and simplifies integration with the remainder of the firmware.

---

## Minimal Public Interface

Only the operations required by the application are exported.

The public API consists of:

- initialization;
- pause;
- resume;
- operational status;
- listening status. :contentReference[oaicite:2]{index=2}

This minimizes long-term maintenance effort while preserving implementation flexibility.

---

# Threading Model

From the public interface, the component behaves as an autonomous runtime service.

The application:

- starts the detector;
- receives asynchronous callbacks;
- controls pause and resume operations.

Internal execution details remain intentionally hidden behind the component interface.

This separation allows future implementation changes without affecting application code.

---

# Engineering Notes

The Wake Word Detector was designed to isolate all ESP-SR dependencies from the remainder of the firmware.

Benefits include:

- simplified application logic;
- improved maintainability;
- easier testing;
- clear ownership boundaries;
- stable integration contracts.

The pause/resume interface additionally supports temporary microphone suspension without requiring complete reconstruction of the speech recognition pipeline. :contentReference[oaicite:3]{index=3}

---

# Related Documentation

Additional documentation for the Echo Pyramid Voice Node is available in:

- `/ambient-physical-ai/firmware/nodes/expression-node/echo-pyramid-voice-node/README.md`
- `/ambient-physical-ai/docs/notes/ECHO_PYRAMID_WAKE_WORD_CONTEXT_INTEGRATION_TECHNICAL_NOTE.md`
- `/ambient-physical-ai/docs/notes/WAKE_WORD_CALIBRATION_AND_BUILD_RECOVERY.md`

These documents describe:

- firmware architecture;
- ESP-SR integration;
- Wake Word engineering decisions;
- calibration methodology;
- runtime interaction.

---

# Summary

The Wake Word Detector provides a compact and reusable abstraction for local speech recognition.

Its responsibilities are intentionally limited to:

```text
Wake Word Detection

↓

Offline Command Recognition

↓

Application Notification
```

By encapsulating the complete ESP-SR pipeline behind a minimal callback-based API, the component preserves the modular architecture of the Echo Pyramid Voice Node while remaining independent from networking, semantic reasoning and Cognitive Runtime implementation details.