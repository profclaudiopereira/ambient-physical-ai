# Architecture

This directory contains the official architectural documentation of the Ambient Physical AI project.

The documents presented here describe the validated architecture of the distributed ecosystem and serve as the primary architectural reference for the repository.

They explain the overall system organization, subsystem responsibilities, runtime coordination model, engineering philosophy, and communication contracts without duplicating implementation-specific documentation.

Readers interested in firmware organization, hardware integration, or implementation details should consult the corresponding subsystem documentation, README files, and Technical Notes available throughout the repository.

---

# Architecture Documents

| Document | Description |
|----------|-------------|
| **AMBIENT_PHYSICAL_AI_SYSTEM_ARCHITECTURE.md** | Primary architectural overview of Ambient Physical AI. Describes the distributed cognitive ecosystem, architectural layers, subsystem responsibilities, end-to-end operation, validated reference hardware, and overall architectural vision. |
| **RUNTIME_AND_COMMUNICATION_ARCHITECTURE.md** | Describes the Cognitive Runtime, StackFlow, semantic communication model, distributed coordination, communication contracts, runtime services, and information flow throughout the ecosystem. |
| **ENGINEERING_PRINCIPLES.md** | Defines the engineering philosophy adopted by the project, including architectural engineering principles, reproducibility, validation strategy, documentation standards, and long-term maintainability guidelines. |
| **IDENTITY_PACKAGE_SPECIFICATION.md** | Official specification of the Identity Package communication contract, including semantic structure, compatibility rules, validation requirements, and architectural responsibilities. |

---

# Reference Implementation

The system architecture is intentionally defined in terms of **architectural responsibilities** rather than specific hardware platforms.

The validated reference implementation is documented within **AMBIENT_PHYSICAL_AI_SYSTEM_ARCHITECTURE.md**, where each architectural responsibility is mapped to the hardware platform used during system validation.

This distinction preserves hardware independence while providing a clear reference for the implementation submitted to the M5Stack Global Innovation Contest.

---

# Relationship to the Repository

The architecture documents provide the conceptual foundation of Ambient Physical AI.

Additional documentation is organized as follows:

- **README files** describe subsystem implementation and usage.
- **Technical Notes** document engineering investigations and validated technical findings.
- **Engineering Notes** preserve engineering rationale and implementation experience.
- **Firmware documentation** explains platform-specific implementation details.

Together, these resources provide a complete view of the project while maintaining a clear separation between architecture, implementation, and engineering knowledge.

---

# Design Philosophy

Ambient Physical AI is documented as a **distributed cognitive ecosystem**, not as a collection of independent firmware projects.

The architecture emphasizes:

- responsibility-driven subsystem design;
- semantic communication;
- distributed cognition;
- modular engineering;
- implementation-independent interfaces;
- reproducible engineering.

These principles remain stable even as the underlying hardware and software implementations evolve.

---

# Historical Architecture

Earlier architectural studies, design explorations, and superseded architectural documents have been preserved in:

```text
docs/archive/architecture/
```

Those documents record the historical evolution of the project and provide engineering context, but they are **not** the authoritative architectural reference.

The documents contained in this directory represent the current validated architecture of Ambient Physical AI.

---

**Last Updated:** August 2026