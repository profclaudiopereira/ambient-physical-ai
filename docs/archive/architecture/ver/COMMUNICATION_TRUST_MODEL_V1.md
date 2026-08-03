# COMMUNICATION_TRUST_MODEL_V1

## Ambient Physical AI

### Architectural Note

**Status:** Draft for V1

**Purpose:** Record the communication trust model adopted by Ambient Physical AI V1 and document future security directions for V2.

---

# Motivation

During the development of Ambient Physical AI, the project adopted UDP as the primary communication mechanism between distributed nodes.

This decision was intentional and aligned with the project's current objectives.

As the project approaches its first public demonstration, it becomes important to formally document:

- why UDP was chosen;
- the assumptions made by the architecture;
- the current security limitations;
- the realistic evolution path toward future versions.

This document records those decisions.

---

# Current Communication Model

The current distributed architecture is based on event-driven communication.

Example:

```text
Presence Node
        │
        ▼
UDP
        ▼
Identity Node
        │
        ▼
UDP
        ▼
Cognitive Runtime
        │
        ▼
Semantic Events
        │
────────┼────────────────────────
        │
        ▼
Expression Layer
```

Each node performs a specialized responsibility and exchanges lightweight semantic information through the local network.

---

# Why UDP Was Chosen

UDP was selected because it matches the engineering goals of Ambient Physical AI V1.

Advantages include:

- very low latency;
- minimal protocol overhead;
- simple implementation;
- excellent fit for embedded devices;
- event-oriented communication;
- loose coupling between nodes;
- easy scalability for additional devices.

The project currently exchanges relatively small semantic events, making UDP an appropriate transport mechanism.

---

# Trust Model

Ambient Physical AI V1 assumes a **Trusted Local Network**.

The system is designed to operate inside a controlled research laboratory where:

- all devices belong to the same administrative domain;
- nodes are known beforehand;
- external clients are not expected;
- communication occurs only inside the local network.

Under these assumptions, the use of plain UDP is considered acceptable for Version 1.

---

# Known Limitations

The current implementation intentionally does not provide:

- node authentication;
- message authentication;
- message integrity verification;
- encryption;
- replay protection;
- device identity validation.

Consequently, any device connected to the trusted local network could theoretically transmit forged UDP packets.

This limitation is acknowledged and accepted for the demonstration phase.

---

# Engineering Justification

The objective of Version 1 is to demonstrate:

- distributed cognition;
- semantic coordination;
- modular architecture;
- reproducible engineering.

The objective is **not** to demonstrate a production-grade secure distributed system.

Adding a complete security infrastructure during V1 would significantly increase complexity without contributing directly to the demonstration objectives.

For this reason, the current communication model represents an intentional engineering trade-off.

---

# Future Evolution (V2)

Security should evolve incrementally.

Rather than immediately adopting complex technologies, Version 2 should begin with a structured engineering study.

Recommended investigation order:

## Phase 1

Study node identity.

Questions:

- How should each node identify itself?
- How should trusted devices be registered?
- How should device identity be managed?

---

## Phase 2

Study message authenticity.

Possible topics:

- message signing;
- HMAC;
- integrity verification;
- replay protection.

The primary objective is ensuring that received messages originated from trusted nodes.

---

## Phase 3

Evaluate communication security alternatives.

Rather than selecting technologies prematurely, compare approaches based on engineering requirements.

Examples:

- Secure UDP (DTLS)
- MQTT
- MQTT with TLS
- Other suitable protocols

The decision should be driven by architectural responsibilities rather than protocol popularity.

---

# MQTT Considerations

MQTT is **not** currently planned for Version 2.

It is only considered a candidate technology.

Its adoption should occur only if it solves a concrete architectural problem better than UDP.

Potential motivations include:

- larger distributed deployments;
- topic-based event routing;
- Quality of Service (QoS);
- retained state;
- centralized monitoring;
- event subscription management.

If these requirements do not emerge, maintaining UDP with additional authentication mechanisms may remain the preferable solution.

---

# Architectural Principle

Protocol selection must always follow engineering requirements.

Never adopt a protocol simply because it is popular.

Instead, first identify the architectural responsibility, then select the simplest technology capable of fulfilling that responsibility.

---

# Version 1 Summary

Ambient Physical AI V1 intentionally adopts:

- UDP communication;
- trusted local network assumption;
- low-latency event distribution;
- minimal communication overhead.

This decision is considered appropriate for the project's current demonstration goals.

---

# Version 2 Summary

Future work should investigate:

- device identity;
- node authentication;
- message authenticity;
- integrity verification;
- replay protection;
- secure communication alternatives;
- protocol comparison based on engineering evidence.

The objective of Version 2 is not merely increasing security, but evolving the communication architecture while preserving the simplicity and modularity that characterize Ambient Physical AI.