# Semantic Event Architecture — Version 1.0

## Ambient Physical AI

**Status:** Status: APPROVED FOR IMPLEMENTATION
**Date:** 2026-07-19

------------------------------------------------------------------------

# Purpose

This document freezes the V1 architecture for introducing Semantic
Events into the Cognitive Runtime. It records the engineering decisions
reached after auditing both the architecture documents and the current
implementation.

The objective is to preserve the validated runtime while completing the
missing semantic pipeline with the minimum architectural impact.

------------------------------------------------------------------------

# Scope

This document covers only the AX630C Cognitive Runtime and the
production of Semantic Events.

It intentionally does **not** redesign:

-   Identity Node
-   Tab5 Ambient Runtime
-   RGB Nodes
-   Voice Pyramid
-   Wearables
-   StackChan firmware

Those components become consumers of Semantic Events.

------------------------------------------------------------------------

# Audit Performed

## Architecture reviewed

-   Semantic Event Model
-   Cognitive Runtime v1.1
-   Ambient Runtime Semantic Event Model
-   Identity Package Specification v1.1

## Runtime reviewed

-   identity_udp_listener.py
-   context_builder.py
-   context_registry.py
-   semantic_services.py
-   semantic_mcp_tools.py
-   stackchan_notifier.py

------------------------------------------------------------------------

# Current Runtime

``` text
Identity Package
        ↓
identity_udp_listener.py
        ↓
context_builder.py
        ↓
5W Context Package
        ↓
context_registry.py
      ├────────► semantic_services.py
      │                 ↓
      │         semantic_mcp_tools.py
      │
      └────────► stackchan_notifier.py
```

------------------------------------------------------------------------

# Main Discoveries

1.  The 5W Context Package is already the central semantic object.
2.  Context Registry correctly stores only the current validated
    context.
3.  Semantic Services expose runtime state without making decisions.
4.  MCP layer is already properly separated.
5.  StackChanNotifier is an output adapter, not a cognitive component.
6.  The pipeline currently ends immediately after the Context Registry.
7.  The missing block is semantic decision generation.

------------------------------------------------------------------------

# Architectural Gap

Current:

``` text
Context Registry
        ↓
StackChan
```

Required:

``` text
Context Registry
        ↓
Semantic Event Generator
        ↓
Semantic Event
        ↓
Semantic Dispatcher
        ↓
Consumers
```

------------------------------------------------------------------------

# Approved Architecture

``` text
Identity Package
        ↓
Context Builder
        ↓
Context Registry
        ↓
Semantic Event Generator
        ↓
Semantic Event
        ↓
Semantic Dispatcher
        ├────────► StackChan Adapter
        ├────────► Ambient Runtime Adapter
        ├────────► RGB Adapter
        ├────────► Voice Pyramid Adapter
        └────────► Wearable Adapter
```

------------------------------------------------------------------------

# Responsibilities

## Context Builder

Produces the validated 5W Context Package.

## Context Registry

Maintains the current runtime state.

## Semantic Event Generator

Transforms semantic context into semantic decisions.

## Semantic Dispatcher

Routes Semantic Events to the correct adapters.

## Adapters

Translate Semantic Events into device-specific protocols.

------------------------------------------------------------------------

# Semantic Event Principles

-   Consumers never read the Context Package directly.
-   Consumers never parse NFC or UID data.
-   Consumers never access Profile Registry internals.
-   All consumers receive normalized semantics only.

------------------------------------------------------------------------

# Example Event Categories

-   researcher_session_started
-   ambient_content_update
-   stackchan_context_update
-   expression_color_request
-   voice_message_request
-   haptic_notification_request

------------------------------------------------------------------------

# External Data Policy

External services are consulted only by the AX630C.

Data must be:

-   real;
-   traceable;
-   normalized;
-   timestamped.

No fictitious data shall be generated for demonstrations.

------------------------------------------------------------------------

# Implementation Roadmap

## Step 1

Implement Semantic Event Generator.

## Step 2

Implement Semantic Dispatcher.

## Step 3

Adapt StackChanNotifier to consume Semantic Events.

## Step 4

Connect additional adapters:

-   Ambient Runtime
-   RGB
-   Voice Pyramid
-   Wearables

------------------------------------------------------------------------

# Success Criteria

The runtime shall:

-   preserve existing validated components;
-   produce Semantic Events from the Current Context;
-   distribute Semantic Events without exposing internal runtime
    structures;
-   provide a stable interface for downstream integration.

------------------------------------------------------------------------

# Final Decision

The Semantic Event Architecture V1 is now frozen.

Further work shall focus exclusively on implementation, validation and integration.

Architectural modifications shall only be introduced through a future versioned architecture document.

Document Status

Architecture Version:
V1

Status:
FROZEN

Purpose:
Reference architecture for the implementation used in the M5Stack Global Innovation Contest 2026.

Next milestone:
Implementation and end-to-end validation.