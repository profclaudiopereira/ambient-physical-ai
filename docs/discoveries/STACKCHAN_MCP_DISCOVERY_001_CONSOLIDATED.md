# STACKCHAN_MCP_DISCOVERY_001

## Ambient Physical AI

**Distributed Cognitive Ecosystem Powered by StackFlow**

------------------------------------------------------------------------

# Purpose

This document consolidates the complete MCP discovery cycle for the
Ambient Physical AI project, preserving both the original architectural
reflections and the subsequent experimental validation.

It records:

-   the initial architectural discovery;
-   the reasoning that motivated the dedicated MCP laboratory;
-   the experimental validation;
-   the resulting architectural conclusions.

------------------------------------------------------------------------

# Phase 1 --- Architectural Discovery

The original objective was to understand how the AX630C Cognitive
Runtime could notify StackChan after a user was identified.

During the investigation, it became clear that the important question
was no longer:

``` text
How do we send commands to StackChan?
```

but:

``` text
How do we provide context to StackChan?
```

Inspection of the StackChan configuration revealed the presence of an
MCP endpoint:

``` text
wss://api.xiaozhi.me/mcp/?token=<REDACTED>
```

This led to the hypothesis that StackChan operated through a
broker-based MCP architecture rather than a direct command interface.

The original architectural assumption:

``` text
AX630C
↓
Direct Command
↓
StackChan
```

evolved into:

``` text
AX630C
↓
MCP Tool Provider
↓
XiaoZhi MCP Broker
↓
LLM
↓
StackChan
```

This hypothesis motivated the creation of LAB_STACKCHAN_MCP_001.

------------------------------------------------------------------------

# Phase 2 --- Experimental Validation

## Environment

-   AX630C + LLM Mate
-   StackChan
-   Ubuntu 22.04
-   Python 3
-   websocket-client

## Security

-   Tokens never committed to Git.
-   Environment variables used for configuration.
-   Documentation always uses:

``` text
wss://api.xiaozhi.me/mcp/?token=<REDACTED>
```

## Validated Experiments

  Experiment                  Result
  --------------------------- --------
  WebSocket connectivity      PASS
  MCP initialize              PASS
  notifications/initialized   PASS
  tools/list                  PASS
  tools/call                  PASS
  ping                        PASS
  Dummy MCP tool              PASS
  Semantic MCP tool           PASS

The semantic tool:

``` text
GetCurrentCountry()
```

returned:

``` text
Brazil
```

and StackChan correctly incorporated the returned value into its final
response.

------------------------------------------------------------------------

# Observed MCP Flow

``` text
User
↓
StackChan
↓
LLM
↓
XiaoZhi MCP Broker
↓
AX630C MCP Tool Provider
↓
Semantic Tool
↓
LLM
↓
StackChan Response
```

------------------------------------------------------------------------

# What Was Validated

-   WebSocket communication
-   MCP JSON-RPC handshake
-   Tool discovery
-   Tool invocation
-   Keepalive
-   AX630C as MCP Tool Provider
-   Semantic tool execution
-   StackChan consuming AX630C knowledge

------------------------------------------------------------------------

# What Was Not Validated

-   Context Push
-   Streaming
-   MCP Resources
-   MCP Prompts
-   Production semantic services

These remain future milestones.

------------------------------------------------------------------------

# Architectural Impact

The laboratory confirmed that the AX630C should evolve from a Context
Builder into the Cognitive Service Provider of the Ambient Physical AI
ecosystem.

Instead of exposing low-level hardware information, it should expose
semantic services through MCP.

Examples:

``` text
GetCurrentIdentity()
GetPresenceState()
GetEnvironmentContext()
GetRuntimeStatus()
```

------------------------------------------------------------------------

# Final Conclusion

The original architectural hypothesis was experimentally confirmed.

The AX630C was successfully validated as an MCP Tool Provider capable of
supplying semantic knowledge to StackChan through the XiaoZhi MCP
broker.

This laboratory concludes the protocol investigation phase and
establishes the foundation for the next milestone: implementing real
Ambient Physical AI cognitive services over MCP.

------------------------------------------------------------------------

# Laboratory Status

``` text
DISCOVERY ............. COMPLETED
VALIDATION ............ COMPLETED
ARCHITECTURE .......... UPDATED

Overall Result:

AX630C validated as an MCP Tool Provider.
```
