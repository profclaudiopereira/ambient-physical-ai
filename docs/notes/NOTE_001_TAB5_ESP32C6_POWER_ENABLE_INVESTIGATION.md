# NOTE_001 — TAB5 ESP32-C6 POWER ENABLE INVESTIGATION

## Ambient Physical AI

### Engineering Investigation of the ESP32-C6 Power Enable Sequence on the M5Stack Tab5

---

# Abstract

During the implementation of the Ambient Runtime Node for the M5Stack Tab5, the wireless subsystem remained unavailable despite the successful integration of Espressif's ESP-Hosted software stack.

Initial analysis suggested a software integration problem involving the Wi-Fi driver, the ESP-Hosted middleware or the communication between the ESP32-P4 and the onboard ESP32-C6 wireless coprocessor.

A detailed engineering investigation demonstrated that the root cause was not software related.

Instead, the ESP32-C6 remained electrically powered off because the board-specific power enable sequence had not yet been executed.

This technical note documents the engineering investigation, the discovery of the PI4IOE2-controlled `WLAN_PWR_EN` signal, the validation process and the initialization sequence adopted by the Ambient Runtime implementation.

The objective of this document is to preserve the engineering knowledge that led to the current implementation and to facilitate future maintenance and platform bring-up activities.

---

# Abstract

...

This technical note documents the engineering investigation, the discovery of the PI4IOE2-controlled `WLAN_PWR_EN` signal, the validation process and the initialization sequence adopted by the Ambient Runtime implementation.

The objective of this document is to preserve the engineering knowledge that led to the current implementation and to facilitate future maintenance and platform bring-up activities.

---

# Engineering Outcome

**Validated Discovery**

The ESP32-C6 wireless subsystem on the M5Stack Tab5 requires the
PI4IOE2-controlled `WLAN_PWR_EN` signal to be asserted before the
ESP-Hosted initialization begins.

This hardware requirement has been incorporated into the official
Ambient Runtime platform initialization and is now considered part
of the validated engineering baseline.

---

Engineering Investigation

ESP Hosted Integrated
        │
        ▼
Wi-Fi Unavailable
        │
        ▼
Software Investigation
        │
        ▼
Official BSP Analysis
        │
        ▼
PI4IOE2 Discovery
        │
        ▼
WLAN_PWR_EN Identified
        │
        ▼
ESP32-C6 Powered
        │
        ▼
ESP Hosted Operational
        │
        ▼
Wi-Fi Available

---

# Background

The Ambient Runtime Node is implemented on the M5Stack Tab5 platform.

Unlike conventional ESP32-based development boards, the Tab5 combines two independent processors with distinct responsibilities.

```text
ESP32-P4
│
├── Application
├── Runtime
├── Display
├── Sensors
└── System Control

↓

ESP Hosted

↓

ESP32-C6

↓

Wi-Fi PHY / MAC
```

The ESP32-P4 executes the Ambient Runtime firmware while the ESP32-C6 operates as a dedicated wireless coprocessor responsible for IEEE 802.11 communication.

Application software does not communicate directly with the ESP32-C6 firmware.

Instead, communication occurs through Espressif's ESP-Hosted architecture. :contentReference[oaicite:0]{index=0}

---

# Engineering Context

At the time of this investigation, the Ambient Runtime hardware platform had already reached a stable state.

The following subsystems had been successfully validated:

- LCD initialization;
- MIPI-DSI display driver;
- Runtime Console;
- Internal I²C infrastructure;
- PORT A I²C infrastructure;
- PaHub integration;
- ENV-IV environmental sensor;
- Mini OLED;
- DLight sensor.

The remaining major subsystem pending validation was wireless communication through the onboard ESP32-C6. :contentReference[oaicite:1]{index=1}

---

# Problem Statement

Although the ESP-Hosted software stack had been correctly integrated into the firmware, the Wi-Fi subsystem consistently failed to become operational.

From the application perspective:

- the firmware compiled successfully;
- the runtime initialized correctly;
- the display subsystem operated normally;
- environmental sensors were functional;
- no obvious software errors were present.

