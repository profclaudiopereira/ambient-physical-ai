# Ambient Physical AI

## System Architecture Specification

**Version:** 1.1\
**Part 05 --- Human Interaction**

------------------------------------------------------------------------

# 8. Human Interaction

## 8.1 Design Philosophy

Human Interaction is a first-class architectural domain of Ambient
Physical AI. Within the **Human-Centered Laboratory**, interaction is
designed to be natural, contextual and non-intrusive, allowing
technology to support research activities without unnecessarily
competing for the Active Researcher's attention.

Rather than relying on a single interface, the architecture distributes
interaction across complementary modalities, each optimized for a
specific role.

------------------------------------------------------------------------

## 8.2 Architectural Principle

The **Cognitive Runtime** owns all semantic decisions.

Human Interaction interfaces never determine system behavior
autonomously. Their responsibilities are to receive human intentions,
present system responses, express semantic state, cooperate according to
the Active Context and avoid competing for the Active Researcher's
attention.

------------------------------------------------------------------------

## 8.3 Research Assistant

### StackChan

StackChan serves as the personal research assistant for the **Active
Researcher**.

Its responsibilities include technical discussions, laboratory guidance,
research support, context-aware conversations and access to external
knowledge through the Cognitive Runtime when appropriate.

StackChan does not control the laboratory. Depending on the Active
Context, the Cognitive Runtime may dynamically adjust its behavior,
including Silent Mode during classroom activities.

------------------------------------------------------------------------

## 8.4 Voice Interface

### Voice Pyramid + AtomS3R

Responsibilities:

-   Wake-word detection
-   Speech acquisition
-   User Intent generation
-   Spoken feedback
-   Bidirectional communication with the Cognitive Runtime

Voice commands represent semantic requests. Validation remains the
responsibility of the Cognitive Runtime.

------------------------------------------------------------------------

## 8.5 Physical Interaction

### M5Dial

Responsibilities:

-   NFC researcher identification
-   Profile management
-   Active Context selection
-   Touch and encoder interaction

Every requested context transition is validated before becoming part of
the global semantic state.

------------------------------------------------------------------------

## 8.6 Haptic Interface *(Future Evolution)*

Future wearable devices will provide silent notifications, confirmation
pulses, attention requests and Adaptive Accessibility support.

------------------------------------------------------------------------

## 8.7 Visual Expression

Distributed RGB-based **Expression Layer** nodes communicate semantic
information through coordinated light patterns such as Ready, Welcome,
Thinking, Attention, Warning and Error.

The objective is semantic communication rather than illumination.

------------------------------------------------------------------------

## 8.8 Attention Management

``` text
Active Context = Classroom

↓

StackChan → Silent

Voice Pyramid → Wake Word Only

Ambient Runtime → Classroom Dashboard

Expression Layer → Classroom Pattern
```

The Active Context coordinates every interaction channel while
minimizing unnecessary interruptions.

------------------------------------------------------------------------

## 8.9 Engineering Benefits

-   Multimodal interaction
-   Complementary interfaces
-   Adaptive Accessibility
-   Context-aware behavior
-   Reproducibility
-   Researcher-centered interaction

New interaction interfaces may be incorporated without redesigning the
overall architecture.

------------------------------------------------------------------------

**End of Part 05 (Version 1.1 -- Editorial Edition)**
