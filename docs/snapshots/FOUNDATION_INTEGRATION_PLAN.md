# FOUNDATION_INTEGRATION_PLAN.md

# Ambient Physical AI

## Milestone

M005 — Foundation Integration

## Status

Planned

---

# Purpose

This document defines the merge strategy for consolidating the current foundation branches of the Ambient Physical AI project into the main branch.

The goal is to avoid premature integration while ensuring that the project does not accumulate long-lived divergent branches.

---

# Current Branches

## main

Stable project baseline.

---

## feature/vl53l0x-integration

Current role:

Identity Console V1

Includes:

* M5Dial
* Unit Mini ToF
* NFC
* Display
* Encoder
* Buzzer
* Context Selection
* Identity Package Generation

---

## feature/ambient-runtime-node

Current role:

Ambient Runtime Node V1

Includes:

* PoE-P4
* PaHub / PCA9548A
* ENV-IV
* SH1107 OLED
* Ethernet
* DHCP
* Runtime Dashboard

---

# Merge Target

All foundation branches will eventually merge into:

```text
main
```

The main branch must represent a stable baseline and not an experimental workspace.

---

# Merge Policy

Do not merge while a branch is still in active hardware bring-up.

Merge only after the branch reaches a functional milestone.

---

# Required Merge Conditions

## Identity Console Branch

Branch:

```text
feature/vl53l0x-integration
```

Ready to merge when:

```text
Presence Detection
✓

NFC Reading
✓

Profile Recognition
✓

Context Selector
✓

Buzzer Feedback
✓

Display Feedback
✓

Identity Package Generation
✓

README / Documentation Minimum
✓

Build Validation
✓

Working Tree Clean
✓
```

---

## Ambient Runtime Branch

Branch:

```text
feature/ambient-runtime-node
```

Ready to merge when:

```text
PoE-P4 Boot Stable
✓

PaHub / PCA9548A Validated
✓

ENV-IV Readings Validated
✓

SH1107 OLED Validated
✓

Ethernet Validated
✓

DHCP Validated
✓

Runtime Dashboard Operational
✓

README / Documentation Minimum
✓

Build Validation
✓

Working Tree Clean
✓
```

---

# Recommended Merge Order

Recommended order:

```text
1. feature/vl53l0x-integration
2. feature/ambient-runtime-node
```

Rationale:

The project narrative follows:

```text
Presence
↓
Identity
↓
Cognition
↓
Ambient Runtime
```

Therefore, Identity Console should become part of the baseline before Ambient Runtime integration.

---

# Integration Procedure

## Step 1 — Update Main

```bash
git checkout main
git pull origin main
```

---

## Step 2 — Merge Identity Console

```bash
git merge feature/vl53l0x-integration
```

If conflicts occur, resolve them carefully.

Validate:

```bash
git status
```

---

## Step 3 — Merge Ambient Runtime

```bash
git merge feature/ambient-runtime-node
```

If conflicts occur, resolve them carefully.

Validate:

```bash
git status
```

---

## Step 4 — Final Validation

After both merges:

```bash
git log --oneline --graph --decorate --all -20
```

Validate:

* Repository structure
* Documentation
* Firmware folders
* Snapshots
* Branch history

---

# Conflict Risk Areas

Possible conflict areas:

```text
README.md
docs/snapshots/
docs/architecture/
docs/notes/
firmware/nodes/
hardware/
```

Expected low-risk areas:

```text
firmware/nodes/identity-console/
firmware/nodes/ambient-runtime-node/
firmware/nodes/expression-node/
```

---

# Do Not Merge Yet

Do not merge while:

* Identity Console does not generate Identity Package
* Ambient Runtime Dashboard is not operational
* Builds are not validated
* Documentation is incomplete
* Working tree is not clean

---

# Future Branches

Expected future branches may include:

```text
feature/cognitive-runtime-node
feature/voice-node
feature/expression-node
feature/stackflow-integration
```

This document should be revisited whenever a new major branch is introduced.

---

# MASTER Decision

Merge now:

```text
NO
```

Prepare merge plan now:

```text
YES
```

Execute merge only after both foundation branches reach functional stability.

---

# Expected Outcome

After M005, the main branch should contain the first stable foundation of Ambient Physical AI:

```text
Presence / Identity Foundation
+
Ambient Runtime Foundation
+
Architecture Documentation
+
Runtime State Language
+
Expression Layer Concepts
```

This baseline will become the official starting point for the next integration phase of the Ambient Physical AI ecosystem.

---

# Integration Vision

```text
Presence
↓
Identity
↓
Cognition
↓
Ambient Runtime
↓
Expression
```

M005 establishes the first unified foundation capable of supporting future Cognitive Runtime, StackFlow integration and multimodal interaction services.
