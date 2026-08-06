# TECHNICAL_NOTE_005_ECHO_PYRAMID_WAKE_WORD_BUILD_AND_CONFIGURATION

## Ambient Physical AI

### Objective

Document the validated build and configuration procedure required to
successfully compile, flash and validate the Echo Pyramid Voice Node
with ESP-SR Wake Word support.

This note records the engineering knowledge acquired during
troubleshooting and should be used as the reference for future
environment recovery after SDK updates, clean installations or
workstation migrations.

------------------------------------------------------------------------

## Hardware Baseline

-   Echo Pyramid
-   M5Stack AtomS3R
-   ESP-IDF v5.4.2
-   ESP-SR
-   8 MB Flash
-   8 MB PSRAM

------------------------------------------------------------------------

## Validated menuconfig Configuration

### Serial Flasher Config

-   Flash Size: **8 MB**

### Partition Table

-   Custom Partition Table
-   File: `partitions.csv`

### ESP-SR

#### WakeNet

-   WakeNet9
-   Hi ESP

> Validated configuration. Do not use WakeNet9s unless intentionally
> revalidated.

#### MultiNet

-   English MultiNet6 Quantized

### PSRAM

-   Enabled
-   Octal Mode
-   80 MHz
-   Use malloc() enabled

------------------------------------------------------------------------

## Clean Build Procedure

``` text
rmdir /s /q build
idf.py reconfigure
idf.py build
```

Windows terminal:

``` text
chcp 65001
set PYTHONUTF8=1
set PYTHONIOENCODING=utf-8
```

------------------------------------------------------------------------

## Expected Boot Indicators

-   SPI Flash Size: 8MB
-   PSRAM detected
-   Successfully load srmodels
-   WakeNet model: `wn9_hiesp`
-   Speech detector started

------------------------------------------------------------------------

## Common Recovery Scenarios

### Flash configured as 2 MB

-   Partition table does not fit in flash.

### Missing model partition

-   `MODEL_LOADER: Can not find model`

### Incorrect WakeNet

-   `wakenet model not found`

### Missing English MultiNet

-   `English MultiNet model was not found`

------------------------------------------------------------------------

## Engineering Lessons Learned

-   Flash must be configured as 8 MB.
-   Use the custom `partitions.csv`.
-   PSRAM must be enabled.
-   Validated WakeNet: **WN9 Hi ESP**.
-   Validated MultiNet: **English MultiNet6 Quantized**.
-   Execute `idf.py reconfigure` after configuration changes.
-   Change one menuconfig option at a time during troubleshooting.

------------------------------------------------------------------------

## Recovery Checklist

-   [ ] Flash = 8 MB
-   [ ] Custom `partitions.csv`
-   [ ] PSRAM enabled
-   [ ] WakeNet9 → Hi ESP
-   [ ] English MultiNet6 Quantized
-   [ ] `idf.py reconfigure`
-   [ ] `idf.py build`
-   [ ] Flash firmware
-   [ ] Validate boot
-   [ ] Test "Hi ESP"
-   [ ] Test English commands
-   [ ] Test NFC greeting

------------------------------------------------------------------------

## Engineering Note

Small `menuconfig` changes may modify ESP-SR behaviour without any
source-code changes. This document should be considered the reference
for restoring the validated build environment.
