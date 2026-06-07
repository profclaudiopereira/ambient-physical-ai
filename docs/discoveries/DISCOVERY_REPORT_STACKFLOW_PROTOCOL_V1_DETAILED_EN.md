# DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1
## Ambient Physical AI
### AX630C + StackFlow Protocol Discovery

**Date:** 2026-06-08

---

# Executive Summary

This report documents the successful reverse-engineering and validation of the StackFlow runtime communication protocol running on the AX630C Module LLM Kit.

The investigation proved that the AX630C exposes a TCP-based API that allows direct interaction with the local Qwen2.5 model without requiring official M5Stack applications.

This discovery transforms the AX630C from a development kit into a programmable Cognitive Runtime Node suitable for Ambient Physical AI.

---

# Mission Objective

Understand how the local AI runtime can be accessed programmatically.

Principle:

Explore First. Integrate Later.

---

# Validated Platform

Hardware:
- AX630C Module
- LLM Mate
- Ethernet via LLM Mate
- Flat FPC connection

System:
- Ubuntu 22.04 LTS
- ADB access
- Root shell access
- Internet connectivity

Runtime Services:
- llm_sys
- llm_llm
- llm_asr
- llm_tts
- llm_vlm
- llm_yolo
- llm_kws

Model:
- Qwen2.5-0.5B-prefill-20e

---

# Discovery D01 — Protocol Identification

Listening service discovered:

Port: 10001/TCP

Owner:

llm_sys

Configuration:

{
  "config_enable_tcp": 1
}

Protocol:

JSON over TCP

Message delimiter:

newline (\n)

---

# Discovery D02 — Runtime Ping

Request:

{
  "request_id":"safe_ping_001",
  "work_id":"sys",
  "action":"ping"
}

Response:

{
  "error":{"code":0}
}

Result:

Communication with StackFlow runtime validated.

---

# Discovery D03 — Session Creation

The runtime does not allow direct inference.

A session must first be created.

Request:

work_id = "llm"
action = "setup"

Response:

work_id = "llm.1000"

Important finding:

Session identifiers are dynamically allocated.

---

# Discovery D04 — Local Inference

Prompt:

Hello

Response:

Hello! How can I assist you today?

Streaming behavior observed.

Example stream:

Hello! How
can I assist
you today?

Result:

Local inference fully validated.

---

# Discovery D05 — Python Client

A standalone Python socket client was created.

Capabilities:

- Connect
- Setup session
- Send prompt
- Receive stream
- Close session

Location:

/root/client.py

---

# Message Structure

Common fields:

- request_id
- work_id
- action
- object
- data

Lifecycle:

Ping -> Setup -> Inference -> Stream -> Exit

---

# Architecture

TCP 10001
    |
    v
 llm_sys
    |
    v
 llm_llm
    |
    v
 Qwen2.5-0.5B

---

# Engineering Impact

The AX630C behaves as a local AI server.

Potential future clients:

- CoreS3 Lite
- StackChan
- PoE-P4
- Voice Node
- Ambient Node

---

# Conclusions

Validated:
- TCP interface
- JSON protocol
- Session management
- Streaming inference
- Python client

Status:

DISCOVERY COMPLETED
