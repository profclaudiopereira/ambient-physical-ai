# TECHNICAL_NOTE_001_RUNTIME_STATE_INTEGRATION.md

# Technical Note

**Project:** Ambient Physical AI\
**Node:** StickC Plus 2 RGB Node (Runtime State Indicator)\
**Phase:** Runtime State Integration and Visual Timing Stabilization

------------------------------------------------------------------------

## Objective

This note records the engineering changes implemented during the Runtime
State integration between the StackFlow Cognitive Runtime (AX630C) and
the StickC Plus 2 Runtime State Indicator.

The purpose is to preserve implementation details before the README is
updated.

------------------------------------------------------------------------

# StackFlow Changes (AX630C)

## New Runtime State channel

A dedicated Runtime State pipeline was introduced, completely
independent from the Semantic Event pipeline.

Responsibilities:

-   publish Runtime States through UDP;
-   keep Semantic Events unchanged;
-   avoid coupling Runtime State visualization with Semantic Event
    dispatch.

Implemented states:

-   idle
-   thinking
-   responding
-   error

------------------------------------------------------------------------

## New notifier

Added:

    runtime_state_notifier.py

UDP contract:

``` json
{
  "type":"runtime_state",
  "target":"runtime_state_indicator",
  "state":"thinking"
}
```

------------------------------------------------------------------------

## identity_udp_listener.py

Updated to:

-   publish `idle` at startup;
-   publish `thinking` during context construction;
-   publish `responding` after Semantic Event dispatch;
-   publish `idle` in `finally`;
-   isolate Runtime State failures using protected notification calls;
-   remove obsolete `processing_context`.

------------------------------------------------------------------------

## Deployment

Deployment script updated to copy:

    runtime_state_notifier.py
    identity_udp_listener.py

------------------------------------------------------------------------

# StickC Plus 2 RGB Node Changes

## Runtime State Scheduler

The Runtime State consumer was redesigned.

Previous behavior:

    Receive
    ↓
    Display immediately

Current behavior:

    Receive
    ↓
    Queue
    ↓
    Worker Task
    ↓
    Minimum display time
    ↓
    Next state

Characteristics:

-   asynchronous queue;
-   dedicated worker task;
-   non-blocking UDP reception;
-   no delay introduced into AX630C.

------------------------------------------------------------------------

## Minimum display policy

  Runtime State     Minimum Time
  --------------- --------------
  thinking                500 ms
  responding              800 ms
  error                  1500 ms
  alert                  1500 ms
  others               immediate

------------------------------------------------------------------------

## Firmware modules updated

-   semantic_consumer.h
-   semantic_consumer.c
-   stickc_plus2_rgb_node.cpp

Responsibilities introduced:

-   Runtime State queue
-   asynchronous worker
-   presentation scheduler

------------------------------------------------------------------------

## Visual color mapping

  Runtime State   Color
  --------------- ---------------------------
  idle            Green
  presence        Yellow
  listening       Magenta
  thinking        White
  responding      Blue
  alert           Orange
  error           Red
  offline         Gray (planned convention)
  learning        Purple

This mapping applies **only** to the StickC Plus 2 Runtime State
Indicator.

The RGB Strip Node and Atom Matrix Node continue to represent Ambient
Transformation through Semantic Events.

------------------------------------------------------------------------

# Architectural Result

Two independent visualization pipelines now coexist:

## Runtime State

AX630C

↓

Runtime State Notifier

↓

StickC Plus 2

↓

Runtime Status Visualization

------------------------------------------------------------------------

## Semantic Events

AX630C

↓

Semantic Dispatcher

↓

RGB Strip

↓

Atom Matrix

↓

Ambient Runtime

↓

Voice Pyramid

↓

Other semantic consumers

------------------------------------------------------------------------

# Validation

Validated behavior:

    IDLE
    ↓
    THINKING (500 ms)
    ↓
    RESPONDING (800 ms)
    ↓
    IDLE

Confirmed:

-   asynchronous presentation;
-   non-blocking communication;
-   correct Runtime State sequence;
-   visual differentiation between states.

------------------------------------------------------------------------

## Pending

Future README update documenting:

-   Runtime State architecture;
-   timing scheduler;
-   color convention;
-   engineering rationale.

This technical note is temporary and should be incorporated into the
official README after review.
