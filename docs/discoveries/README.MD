# Discoveries

## Ambient Physical AI

The `docs/discoveries/` directory preserves engineering discoveries made during the evolution of the Ambient Physical AI project.

Unlike Architecture, Technical Notes, or Milestones, the documents stored here record the investigation process that led to important engineering decisions. They capture protocol analysis, hardware characterization, implementation experiments, integration studies, and architectural discoveries before they became part of the validated system.

These reports are preserved as technical history and provide traceability between experimental investigations and the project's current implementation.

---

## Engineering Philosophy

The Discoveries directory follows a simple engineering principle:

```text
Explore
↓
Understand
↓
Validate
↓
Integrate
```

Every discovery should be based on practical investigation and observable evidence.

The implementation, architecture, and validated documentation remain the authoritative sources for the current project state. Discovery documents preserve how that knowledge was originally obtained.

---

# Discovery Reports

## DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_EN.md

**Purpose**

Provides a concise overview of the first successful investigation of the StackFlow communication protocol running on the AX630C Cognitive Runtime.

**Contents**

* Identification of the TCP communication endpoint.
* Validation of the JSON-based communication protocol.
* Session creation workflow.
* Local inference lifecycle.
* Streaming response behavior.
* Initial engineering conclusions.

**Engineering Relevance**

This report documents the first successful programmatic interaction with the AX630C runtime and established the foundation for all subsequent Cognitive Runtime integrations.

**Related Document**

* `DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_DETAILED_EN.md`

---

## DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_DETAILED_EN.md

**Purpose**

Provides the complete engineering record of the StackFlow protocol investigation.

**Contents**

* Experimental environment.
* Runtime services identified.
* Protocol messages.
* Session allocation.
* Python client implementation.
* Runtime architecture.
* Engineering conclusions.
* Architectural impact.

**Engineering Relevance**

This document serves as the primary technical record of the StackFlow protocol discovery and preserves the evidence supporting the protocol validation.

**Related Documents**

* `DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_EN.md`
* `DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_EN_COMPLETE.md`

---

## DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_EN_COMPLETE.md

**Purpose**

Documents the investigation of session behavior, context injection, conversational state, and runtime stability after the communication protocol had been successfully identified.

**Contents**

* Session lifecycle analysis.
* Context injection experiments.
* Multi-turn conversation evaluation.
* Runtime stability observations.
* Recovery procedures.
* Architectural implications for context management.

**Engineering Relevance**

The investigation demonstrated that Ambient Physical AI should treat each event as carrying its own contextual information, influencing the design of later semantic processing components.

**Related Documents**

* `DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_EN.md`
* `DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_DETAILED_EN.md`

---

## echo-pyramid-atoms3r-characterization.md

**Purpose**

Records the engineering characterization of the Echo Pyramid and AtomS3R hardware platform before the ESP-IDF implementation of the Expression Layer.

**Contents**

* Hardware baseline validation.
* Arduino-based hardware characterization.
* Audio subsystem observations.
* Touch interface evaluation.
* AudioStage investigation.
* Engineering decisions for the ESP-IDF migration.

**Engineering Relevance**

This report documents the practical knowledge obtained during hardware evaluation and defines the validated baseline used before firmware development moved entirely to ESP-IDF.

---

## IDENTITY_PROFILE_IMAGES_V1.md

**Purpose**

Documents the engineering decisions adopted during the implementation of profile photographs in the Identity Node Version 1.

**Contents**

* Motivation for visual profile identification.
* ProfileImageManager abstraction.
* Embedded RGB565 image strategy.
* Image conversion pipeline.
* Firmware integration.
* Temporary implementation rationale.
* Planned migration path toward synchronized profile storage.

**Engineering Relevance**

This document preserves the engineering reasoning behind the first profile visualization implementation while maintaining a clear migration path toward a future dynamic profile infrastructure.

---
## STACKCHAN_INTEGRATION_SUSPENSION_001.md

**Purpose**

Documents the engineering decision to suspend the initial StackChan integration laboratory after identifying a potentially more appropriate integration strategy through the Model Context Protocol (MCP).

**Contents**

* Original integration objective.
* Implemented software artifacts.
* Validated components.
* Context Builder results.
* Human-readable message generation.
* WebSocket capability validation.
* MCP-related architectural discovery.
* Reasons for suspending the laboratory.
* Conditions required to resume the investigation.

**Engineering Relevance**

This document demonstrates an important engineering principle adopted throughout the project: implementation may be intentionally suspended when new technical evidence indicates that continuing would introduce unnecessary rework or architectural inconsistency.

**Related Documents**

