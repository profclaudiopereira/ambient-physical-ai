# DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_EN

## Ambient Physical AI
## AX630C + StackFlow Lab

Date: 2026-06-08

# Executive Summary

This investigation focused on understanding the Cognitive Runtime behavior running on AX630C.

After Protocol Discovery V1 identified JSON over TCP, port 10001, Setup → Inference → Streaming → Exit workflow and the first Python client, the focus moved to session, context and conversational state.

# Environment

- Ubuntu 22.04 LTS
- AX630C + LLM Mate
- ADB
- Ethernet
- Internet
- llm_sys
- llm_llm
- Qwen2.5-0.5B-prefill-20e

# Findings

## Session Lifecycle

Validated:

Setup → Dynamic work_id → Inference → Streaming → Exit

## Multi-turn Conversation

Multiple inferences inside the same session produced timeout conditions.

Result:

NOT VALIDATED

## Context Injection

Prompt:

Context: My name is Claudio.
Question: What is my name?

Response:

My name is Claudio.

Result:

VALIDATED

## Event Driven Behavior

Planned but not fully validated.

Status:

PENDING

## Conversational State

No evidence of persistent conversational memory.

Current hypothesis:

Event → Context → Inference → Response

# Runtime Stability

Stable:

Setup → Single Inference → Streaming → Exit

Unstable:

Setup → Multiple Inferences → Timeout

Recovery:

systemctl restart llm-llm
systemctl restart llm-sys

# Architectural Impact

Ambient Physical AI should assume that every event carries full context.

# Conclusion

Validated:

- Session creation
- Context injection
- Single-turn inference
- Streaming

Not validated:

- Multi-turn memory
- Persistent session state

Status:

PARTIALLY COMPLETED
