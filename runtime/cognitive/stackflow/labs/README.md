# StackFlow Laboratory Probes

This folder contains exploratory probe scripts used during the StackChan MCP investigation.

These files are **not part of the core Cognitive Runtime execution path**.

They are preserved as engineering evidence because they were used to validate:

- MCP initialization
- MCP `tools/list`
- MCP `tools/call`
- WebSocket communication
- AX630C as an MCP Tool Provider
- StackChan MCP integration experiments

## Core Runtime

The production Cognitive Runtime is implemented in:

```text
runtime/cognitive/stackflow/
```

## Runtime Pipeline

The validated runtime architecture is:

```text
Identity Package
        │
        ▼
Context Builder
        │
        ▼
5W Context Package
        │
        ▼
Context Registry
        │
        ▼
Local Semantic Services
        │
        ▼
Semantic MCP Tool Layer
```

## Purpose

The scripts contained in this directory document the engineering investigation that led to the current Cognitive Runtime architecture.

They are intentionally preserved to improve reproducibility and to document the project's technical evolution.

They are **laboratory artifacts**, not production runtime components.