Nevertheless, the wireless subsystem never became available.

The observed behavior strongly suggested that the problem was located below the application layer.

---

# Initial Symptoms

The engineering investigation began with the following observations.

## Successful Firmware Initialization

The Ambient Runtime executed normally.

Platform initialization, sensor acquisition and display rendering all completed successfully.

No indication suggested a failure in the main application.

---

## ESP-Hosted Integration Appeared Correct

The ESP-Hosted middleware had already been integrated into the project.

From a software perspective, the communication architecture appeared consistent with Espressif's reference implementation.

No evidence indicated incorrect API usage or middleware configuration.

---

## Wi-Fi Remained Unavailable

Despite the successful software integration, the wireless subsystem never became operational.

The ESP32-P4 could not establish communication through the expected wireless interface.

This behavior suggested one of two possibilities:

- a low-level software integration failure;
- a hardware initialization sequence that had not yet been reproduced by the Ambient Runtime.

---

# Initial Engineering Hypotheses

Several hypotheses were considered during the investigation.

```text
Application bug

↓

ESP Hosted configuration

↓

Driver initialization

↓

SDIO communication

↓

Board Support Package differences

↓

Hardware power sequencing
```

At the beginning of the investigation, software-related explanations appeared to be the most likely.

Only after progressively eliminating these possibilities did attention shift toward the board initialization process itself.

---

# Investigation Objective

The objective of the investigation became clear:

> Determine why the onboard ESP32-C6 remained unavailable despite the successful integration of the ESP-Hosted software stack.

Rather than modifying application logic, the investigation focused on understanding the hardware initialization performed by the official M5Stack Board Support Package and identifying any platform-specific requirements that had not yet been reproduced by the Ambient Runtime implementation.

The remainder of this document describes the engineering process that led to the discovery of the missing power-enable sequence and its incorporation into the Ambient Runtime platform initialization. :contentReference[oaicite:2]{index=2}

---

# Investigation

After the initial software hypotheses proved insufficient to explain the observed behavior, the investigation shifted toward the hardware initialization performed by the official M5Stack platform.

The objective was no longer to debug the ESP-Hosted software stack itself, but rather to determine whether the official Board Support Package (BSP) performed any hardware initialization that had not yet been reproduced by the Ambient Runtime.

This marked a significant change in the investigation strategy.

Instead of asking:

> *"Why is Wi-Fi failing?"*

the engineering question became:

> *"What hardware initialization does the official platform perform before Wi-Fi becomes available?"*

---

# Analysis of the Official Platform Initialization

The investigation focused on understanding the startup sequence implemented by the official M5Stack software.

Special attention was given to board-level initialization occurring before any network services were started.

This analysis revealed that the Tab5 initialization involved considerably more than simply configuring the ESP32-P4 peripherals.

Several board-control devices were also initialized very early during startup.

Among these devices was a second I²C GPIO expander.

---

# Discovery of PI4IOE2

The engineering investigation identified the presence of two independent PI4IOE I/O expanders connected to the internal System I²C bus.

Their responsibilities were distinct.

```text
System I²C

├── PI4IOE1
│       Board peripherals
│       LCD
│       Touch
│
└── PI4IOE2
        Platform power control
```

While PI4IOE1 was already understood as part of the display subsystem, PI4IOE2 proved to be responsible for several power-management signals within the Tab5 hardware architecture. :contentReference[oaicite:0]{index=0}

---

# Identification of WLAN_PWR_EN

Further analysis demonstrated that one output of PI4IOE2 controlled the electrical power supplied to the onboard ESP32-C6.

```text
ESP32-P4

↓

System I²C

↓

PI4IOE2 (0x44)

↓

GPIO Output P0

↓

WLAN_PWR_EN

↓

ESP32-C6 Power Rail
```

Unless this output was explicitly asserted, the ESP32-C6 remained powered off.

Consequently, the failure of the Wi-Fi subsystem was not caused by incorrect software integration.

The wireless coprocessor simply had not yet been powered. :contentReference[oaicite:1]{index=1}

