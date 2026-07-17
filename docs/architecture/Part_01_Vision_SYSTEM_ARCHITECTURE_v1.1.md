# Ambient Physical AI

## System Architecture Specification

**Version:** 1.1\
**Part 01 --- Vision**

> **Editorial Note:** This document is the revised editorial edition of
> Part 01. It preserves the validated architecture while strengthening
> its conceptual foundation, scientific context, and engineering
> consistency.

------------------------------------------------------------------------

# 1. Vision

## 1.1 Introduction

Computing has evolved through successive paradigms, from centralized
systems to personal computing, networked platforms, cloud services and,
more recently, Artificial Intelligence. Each stage expanded the
capabilities of digital systems, yet interaction has largely remained
centered on explicit human attention.

In 1991, Mark Weiser proposed a different direction in *The Computer for
the 21st Century*. Rather than making computers increasingly visible, he
envisioned computation becoming an unobtrusive part of everyday
environments. This perspective, later known as **Ubiquitous Computing**,
inspired decades of research in Context-Aware Computing, Ambient
Intelligence and pervasive systems.

Advances in embedded systems, Edge AI, multimodal interfaces and Large
Language Models now make it possible to revisit that vision from a
practical engineering perspective. Ambient Physical AI is presented as
one implementation of these mature concepts through a reproducible
distributed architecture.

Rather than concentrating intelligence within a single device, the
system distributes specialized responsibilities among cooperative nodes
that perceive presence, identify the Active Researcher, maintain the
Active Context, reason about semantic information and transform the
surrounding environment through coordinated multimodal interaction.

------------------------------------------------------------------------

## 1.2 Conceptual Foundation

Ambient Physical AI integrates established research areas into a
coherent engineering architecture.

``` text
Ubiquitous Computing
(Mark Weiser, 1991)

↓

Context-Aware Computing

↓

Ambient Intelligence

↓

Edge AI

↓

Large Language Models

↓

Distributed Cognitive Systems

↓

Ambient Physical AI
```

The project does not replace these research areas. Instead, it combines
their complementary contributions into a modular, reproducible and
Human-Centered Laboratory architecture.

------------------------------------------------------------------------

## 1.3 Vision Statement

> Build a **Distributed Cognitive Ecosystem** capable of perceiving an
> Active Researcher, understanding the Active Context, adapting the
> physical environment and interacting naturally through multiple
> complementary interfaces.

------------------------------------------------------------------------

## 1.4 Active Context

Ambient Physical AI extends traditional context awareness by maintaining
an **Active Context** shared across the ecosystem. The system reasons
about the current activity rather than reacting only to isolated sensor
values.

Typical contexts include:

-   Research
-   Laboratory
-   Meeting
-   Classroom
-   Demonstration

------------------------------------------------------------------------

## 1.5 Understanding Context

  Question     Architectural Responsibility
  ------------ ---------------------------------------
  **Who?**     Identity Layer (Active Researcher)
  **Where?**   Presence Layer
  **When?**    Temporal Context *(future evolution)*
  **What?**    Active Context
  **Why?**     Cognitive Runtime reasoning

Together these dimensions progressively transform physical observations
into semantic understanding.

------------------------------------------------------------------------

## 1.6 Human-Centered Laboratory

The reference scenario is an intelligent research laboratory in which
the environment becomes an active participant in research activities.

The architecture intentionally maintains one **Active Researcher**, one
**Active Context** and one **Attention Focus** at a time. This design
choice simplifies reasoning, preserves contextual consistency and
supports incremental evolution without increasing system complexity.

Accessibility is treated as an architectural principle. Future adaptive
interfaces---including multimodal interaction and color-aware
accessibility profiles---extend the same human-centered philosophy
without requiring architectural redesign.

------------------------------------------------------------------------

## 1.7 Architectural Vision

The information lifecycle adopted throughout the architecture is:

``` text
Sense

↓

Identify

↓

Understand

↓

Transform

↓

Express
```

This sequence provides the conceptual framework for every subsystem
described in the remaining parts of this specification.

------------------------------------------------------------------------

## 1.8 Engineering Principles

Ambient Physical AI is guided by the following principles:

-   Simplicity.
-   Modularity.
-   Separation of responsibilities.
-   Semantic communication.
-   Reproducibility.
-   Incremental evolution.
-   Human-centered design.
-   **Hardware evolution without system redesign.**

The final principle was experimentally validated during the evolution of
the Presence Layer from a VL53L0X-based implementation to an
HLK-LD2410C-based implementation while preserving the overall system
architecture.

------------------------------------------------------------------------

## 1.9 Engineering Contribution

The contribution of Ambient Physical AI is not the creation of a new
computing paradigm. Its contribution is the practical integration of
established concepts into a coherent engineering architecture capable of
supporting intelligent research environments through a Distributed
Cognitive Ecosystem.

------------------------------------------------------------------------

**End of Part 01 (Version 1.1 -- Editorial Edition)**
