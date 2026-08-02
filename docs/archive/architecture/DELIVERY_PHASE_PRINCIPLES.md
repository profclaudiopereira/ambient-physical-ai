# DELIVERY_PHASE_PRINCIPLES

## Ambient Physical AI

### Status

Architectural / Engineering Decision

### Date

2026-06-28

------------------------------------------------------------------------

# Purpose

This document establishes the engineering principles that guide the
Ambient Physical AI project during the delivery phase leading to the
competition submission.

The architecture is considered sufficiently mature.

From this point forward, engineering effort shall prioritize complete,
validated and reproducible milestones instead of expanding architectural
scope.

------------------------------------------------------------------------

# Current Project Phase

The project has transitioned from:

``` text
Architecture Exploration
```

to:

``` text
Architecture Consolidation
```

and now enters:

``` text
System Integration and Delivery
```

The objective is no longer to discover new architectural directions.

The objective is to complete the validated architecture and integrate
all project layers into a coherent demonstration.

------------------------------------------------------------------------

# Engineering Philosophy

Every implementation should follow:

``` text
Plan
    │
    ▼
Implement
    │
    ▼
Validate on Real Hardware
    │
    ▼
Document
    │
    ▼
Commit
```

Experimental code should not be committed.

Only validated engineering milestones should become part of the
repository.

------------------------------------------------------------------------

# Milestone Strategy

Each laboratory should deliver one complete architectural block.

A milestone is considered complete only when it is:

-   Implemented
-   Validated on hardware
-   Documented
-   Reproducible
-   Committed to Git

Large exploratory branches should be avoided.

------------------------------------------------------------------------

# Scope Control

The following topics are intentionally deferred until after the
competition unless they become essential to the demonstration:

-   Persistent Memory
-   RAG
-   Databases
-   Multiple simultaneous users
-   Performance optimizations without measured need
-   Premature abstractions
-   Features that do not directly strengthen the final demonstration

Deferring these topics is a conscious engineering decision, not a
limitation.

------------------------------------------------------------------------

# Demonstration-Oriented Development

Every new implementation should answer the question:

> Does this feature directly improve or enable the final demonstration?

If the answer is **yes**, it belongs to the current milestone.

If the answer is **no**, it should be documented as future work rather
than implemented immediately.

------------------------------------------------------------------------

# Integration Priority

The remaining work should prioritize:

1.  Cognitive Runtime completion
2.  Semantic services
3.  MCP exposure
4.  Ambient Runtime integration
5.  Expression Layer integration
6.  End-to-end demonstration
7.  Competition documentation
8.  Reproducibility review

------------------------------------------------------------------------

# Engineering Principle

Prefer:

``` text
Simple
Validated
Documented
Committed
```

over:

``` text
Complex
Experimental
Incomplete
Open-ended
```

The architecture should evolve through small, complete and reusable
integration blocks.

------------------------------------------------------------------------

# Guiding Statement

``` text
Each new implementation shall move the project measurably closer to the final demonstration.

If a feature does not strengthen the demonstration, it should be documented for future evolution instead of being implemented before the competition.
```

------------------------------------------------------------------------

# Final Statement

The Ambient Physical AI project now prioritizes disciplined delivery.

Architecture exploration has produced a solid foundation.

The remaining engineering effort should transform that foundation into a
complete, validated and reproducible system capable of demonstrating the
project's vision.
