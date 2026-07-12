# Engineering Notes

## Echo Pyramid + AtomS3R

---

# Purpose

This document records engineering observations made during the hardware characterization laboratory.

The purpose is to preserve discoveries that may influence the future ESP-IDF implementation without polluting the validated examples.

---

# Hardware Baseline

Validated hardware

 Echo Pyramid
 AtomS3R

Official validation firmware

 Echo Pyramid Xiaozhi Voice Assistant
 Version 2.2.6

The official firmware was used exclusively as a hardware validation baseline.

---

# Arduino Characterization

The official Arduino libraries proved sufficient to validate

 RGB control
 Audio codec initialization
 Microphone capture
 Speaker playback
 Touch-triggered interaction

---

# Audio Observations

The official RecordPlay example initializes

```cpp
ep.codec().setVolume(50);
```

Observed during validation

 Volume  Result                  
 ------  ----------------------- 
 50      Playback very low       
 70      Best subjective quality 
 100     Audible but distorted   

Recommendation

Use 70 as the initial validation volume during future ESP-IDF bring-up.

---

# Touch

Touch successfully triggered the official RecordPlay example.

However, the low-level touch interface remains partially characterized.

Further investigation is still required before designing the final Expression Layer input subsystem.

---

# AudioStage

The AudioStage example introduces several architectural concepts that may be reused in the future ESP-IDF implementation

 FreeRTOS audio task
 Dedicated audio partition
 SPIFFS integration
 Beat detection
 RGB synchronization

The example was intentionally not validated during this laboratory because it exceeds the current bring-up scope.

---

# Engineering Decision

The Arduino phase is considered complete.

Future development shall continue exclusively in ESP-IDF.

Arduino will remain only as a hardware characterization reference.

---

# Next Laboratory

Expression Layer ESP-IDF Bring-up

Goals

 ESP-IDF initialization
 Echo Pyramid initialization
 RGB control
 Wi-Fi connectivity
 Semantic Event reception
 Cognitive Runtime integration
