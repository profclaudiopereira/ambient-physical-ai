# STACKFLOW MCP INTEGRATION ARCHITECTURE

## Version 2.0

**Ambient Physical AI --- Engineering Architecture Document**

------------------------------------------------------------------------

## Executive Summary

This document records the engineering rationale behind the integration
between the Ambient Physical AI Cognitive Runtime and the
StackChan/XiaoZhi MCP ecosystem.

Its purpose is not only to describe the implementation, but also to
preserve the reasoning that led to the current architecture. Future
contributors should be able to understand *why* the architecture exists
in its present form before modifying it.

The document consolidates the investigation of the runtime, the
historical MCP laboratories, the semantic pipeline, and the recommended
production integration.

------------------------------------------------------------------------

## 1. Project Context

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 2. Architectural Principles

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 3. System Overview

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 4. Cognitive Runtime

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 5. Identity Package

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 6. Context Builder

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 7. Context Registry

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 8. Semantic Services

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 9. Semantic Event Generator

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 10. Semantic Dispatcher

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 11. Notification Adapters

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 12. Historical MCP Laboratories

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 13. XiaoZhi MCP Broker

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 14. Validated Experiments

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 15. Semantic Queries vs Semantic Events

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 16. Production MCP Server

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 17. Concurrency Model

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 18. Engineering Decisions

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 19. Security Considerations

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 20. Repository Organization

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 21. Development Workflow

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 22. Validation Strategy

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 23. Testing Matrix

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 24. Future Evolution

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 25. Context Push

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 26. Multiple Agents

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 27. Scalability

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 28. Lessons Learned

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 29. Recommendations

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

## 30. Final Conclusions

### Objectives

-   Preserve engineering knowledge.
-   Avoid architectural redesign.
-   Maximize reuse of validated components.
-   Keep the runtime modular.
-   Maintain local-first execution.

### Discussion

The architecture deliberately separates semantic reasoning from
transport. Semantic components never depend directly on WebSocket, UDP,
or a particular assistant implementation. Likewise, transport layers do
not implement business logic; they only expose or forward semantic
information.

This separation enables the same semantic layer to be reused by
different interfaces (StackChan, future assistants, dashboards, or
external APIs) without modifying the cognitive core.

### Engineering Notes

The Context Registry is intentionally lightweight and process-local.
During the investigation it became clear that this decision greatly
simplifies deployment, debugging and reproducibility. Rather than
introducing external synchronization mechanisms, the recommended
solution is to execute the MCP server within the same Python process as
the UDP listener, allowing both components to share the same semantic
state.

### Architectural Diagram

``` text
Identity Node
      │
      ▼
Context Builder
      │
      ▼
Context Registry
   ┌──┴────────────┐
   ▼               ▼
Semantic      Semantic
Dispatcher    Services
   │               │
   ▼               ▼
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

------------------------------------------------------------------------

# Appendix A -- Validation Status

  Component                    Status
  ---------------------------- ------------------
  Context Builder              VALIDATED
  Context Registry             VALIDATED
  Semantic Services            IMPLEMENTED
  Semantic MCP Tools           IMPLEMENTED
  MCP Transport                VALIDATED IN LAB
  StackChan Tool Consumption   VALIDATED
  Context Push                 PENDING

# Appendix B -- Recommended Roadmap

1.  Implement production `stackchan_mcp_server.py`.
2.  Reuse `semantic_mcp_tools`.
3.  Execute MCP server in the same runtime process.
4.  Validate runtime queries using real identity data.
5.  Evaluate Context Push as a future capability.

# Final Statement

The engineering investigation demonstrates that the Ambient Physical AI
project already contains the essential building blocks for a
production-grade MCP integration. The remaining work is primarily the
construction of a robust transport adapter while preserving the
validated semantic architecture.