* `STACKCHAN_MCP_DISCOVERY_001.md`
* `STACKCHAN_MCP_DISCOVERY_001_CONSOLIDATED.md`
* `STACKCHAN_MCP_DISCOVERY_002.md`

---

## STACKCHAN_MCP_DISCOVERY_001.md

**Purpose**

Records the initial discovery that StackChan appeared to communicate through an MCP-based architecture rather than a direct command interface.

**Contents**

* Initial StackChan integration assumptions.
* Discovery of the MCP endpoint.
* Network validation.
* AX630C WebSocket capability.
* Early architectural reinterpretation.
* Context-oriented integration hypothesis.
* Engineering risks.
* Recommended next laboratory.

**Engineering Relevance**

This investigation shifted the project's understanding of StackChan integration from direct message delivery toward semantic context exchange through MCP, establishing the basis for subsequent validation work.

**Related Documents**

* `STACKCHAN_INTEGRATION_SUSPENSION_001.md`
* `STACKCHAN_MCP_DISCOVERY_001_CONSOLIDATED.md`
* `STACKCHAN_MCP_DISCOVERY_002.md`

---

## STACKCHAN_MCP_DISCOVERY_001_CONSOLIDATED.md

**Purpose**

Consolidates the complete MCP discovery cycle, combining the original architectural observations with the subsequent laboratory validation.

**Contents**

* Initial architectural hypothesis.
* MCP investigation rationale.
* Experimental environment.
* Security considerations.
* MCP protocol validation.
* Tool Provider validation.
* Semantic tool execution.
* Architectural conclusions.
* Final engineering assessment.

**Engineering Relevance**

This document represents the consolidated engineering record of the MCP investigation and establishes the validated architectural foundation for exposing semantic services from the AX630C Cognitive Runtime.

**Related Documents**

* `STACKCHAN_MCP_DISCOVERY_001.md`
* `STACKCHAN_MCP_DISCOVERY_002.md`

---

## STACKCHAN_MCP_DISCOVERY_002.md

**Purpose**

Documents the experimental validation that confirmed the AX630C Cognitive Runtime can operate as an MCP Tool Provider for the StackChan/XiaoZhi ecosystem.

**Contents**

* Laboratory objectives.
* Experimental environment.
* MCP handshake validation.
* Tool discovery.
* Tool invocation.
* Semantic service validation.
* Validated communication flow.
* Architectural conclusions.
* Recommended next milestone.

**Engineering Relevance**

This report concludes the protocol investigation by demonstrating that semantic services exposed by the Cognitive Runtime can be consumed through MCP without requiring modifications to the StackChan firmware.

**Related Documents**

* `STACKCHAN_MCP_DISCOVERY_001.md`
* `STACKCHAN_MCP_DISCOVERY_001_CONSOLIDATED.md`

---

# Relationship with Other Documentation

The Discoveries directory complements the remaining documentation maintained by the Ambient Physical AI project.

Each documentation area has a distinct purpose:

| Directory            | Primary Purpose                                                                                         |
| -------------------- | ------------------------------------------------------------------------------------------------------- |
| `docs/architecture/` | Defines the approved system architecture and component responsibilities.                                |
| `docs/discoveries/`  | Preserves engineering investigations and technical discoveries that influenced project evolution.       |
| `docs/notes/`        | Documents focused engineering knowledge, implementation details, and reproducible technical procedures. |
| `docs/milestones/`   | Records completed engineering achievements and validated project phases.                                |
| `firmware/`          | Documents the implementation of embedded nodes and firmware components.                                 |
| `runtime/`           | Documents Cognitive Runtime services, StackFlow components, and runtime infrastructure.                 |

Together, these directories provide complete traceability from engineering investigation to validated implementation.

---

# Preservation Policy

Discovery documents are preserved because they capture engineering knowledge that influenced the evolution of Ambient Physical AI.

Even after an implementation becomes stable or an architectural decision is consolidated, the original investigation remains valuable as technical history.

Some reports may eventually be consolidated or superseded by newer documentation. When this occurs, the original discoveries should normally remain available as historical engineering references unless an equivalent consolidated document fully preserves their technical content.

---

# Conclusion

The `docs/discoveries/` directory serves as the engineering history of Ambient Physical AI.

Rather than describing the current implementation, it documents the investigations, experiments, hardware characterization activities, protocol analyses, and architectural discoveries that shaped the project's evolution.

These reports provide the technical context behind important engineering decisions and complement the project's Architecture, Technical Notes, Milestones, Firmware, and Runtime documentation by preserving the knowledge acquired throughout the development process.
