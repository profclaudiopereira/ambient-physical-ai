# Technical Notes

## Ambient Physical AI

### Mission

The `docs/notes/` directory preserves validated engineering knowledge
that complements the project's architectural and implementation
documentation.

Technical Notes record the engineering rationale behind important
implementation decisions, investigations, calibration work, integration
strategies, and validated technical solutions. They explain **why** a
solution was adopted rather than **how** to build or use a component.

Unlike Architecture documents, Technical Notes focus on specific
engineering subjects. Unlike Discoveries, they document validated
conclusions rather than exploratory investigations. Unlike Milestones,
they do not describe project progress or delivery stages.

------------------------------------------------------------------------

# Documentation Philosophy

Technical Notes should:

-   preserve validated engineering knowledge;
-   document implementation rationale;
-   record important investigations and engineering decisions;
-   complement, not replace, component READMEs;
-   remain concise, technically rigorous and reproducible.

Whenever the implementation becomes easier to understand because of the
engineering reasoning behind it, a Technical Note is the appropriate
documentation artifact.

------------------------------------------------------------------------

# Relationship with Other Documentation

  -----------------------------------------------------------------------
  Category                    Primary Purpose
  --------------------------- -------------------------------------------
  `architecture/`             System structure, responsibilities and
                              long-term design.

  `discoveries/`              Engineering investigations and discoveries
                              before consolidation.

  `milestones/`               Validated project achievements and
                              integration baselines.

  `notes/`                    Engineering rationale, implementation
                              decisions and technical reference material.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

# Technical Note Catalog

## `ECHO_PYRAMID_WAKE_WORD_CONTEXT_INTEGRATION_TECHNICAL_NOTE.md`

Documents the validated engineering decisions behind integrating Wake
Word, offline context recognition, ESP-SR, and the Cognitive Runtime
while preserving the project's distributed architecture.

------------------------------------------------------------------------

## `NFC_IDENTITY_RESOLUTION_STRATEGY.md`

Documents the approved NFC identity resolution strategy based on UID
Mapping with future NDEF support while preserving backward
compatibility.

------------------------------------------------------------------------

## `NOTE_001_STATIC_IP_CONFIGURATION.md`

Records the engineering decision to migrate the RGB Strip Node from DHCP
to deterministic static IPv4 addressing to improve reproducibility and
distributed deployment.

------------------------------------------------------------------------

## `NOTE_001_TAB5_ESP32C6_POWER_ENABLE_INVESTIGATION.md`

Preserves the complete engineering investigation that identified the
mandatory ESP32-C6 power-enable sequence required for the M5Stack Tab5
wireless subsystem.

------------------------------------------------------------------------

## `NOTE_002_PROFILE_BASED_LIGHTING.md`

Documents the evolution of the RGB Strip Node from generic semantic
feedback to profile-aware ambient lighting while preserving the existing
semantic architecture.

------------------------------------------------------------------------

## `NOTE_002_SH1107_DISPLAY_GEOMETRY_VALIDATION.md`

Records the investigation, validation and engineering rationale behind
the SH1107 initialization sequence adopted for the Ambient Runtime Mini
OLED.

------------------------------------------------------------------------

## `NOTE_ATOM_MATRIX_RGB_NODE_PROFILE_LIGHTING.md`

Documents the engineering decisions, calibration methodology and
profile-aware lighting implementation for the Atom Matrix RGB Node.

------------------------------------------------------------------------

## `TECHNICAL_NOTE_001_RUNTIME_STATE_INTEGRATION.md`

Preserves the engineering rationale behind the Runtime State
communication pipeline, asynchronous presentation scheduler and visual
timing policies adopted for the Runtime State Indicator.

------------------------------------------------------------------------

## `TECHNICAL_NOTE_PERSONALIZED_WELCOME_INTEGRATION.md`

Documents the protocol evolution and implementation decisions that
introduced personalized welcome messages while maintaining complete
backward compatibility.

------------------------------------------------------------------------

## `WAKE_WORD_CALIBRATION_AND_BUILD_RECOVERY.md`

Records the validated procedures required to reproduce the build
environment, recover ESP-SR configuration and perform controlled Wake
Word calibration.

------------------------------------------------------------------------

# Maintenance Guidelines

A new Technical Note should only be created when there is engineering
knowledge that cannot be adequately preserved in source code comments or
a component README.

Typical candidates include:

-   validated engineering investigations;
-   implementation rationale;
-   platform-specific hardware behavior;
-   integration strategies;
-   calibration methodologies;
-   reproducible engineering procedures.

Technical Notes should avoid duplicating Architecture documents,
Discoveries, Milestones or operational READMEs. Their purpose is to
preserve the engineering reasoning that future developers are unlikely
to infer solely from the implementation.

------------------------------------------------------------------------

# Conclusion

The Technical Notes directory represents the engineering memory of the
Ambient Physical AI project. Together, these documents preserve the
reasoning, validation process and technical decisions that support the
current implementation, improving long-term maintainability while
complementing the repository's architectural and component
documentation.