---

# Engineering Interpretation

This discovery fundamentally changed the understanding of the problem.

The ESP32-C6 was not failing during initialization.

Instead, it never reached a state in which software initialization could even begin.

The software stack was waiting for hardware that was still electrically inactive.

This explained why:

- ESP-Hosted configuration appeared correct;
- application startup completed normally;
- display and sensor subsystems functioned correctly;
- Wi-Fi never became available.

The apparent software problem was, in reality, a hardware power-sequencing requirement.

---

# Validation

Once the relationship between PI4IOE2 and the ESP32-C6 power rail had been identified, the next step was to reproduce the initialization sequence within the Ambient Runtime.

The implementation performed the following operations:

1. Initialize the internal I²C bus.
2. Configure the PI4IOE2 device.
3. Configure its GPIO directions.
4. Write the required output register.
5. Enable `WLAN_PWR_EN`.
6. Verify the resulting output state.
7. Wait for hardware stabilization.
8. Continue platform initialization.

After reproducing this sequence, the ESP32-C6 became operational and the ESP-Hosted software stack initialized successfully.

This experimentally confirmed that the missing hardware initialization sequence had been the root cause of the observed behavior. :contentReference[oaicite:2]{index=2}

---

# Final Initialization Sequence

The validated Ambient Runtime startup sequence is summarized below.

```text
Boot

↓

Internal I²C Initialization

↓

PI4IOE2 Initialization

↓

Enable WLAN_PWR_EN

↓

Verify Output State

↓

200 ms Stabilization

↓

ESP32-C6 Powered

↓

ESP Hosted Initialization

↓

Wi-Fi Initialization

↓

Ambient Runtime Services
```

Each stage depends on the successful completion of the previous one.

This sequence has become part of the official Ambient Runtime platform initialization.

---

# Architectural Impact

The investigation demonstrated that the ESP32-C6 cannot be treated as an always-powered wireless subsystem.

Instead, it must be considered another managed hardware resource under the control of the platform layer.

This finding reinforced one of the architectural principles adopted by the Ambient Runtime:

```text
Application

↓

Platform Services

↓

Board Control

↓

Peripheral Power

↓

Peripheral Initialization

↓

Runtime Services
```

The responsibility for enabling the ESP32-C6 therefore belongs exclusively to the `tab5_platform` component.

Higher-level modules, including `ambient_network`, remain completely independent from board-specific power sequencing.

---

# Implementation Impact

As a direct consequence of this investigation, the Ambient Runtime platform implementation now includes:

- initialization of PI4IOE2;
- explicit activation of `WLAN_PWR_EN`;
- verification of the configured output state;
- stabilization delay before network initialization;
- encapsulation of the complete procedure inside `tab5_platform`.

No changes were required in higher-level networking components.

The engineering solution was therefore localized entirely within the hardware abstraction layer, preserving the modular architecture of the Ambient Runtime. :contentReference[oaicite:3]{index=3}

---

# Engineering Decision

The engineering investigation demonstrated that the inability to establish Wi-Fi communication was **not** caused by the ESP-Hosted software stack.

Instead, the root cause was the absence of a board-specific hardware initialization sequence required to power the onboard ESP32-C6.

As a consequence, the following engineering decision was adopted.

> **The power management of the ESP32-C6 is a platform responsibility and shall remain exclusively inside the `tab5_platform` component.**

No higher-level runtime component shall directly manipulate board-control registers or hardware power sequencing.

This decision preserves the architectural separation between:

- hardware initialization;
- network services;
- application logic.

---

# Lessons Learned

The investigation produced several engineering lessons that should remain part of the project's long-term documentation.

## Hardware Before Software

When integrating complex embedded platforms, software failures may originate from missing hardware initialization.

Successful compilation and correct middleware integration do not guarantee that all required hardware resources are operational.

---

## Vendor BSPs Contain Engineering Knowledge

Official Board Support Packages often contain hardware initialization sequences that are not immediately obvious from public APIs.

Careful analysis of vendor implementations can reveal undocumented board-specific requirements.

