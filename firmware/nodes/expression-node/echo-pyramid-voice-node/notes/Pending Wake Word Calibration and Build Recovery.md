# Appendix A — Pending Wake Word Calibration and Build Recovery

## Status at Session Closure

The Echo Pyramid voice journey is functionally operational. WakeNet detects the `Hi ESP` Wake Word, MultiNet recognizes canonical environment commands, and the firmware sends valid `context_change_request` packets to the AX630C.

However, Wake Word detection sensitivity is not yet suitable for final demonstration conditions. During informal testing, recognition was intermittent and required multiple repetitions.

A controlled calibration session must therefore be completed before declaring the acoustic interface fully stabilized.

This pending calibration does not invalidate the validated system integration. It is a refinement of recognition reliability and accessibility.

---

## Configuration Change Awaiting Validation

The AFE operating mode was changed from:

```c
AFE_MODE_LOW_COST
```

to:

```c
AFE_MODE_HIGH_PERF
```

The purpose of this experiment is to determine whether the higher-performance Audio Front End configuration improves Wake Word recognition reliability.

This configuration was not successfully flashed and tested before the session ended because the project build configuration temporarily reverted to incompatible flash and partition settings.

Do not assume that `AFE_MODE_HIGH_PERF` is better until a controlled comparison has been completed.

---

## Flash and Partition Configuration Required

The physical Echo Pyramid controller uses 8 MB of flash.

The following settings must be simultaneously active in `sdkconfig`:

```text
CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="8MB"

CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
CONFIG_PARTITION_TABLE_FILENAME="partitions.csv"
```

The custom partition table must remain:

```csv
# ESP-IDF Partition Table
# Name,     Type, SubType, Offset,   Size,    Flags
nvs,        data, nvs,     0x9000,   24K,
phy_init,   data, phy,     0xf000,   4K,
factory,    app,  factory, 0x10000,  1536K,
model,      data, spiffs,  ,         6000K,
```

The two settings must be verified in the same `menuconfig` session:

```text
Serial flasher config
  Flash size
    8 MB

Partition Table
  Partition Table
    Custom partition table CSV

  Custom partition CSV file
    partitions.csv
```

After saving, verify:

```cmd
findstr /I "CONFIG_ESPTOOLPY_FLASHSIZE" sdkconfig
findstr /I "CONFIG_PARTITION_TABLE" sdkconfig
```

Do not continue if the generated configuration reports:

```text
Flash size: 2 MB
```

or:

```text
CONFIG_PARTITION_TABLE_SINGLE_APP=y
```

---

## Clean Rebuild Procedure

Before rebuilding ESP-SR firmware on Windows CMD:

```cmd
set PYTHONUTF8=1
set PYTHONIOENCODING=utf-8
chcp 65001
```

Then run:

```cmd
idf.py fullclean
idf.py reconfigure
idf.py build
```

The build output must confirm that:

* the target is ESP32-S3;
* the configured flash size is 8 MB;
* `partitions.csv` is used;
* the factory application partition is 1536 KB;
* ESP-SR models are packaged;
* WakeNet `wn9_hiesp` is loaded;
* English MultiNet6 quantized is loaded.

The following output is not acceptable:

```text
ESP-SR Models Report
No speech models loaded.
```

If this appears, reopen `menuconfig` and verify the WakeNet and MultiNet model selections before flashing.

---

## Controlled Wake Word Test

Compare `AFE_MODE_LOW_COST` and `AFE_MODE_HIGH_PERF` under the same conditions.

For each mode:

```text
Number of attempts: 20
Wake phrase: Hi ESP
Same speaker
Same distance
Same direction
Same room
No TTS playback
No intentional background noise
```

Record:

```text
Detection count
False activations
Average comfortable speaking distance
Need to raise voice
Recognition after repeated attempts
```

Example:

```text
LOW_COST:  4/20
HIGH_PERF: 15/20
```

Only retain `AFE_MODE_HIGH_PERF` if it produces a clear improvement without introducing memory instability, audio failures or unacceptable command-recognition regressions.

---

## Parameters to Review After the AFE Comparison

Change only one parameter at a time.

Recommended investigation order:

1. Confirm the real AFE mode used at runtime.
2. Confirm that WakeNet and MultiNet models are loaded.
3. Inspect `vad_init`.
4. Inspect microphone gain configuration.
5. Measure or log captured PCM amplitude.
6. Confirm microphone/reference channel ordering.
7. Temporarily compare AEC enabled and disabled.
8. Review WakeNet threshold configuration.
9. Review command-window duration.
10. Evaluate recognition with TTS playback and ambient noise.

Avoid changing microphone gain, AEC, VAD and WakeNet thresholds simultaneously, because the result would not identify which parameter caused the improvement or regression.

---

## Audio Channel and AEC Validation

The firmware currently uses an `MR` audio channel layout:

```text
M = microphone signal
R = playback reference signal
```

This is appropriate only if the reference channel contains the correct playback-reference samples.

An incorrect, excessively strong or misaligned reference signal can cause Acoustic Echo Cancellation to suppress part of the user's voice.

During the next review, confirm:

* channel order;
* sample interleaving;
* sample rate;
* frame size;
* reference-channel origin;
* synchronization between microphone and playback reference.

AEC should not be disabled permanently without validating TTS playback behavior, because the Echo Pyramid must avoid recognizing its own reproduced audio.

---

## Accessibility Requirement

Wake Word calibration must consider more than an ideal speaker in a silent room.

The final evaluation should include:

* softer speech;
* different voice pitches;
* different distances;
* different speaking speeds;
* mild background noise;
* users with articulation differences;
* users unable to project their voice strongly.

The target should be a comfortable interaction, not one that requires shouting or repeated attempts.

---

## Closure Condition

Wake Word calibration may be considered complete only when:

```text
The firmware builds reproducibly
The correct ESP-SR models are packaged
The device flashes reliably
Wake Word sensitivity is acceptable
False activations remain controlled
MultiNet context recognition remains functional
TTS playback remains functional
The complete NFC + voice + context journey is revalidated
```

Until then, record the status as:

```text
Core voice integration: VALIDATED
Wake Word sensitivity: CALIBRATION PENDING
```
