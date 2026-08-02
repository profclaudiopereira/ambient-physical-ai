# Engineering and Reproducibility Principles

## Ambient Physical AI

### Status

Architectural Guideline

### Date

2026-06-24

---

# Purpose

This document defines the engineering principles used throughout the Ambient Physical AI project.

These principles apply to:

```text
Firmware Nodes
Runtime Components
StackFlow Services
Infrastructure Components
Documentation
Future Extensions
```

The objective is to ensure that the project remains:

```text
Understandable
Reproducible
Maintainable
Evaluable
```

by developers, researchers and competition judges.

---

# Core Principle

A component is not considered complete when only the code exists.

A component is considered complete when it provides:

```text
Code
+
Documentation
+
Reproduction Guide
+
Validation Evidence
```

---

# Principle 1 — Code

Every component must provide working source code.

Examples:

```text
Presence Node
Identity Node
Ambient Runtime Node
Expression Node
StackFlow Runtime
AX630C Services
```

The code must be stored in the appropriate repository location and be sufficient for building or executing the component.

---

# Principle 2 — Documentation

Every component must include technical documentation.

At minimum, documentation should answer:

```text
What is it?
Why does it exist?
How does it work?
```

Documentation should describe:

```text
Architecture
Responsibilities
Dependencies
Interfaces
Design Decisions
```

---

# Principle 3 — Reproduction Guide

Every component must include instructions that allow another person to reproduce the work.

A reproduction guide should explain:

```text
Required Hardware
Required Software
Build Procedure
Installation Procedure
Execution Procedure
Validation Procedure
```

The objective is:

```text
A developer unfamiliar with the project
should be able to reproduce the result.
```

---

# Principle 4 — Validation Evidence

Every significant milestone should provide evidence that the implementation was validated.

Examples:

```text
Serial Logs
Screenshots
Photos
Videos
Measured Outputs
Architecture Reports
Validation Reports
```

Evidence should demonstrate that the component was tested on real hardware whenever possible.

---

# Standard README Expectations

Every major component should provide a README.

At minimum, the README should answer five questions:

```text
1. What is it?
2. Why does it exist?
3. What hardware is required?
4. How do I build or run it?
5. How do I validate it?
```

If these five questions are answered, a reviewer can usually understand and reproduce the component.

---

# Repository Philosophy

Ambient Physical AI is designed to be:

```text
Open
Documented
Reproducible
Engineering-Oriented
```

The repository should not depend on undocumented knowledge contained only in conversations, presentations or personal notes.

Knowledge should be progressively transferred into the repository.

---

# Competition Philosophy

A competition entry should not merely demonstrate functionality.

It should demonstrate:

```text
Architecture
Engineering Quality
Documentation
Reproducibility
```

The project should allow a judge, researcher or developer to:

```text
Clone the repository
Read the documentation
Assemble the hardware
Build the software
Execute the system
Observe the expected behavior
```

without requiring direct assistance from the original authors.

---

# Application Across the Project

These principles apply to:

```text
firmware/
runtime/
hardware/
docs/
tools/
scripts/
```

and to all future project components.

---

# Engineering Completion Rule

A milestone should only be considered complete when the following checklist is satisfied:

```text
Source Code .............. Available
Documentation ............ Available
Reproduction Guide ....... Available
Validation Evidence ...... Available
```

Only then should the milestone be classified as:

```text
VALIDATED
```

---

# Guiding Statement

```text
Code demonstrates that something works.

Documentation explains why it works.

Reproduction proves that others can make it work.

Validation demonstrates that it was actually tested.
```

---

# Conclusion

Ambient Physical AI is intended to be more than a collection of prototypes.

It is intended to be a reproducible engineering project.

For this reason, every major component should follow:

```text
Code
+
Documentation
+
Reproduction Guide
+
Validation Evidence
```

as a permanent engineering principle of the project.