---

## Board-Control Devices Matter

GPIO expanders should not be viewed as simple peripheral devices.

On the Tab5, they participate directly in:

- power sequencing;
- peripheral reset;
- subsystem activation.

Their initialization is therefore a mandatory part of platform bring-up rather than an optional hardware feature.

---

## Preserve Hardware Knowledge

Discoveries obtained through engineering investigation should not remain only inside the source code.

They should also be documented so that future developers understand:

- why the implementation exists;
- why its execution order matters;
- which hardware assumptions are being preserved.

That objective motivated the creation of this technical note.

---

# Current Implementation

The validated Ambient Runtime implementation now performs the following startup sequence.

```text
Boot

↓

Initialize Internal I²C

↓

Initialize PI4IOE2

↓

Enable WLAN_PWR_EN

↓

Verify Output Register

↓

Wait 200 ms

↓

ESP32-C6 Powered

↓

Initialize ESP Hosted

↓

Initialize Ambient Network

↓

Wi-Fi Available
```

This sequence has become part of the official hardware initialization baseline for the Ambient Runtime.

Future platform modifications should preserve this ordering unless a new hardware revision explicitly requires otherwise.

---

# Relationship with the Ambient Runtime Architecture

This investigation influenced the architectural organization of the Ambient Runtime.

The resulting separation of responsibilities is illustrated below.

```text
Application

↓

ambient_network

↓

tab5_platform

↓

PI4IOE2

↓

WLAN_PWR_EN

↓

ESP32-C6

↓

Wi-Fi Network
```

In this architecture:

- `ambient_network` assumes that the wireless subsystem is already available;
- `tab5_platform` is responsible for preparing the hardware;
- board-specific details remain encapsulated within the platform layer.

This separation keeps networking independent from hardware implementation details and improves long-term maintainability.

---

# Maintenance Considerations

Future modifications involving:

- ESP32-C6 firmware;
- ESP-Hosted integration;
- board initialization;
- Tab5 hardware revisions;
- wireless subsystem bring-up;

should verify that the validated power-enable sequence remains intact.

If future hardware revisions modify:

- the GPIO expander;
- the I²C address;
- the `WLAN_PWR_EN` signal;
- stabilization timing;

the corresponding changes should be implemented exclusively within the `tab5_platform` component.

---

# References

This investigation is based on the validated Ambient Runtime implementation and its architectural documentation.

Primary references include:

- Ambient Runtime platform implementation (`tab5_platform`);
- Ambient Runtime architectural documentation;
- Official ESP-IDF LCD and platform infrastructure;
- Espressif ESP-Hosted architecture;
- M5Stack Tab5 Board Support Package analysis. :contentReference[oaicite:0]{index=0}

Related project documentation:

```text
firmware/nodes/ambient-runtime-node/README.md

firmware/nodes/ambient-runtime-node/components/tab5_platform/README.md

docs/notes/NOTE_002_SH1107_DISPLAY_GEOMETRY_VALIDATION.md
```

---

# Conclusion

The investigation documented in this technical note established that the Wi-Fi initialization problem encountered during the development of the Ambient Runtime was not caused by software integration issues.

Instead, the root cause was the omission of a mandatory hardware power-enable sequence for the onboard ESP32-C6 wireless coprocessor.

By identifying the role of the PI4IOE2 GPIO expander and the `WLAN_PWR_EN` control signal, the Ambient Runtime adopted a platform initialization sequence consistent with the hardware architecture of the M5Stack Tab5.

The resulting implementation encapsulates all board-specific power management within the `tab5_platform` component, allowing higher-level networking and application modules to remain independent from hardware details.

Beyond solving the immediate problem, this investigation reinforced an important engineering principle adopted throughout the Ambient Physical AI project:

> **Platform-specific hardware knowledge belongs in the hardware abstraction layer, not in application code.**

Documenting this investigation preserves the rationale behind the implementation, facilitates future maintenance and provides a reproducible engineering reference for anyone bringing up the Ambient Runtime on the M5Stack Tab5.

---

