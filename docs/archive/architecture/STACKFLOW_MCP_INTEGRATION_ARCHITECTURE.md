# STACKFLOW MCP Integration Architecture

**Project:** Ambient Physical AI\
**Subsystem:** Cognitive Runtime / StackFlow\
**Document Type:** Engineering Discovery & Integration Architecture\
**Status:** Approved Architecture Proposal

------------------------------------------------------------------------

## Purpose

This document consolidates the engineering conclusions obtained from
reconstructing the StackFlow runtime and the historical MCP
laboratories. It explains how the validated components fit together to
form the production MCP integration without redesigning the current
architecture.

## Historical Reconstruction

Two development lines evolved independently.

### Cognitive Runtime

``` text
Identity Package
        ↓
Context Builder
        ↓
Context Registry
        ↓
Semantic Services
        ↓
Semantic MCP Tools
```

### MCP Laboratories

``` text
Handshake
        ↓
Empty Tools
        ↓
AmbientPing
        ↓
GetCurrentCountry
```

The final laboratory validated that the StackChan/XiaoZhi LLM queried an
MCP Tool and incorporated the returned information into its answer.

## Context Registry

The registry stores only the latest validated Context Package.

Characteristics:

-   In-memory
-   Lightweight
-   Single responsibility
-   No database
-   No IPC
-   No persistence

Concept:

``` text
update_context()
        ↓
_current_context
        ↓
get_current_context()
```

## Identity UDP Listener

Current runtime flow:

``` text
UDP Packet
        ↓
build_context()
        ↓
update_context()
        ↓
Context Registry
        ↓
Semantic Event Generator
        ↓
Semantic Dispatcher
```

This process owns the semantic runtime state.

## Semantic Services

Expose semantic queries:

-   get_runtime_status()
-   get_current_identity()
-   get_current_environment()
-   get_current_activity()
-   get_current_context()

These services are intentionally independent from transport
technologies.

## Semantic MCP Tools

Responsibilities:

-   list_tools()
-   call_tool()

They expose semantic information but do not implement WebSocket or MCP
transport.

## StackChanNotifier

Current implementation supports only:

-   dry-run
-   UDP notification

It is not an MCP implementation and should remain independent.

## MCP Validation

Validated:

-   WebSocket connection
-   initialize
-   notifications/initialized
-   ping
-   tools/list
-   tools/call
-   LLM consuming Tool output

The laboratory Tool:

``` text
GetCurrentCountry
```

returned:

``` text
Brazil
```

which was incorporated by the StackChan LLM into the generated response.

## Critical Discovery

The Context Registry is process-local memory.

Therefore:

``` text
Listener Process
        ↓
Context Registry A

MCP Process
        ↓
Context Registry B
```

does not work.

The MCP server must execute in the same Python process that owns the
Context Registry.

This avoids unnecessary complexity such as Redis, IPC, databases or
shared-memory synchronization.

## Recommended Production Architecture

``` text
Identity Node
        ↓ UDP
Identity UDP Listener
        ↓
Context Builder
        ↓
Context Registry
      ┌──────────────┐
      │              │
      ▼              ▼
Semantic       Semantic
Dispatcher     Services
      │              │
      ▼              ▼
Adapters   Semantic MCP Tools
                     │
                     ▼
             MCP WebSocket Server
                     │
                     ▼
              XiaoZhi MCP Broker
                     │
                     ▼
                 StackChan
```

## Recommended Implementation

Create:

``` text
runtime/cognitive/stackflow/stackchan_mcp_server.py
```

Responsibilities:

-   WebSocket transport
-   initialize
-   ping
-   tools/list
-   tools/call
-   connection management

Semantic logic remains exclusively inside:

``` text
semantic_mcp_tools.py
```

The listener remains the runtime owner while the MCP server runs as a
lightweight thread within the same process.

## Validation Status

  Component                    Status
  ---------------------------- -------------------
  Context Builder              VALIDATED
  Context Registry             VALIDATED
  Semantic Services            IMPLEMENTED
  Semantic MCP Tools           IMPLEMENTED
  MCP Transport                VALIDATED IN LAB
  StackChan Tool Consumption   VALIDATED
  Context Push                 NOT YET VALIDATED

## Final Conclusion

The project already contains all major architectural building blocks
required for production MCP integration.

The remaining work is an integration effort, not an architectural
redesign.

A production MCP transport adapter should connect the validated
WebSocket communication layer to the existing Semantic MCP Tools while
sharing the same in-process Context Registry.
