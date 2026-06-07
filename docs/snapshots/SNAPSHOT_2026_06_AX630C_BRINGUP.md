# AX630C Bringup Milestone

Date: June 2026

Status: Completed

---

## Objective

Validate the Cognitive Runtime Node in real hardware before starting integration activities.

---

## Hardware Validation

Successfully validated:

* AX630C Module
* LLM Mate
* FPC Interconnection
* Ethernet Connectivity
* USB Connectivity

The previously suspected FPC connector issue was investigated and ultimately did not prevent successful system operation.

---

## Operating System Validation

Validated:

```text
Ubuntu 22.04 LTS
```

Capabilities confirmed:

* Root access
* ADB access
* SSH access
* Network connectivity

The AX630C platform is now confirmed as an operational Linux-based edge computing environment.

---

## Runtime Discovery

Active services identified:

* ASR (Speech-to-Text)
* KWS (Wake Word)
* LLM
* TTS
* VLM
* YOLO

This confirms that the platform provides a multimodal AI runtime rather than a standalone language model service.

---

## Local LLM Validation

Model identified:

```text
Qwen2.5-0.5B-prefill-20e
```

Inference activity was observed through runtime logs.

Conclusion:

```text
Local LLM inference
VALIDATED
```

---

## Runtime Architecture Discovery

Observed service chain:

```text
llm_sys
   ↓
llm_llm
   ↓
Qwen2.5-0.5B
```

This represents the first concrete evidence of the internal runtime organization.

---

## Integration Endpoint Discovery

Observed endpoint:

```text
TCP Port 10001
```

The service `llm_sys` listens on TCP port 10001 and appears to be a central integration point for the local AI runtime.

Current assessment:

```text
High-value discovery
```

This endpoint may become the primary integration interface between StackFlow and external nodes.

---

## Architectural Impact

Prior state:

```text
AX630C
↓
Black Box
```

Current state:

```text
AX630C
↓
Ubuntu 22.04
↓
AI Services
↓
TCP Endpoint
↓
Integration Candidate
```

The Cognitive Runtime Node is no longer considered a black box.

---

## Updated Project Status

```text
Presence Layer
Validated

Identity Layer
Architecturally Defined

Cognitive Runtime Node
Validated

Ambient Runtime Node
Pending

Embodiment Layer (StackChan)
Validated

Integration Layer
In Progress
```

---

## Next Phase

StackFlow Protocol Discovery

Objectives:

* Identify protocol used on TCP 10001.
* Discover available APIs.
* Build first local client.
* Validate runtime interaction.
* Prepare future integration with:

  * StackChan
  * PoE-P4
  * M5Dial
  * Ambient Physical AI nodes

---

## Conclusion

The AX630C Bringup phase has been completed successfully.

The project now possesses a validated Cognitive Runtime Node running on real hardware with local AI inference capabilities.

This milestone significantly reduces technical risk and marks the transition from platform validation to runtime integration.
