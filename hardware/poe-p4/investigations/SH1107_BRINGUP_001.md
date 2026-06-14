# SH1107 Bring-up Investigation 001

## Context

The original Mini OLED validation showed unexpected behavior:

* QR-code-like patterns
* Random bars
* Partial rendering

At first, the issue appeared to be related to hardware or controller incompatibility.

## Investigation

The following checks were performed:

### I2C Scan

Detected devices:

* 0x70 — PaHub
* 0x3C — OLED

Communication was confirmed.

### Initial Assumptions

Potential causes considered:

* Wrong controller
* Incorrect initialization sequence
* Invalid memory addressing
* Horizontal offset mismatch
* Vertical page mapping mismatch

### Experiments

#### Experiment 1

Full-screen rendering.

Result:

Display responded correctly.

#### Experiment 2

Vertical bar rendering.

Result:

Confirmed display memory access.

#### Experiment 3

Column offset variations.

Tested multiple offsets.

Result:

Display remained functional.

#### Experiment 4

Character rendering.

Displayed:

* OK
* AI
* RUN
* TES

Result:

Successful text rendering.

## Findings

The display hardware was functional.

The primary challenge was understanding the SH1107 page and column addressing model.

## Final Status

Validated.

The SH1107 display is approved for use in:

* Ambient Runtime Dashboard
* Environmental visualization
* Context feedback
* Future Ambient Physical AI demonstrations
