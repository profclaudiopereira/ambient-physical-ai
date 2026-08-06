# Milestones

## Ambient Physical AI

### Purpose

The `docs/milestones/` directory contains validated engineering milestones achieved during the evolution of the Ambient Physical AI project.

Each document records a completed engineering checkpoint that has been successfully validated. Milestones preserve significant technical achievements rather than implementation details or ongoing development work.

---

## Organization

A milestone represents a validated engineering baseline that marks measurable progress in the project lifecycle.

Implementation details, design rationale, architectural decisions, experiments, and development notes are documented elsewhere in the repository, including Architecture documents, Technical Notes, Discoveries, and component-specific README files.

---

## Milestone Catalog

### `FOUNDATION_MILESTONE_001.md`

Records the first successful execution of an operational firmware node for the project. It validates the initial ESP-IDF development environment, repository structure, build pipeline, flashing workflow, runtime execution, and establishes the transition from architectural planning to executable firmware development.

**Engineering purpose:** Establish the project's first validated firmware baseline.

---

### `PRESENCE_NODE_V1_BRINGUP_001.md`

Documents the successful bring-up and validation of the Presence Node V1 hardware using the AtomS3 Lite and VL53L0X ToF sensor. The milestone confirms the approved separation between the Presence and Identity layers while validating hardware communication and distance measurements.

**Engineering purpose:** Establish the validated hardware baseline for the Presence Layer.

---

### `SYSTEM_INTEGRATION_MILESTONE_001.md`

Records the completion of the first major system integration milestone by consolidating independently developed subsystems into a unified engineering baseline and primary development branch.

**Engineering purpose:** Mark the transition from isolated subsystem development to an integrated distributed system.

---

### `M004.6_SH1107_OLED_VALIDATION.md`

Documents the successful validation of the SH1107 OLED display connected through the PaHub on the Ambient Runtime Node. The milestone verifies hardware detection, driver bring-up, rendering validation, and stable text display under ESP-IDF.

**Engineering purpose:** Establish the validated display foundation for Ambient Runtime visualization.

---

### `EXPRESSION_RGB_STRIP_NODE_MILESTONE_001.md`

Records the successful validation of the complete semantic processing pipeline for the RGB Strip Expression Node, including Wi-Fi connectivity, UDP semantic event reception, JSON validation, semantic processing, and synchronized RGB expressions.

**Engineering purpose:** Validate the first operational semantic Expression Layer node.

---

### `MCP_RUNTIME_INTEGRATION_MILESTONE_001_V2.md`

Documents the validated production integration between the Cognitive Runtime and StackChan through the Model Context Protocol (MCP). The milestone confirms end-to-end semantic context retrieval from the production runtime using authenticated identities and Semantic Tools.

**Engineering purpose:** Validate context-aware conversational integration through the production MCP infrastructure.


---

### `ECHO_PYRAMID_VOICE_INTEGRATION_MILESTONE.md`

Documents the first complete end-to-end integration of the Echo Pyramid Voice Node with the Ambient Physical AI Cognitive Runtime, validating personalized voice generation through StackFlow Text-to-Speech services, TCP voice transport, embedded audio playback, and the architectural separation between centralized cognition and distributed expression.

**Engineering purpose:** Establish the validated engineering baseline for distributed personalized voice interaction within the Expression Layer.

---

## Version Notes

The previous `MCP_RUNTIME_INTEGRATION_MILESTONE_001.md` has been superseded by `MCP_RUNTIME_INTEGRATION_MILESTONE_001_V2.md`.

The V2 document provides a more complete engineering record, including expanded architectural context, validation procedure, runtime integration details, and engineering significance. Maintaining only the V2 document in this directory provides a clearer and more maintainable milestone catalog while preserving the authoritative engineering record.

---

## Maintenance Notes

New validated engineering achievements should be recorded as additional milestone documents.

Existing milestone documents should normally remain immutable after validation. Editorial improvements, navigation enhancements, and documentation organization should preferentially be implemented in this `README.md` rather than by modifying historical milestone records.
