# Snapshots

## Ambient Physical AI

### Purpose

This directory preserves **technical snapshots** that capture
significant, validated states in the evolution of the Ambient Physical
AI project.

Unlike architecture documents, technical notes, discovery reports, or
milestones, a snapshot records the project exactly as it existed at a
particular point in time. It serves as a historical engineering
reference, documenting implementation progress, validated hardware,
architectural understanding, and integration status at that specific
stage of development.

Snapshots intentionally preserve engineering history. Although later
documentation may supersede individual implementation details, these
documents remain valuable records of how the project evolved.

------------------------------------------------------------------------

# What Constitutes a Snapshot?

A snapshot is a time-based engineering record that captures a validated
state of the project.

Typical contents include:

-   Implemented subsystems
-   Validated hardware configurations
-   Runtime bring-up status
-   Integration progress
-   Architectural understanding at that moment
-   Verified engineering observations
-   Immediate next development objectives

Snapshots describe **what the project looked like at a specific point in
time**, not necessarily its current implementation.

------------------------------------------------------------------------

# Relationship with Other Documentation

Each documentation category has a distinct responsibility.

  -----------------------------------------------------------------------
  Directory                               Purpose
  --------------------------------------- -------------------------------
  `docs/architecture/`                    Current architectural
                                          definition of the system.

  `docs/discoveries/`                     Engineering discoveries,
                                          investigations, and validated
                                          technical findings.

  `docs/milestones/`                      Significant completed
                                          engineering achievements.

  `docs/notes/`                           Technical notes, implementation
                                          references, and engineering
                                          guidance.

  `docs/snapshots/`                       Historical records preserving
                                          validated technical states
                                          during the project's evolution.
  -----------------------------------------------------------------------

Snapshots complement the permanent documentation by preserving the
engineering context of important development phases.

------------------------------------------------------------------------

# Snapshot Catalog

## `SNAPSHOT_2026_06_AMBIENT_RUNTIME_M004_4A.md`

Records the validated bring-up of the first Ambient Runtime Node based
on the ESP32-P4 platform, including hardware initialization, I²C
validation, ENV-IV integration, and the first successful environmental
sensor readings.

**Engineering significance:** Preserves the transition of the Ambient
Runtime Layer from architectural concept to operational firmware.

------------------------------------------------------------------------

## `SNAPSHOT_2026_06_ARCHITECTURAL_DISCOVERIES.md`

Documents the architectural understanding reached after the project's
transition from conceptual design toward hardware implementation,
including refinements to the Cognitive Runtime, Ambient Runtime,
Physical Context Layer, StackFlow, and StackChan roles.

**Engineering significance:** Captures the architectural baseline that
guided the implementation phase.

------------------------------------------------------------------------

## `SNAPSHOT_2026_06_ARCHITECTURAL_DISCOVERIES.pt-BR.md`

Portuguese version of the Architectural Discoveries snapshot, preserving
the same engineering content for Portuguese-speaking collaborators.

**Engineering significance:** Maintains the historical architectural
record in both supported project languages.

------------------------------------------------------------------------

## `SNAPSHOT_2026_06_AX630C_BRINGUP.md`

Documents the successful bring-up and validation of the AX630C Cognitive
Runtime platform, including operating system verification, AI runtime
discovery, local inference validation, and identification of the primary
integration endpoint.

**Engineering significance:** Marks the successful validation of the
project's Cognitive Runtime platform before system-wide integration
activities.

------------------------------------------------------------------------

# Review Summary

The current snapshot collection is consistent and well scoped.

Observations:

-   Each snapshot represents a distinct engineering checkpoint.
-   No unnecessary duplication was identified.
-   The English and Portuguese Architectural Discoveries documents
    intentionally serve different audiences.
-   None of the current snapshots should be reclassified into
    Architecture, Notes, Discoveries, or Milestones.

------------------------------------------------------------------------

# Guidelines for Future Snapshots

Create a new snapshot only when the project reaches a meaningful,
validated engineering state that should be preserved historically.

A snapshot should:

-   represent a verified implementation state;
-   capture the engineering context of a specific development period;
-   document validated information only;
-   remain historically accurate after publication;
-   avoid speculative designs or future planning.

Snapshots should not replace architecture documents, technical notes,
discovery reports, or milestone documentation.

------------------------------------------------------------------------

# Engineering Principle

Snapshots preserve the engineering history of Ambient Physical AI.

They provide immutable records of important technical states, allowing
future contributors to understand how validated implementations,
architectural decisions, and system integration evolved throughout the
project.
