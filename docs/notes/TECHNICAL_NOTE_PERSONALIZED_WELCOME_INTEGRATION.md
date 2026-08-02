# TECHNICAL NOTE

## Ambient Physical AI

### Echo Pyramid Personalized Welcome Integration

---

# Purpose

This Technical Note documents the engineering work that introduced personalized welcome support into the Expression Layer.

The implementation extends the communication protocol between the Cognitive Runtime and the Echo Pyramid Voice Node by allowing the authenticated user's name to be transmitted together with the welcome event.

The solution preserves complete backward compatibility with previous firmware revisions while providing a richer and more personalized user interaction.

This document records the engineering rationale, protocol evolution, implementation changes, and validation performed during this milestone.

---

# Engineering Goal

## Previous Protocol

```text
WELCOME
```

## Extended Protocol

```text
WELCOME|<Authenticated User>
```

Examples:

```text
WELCOME|Mariana

WELCOME|Hermínio

WELCOME|Claudio
```

Legacy firmware compatibility is intentionally preserved.

The following command remains valid:

```text
WELCOME
```

---

# Engineering Motivation

Prior to this milestone, the Expression Layer could only react to a generic welcome event.

The new protocol enables the Cognitive Runtime to include the authenticated user's name while preserving the existing communication contract.

The resulting architecture allows future expression devices to personalize visual, audio or multimodal feedback without requiring changes to the authentication process itself.

---

# Cognitive Runtime Modifications

Updated component:

```text
runtime/cognitive/stackflow/echo_pyramid_adapter.py
```

Engineering improvements introduced:

- protocol abstraction;
- command builder refactoring;
- authenticated user extraction;
- parameter sanitization;
- backward compatibility preservation;
- improved engineering documentation.

---

# Semantic Event Processing

Earlier implementations searched multiple nested structures such as:

```text
payload.profile

payload.user

payload.identity
```

The validated Semantic Event currently exposes authenticated user information using dedicated fields.

Examples include:

```text
payload.user_name

payload.user_id

context.active_user
```

To maximize compatibility, the adapter supports multiple lookup locations, including:

```text
profile.name

profile.display_name

profile.id

payload.user_name

payload.user_id

context.active_user

context.user_name

context.user_id
```

This strategy allows interoperability across different stages of the project while preserving compatibility with previously generated semantic events.

---

# Communication Contract

## Previous

```text
WELCOME
```

---

## Current

```text
WELCOME|<Authenticated User>
```

Examples:

```text
WELCOME|Mariana

WELCOME|Hermínio

WELCOME|Professor
```

The extended command is fully backward compatible with the original protocol.

---

# Echo Pyramid Voice Node

Updated firmware:

```text
firmware/
└── nodes/
    └── expression-node/
        └── echo-pyramid-voice-node/
```

Firmware improvements include:

- extended UDP command parser;
- optional parameter parsing;
- authenticated user extraction;
- personalized welcome processing;
- preservation of legacy commands;
- editorial improvements and code documentation.

Supported commands:

```text
WELCOME

WELCOME|Mariana

WELCOME|Hermínio

WELCOME|Professor
```

---

# End-to-End Validation

Validated communication pipeline:

```text
Identity Node
        │
        ▼
Identity Package
        │
        ▼
Context Builder
        │
        ▼
Semantic Event Generator
        │
        ▼
Semantic Dispatcher
        │
        ▼
Echo Pyramid Adapter
        │
        ▼
UDP Transport
        │
        ▼
Echo Pyramid Voice Node
        │
        ▼
Command Parser
        │
        ▼
Expression Processor
        │
        ▼
Personalized Welcome
```

Observed firmware output:

```text
UDP message received:
WELCOME|Mariana

Personalized welcome event received

Parameter:
Mariana

Authenticated user:
Mariana

Expression action:
RGB_WELCOME
```

Validation Result:

```text
PASS
```

---

# Engineering Impact

The Personalized Welcome protocol demonstrates that user-specific information can be transported from the Cognitive Runtime to the Expression Layer without modifying the authentication architecture.

Identity ownership remains entirely under the Cognitive Runtime.

The Echo Pyramid Voice Node simply consumes the validated event and performs the corresponding local expression.

This preserves the architectural separation between:

- identity;
- cognition;
- expression.

---

# Current Status

## Validated

```text
✓ Personalized welcome transport

✓ Authenticated user parameter extraction

✓ UTF-8 user names

✓ Backward compatibility

✓ Expression Layer integration

✓ Runtime stability

✓ End-to-end communication
```

---

## Future Evolution

This milestone intentionally focuses only on transport and event processing.

Future engineering work may extend the personalized interaction through capabilities documented in their respective Technical Notes, including:

- personalized speech synthesis;
- expanded display feedback;
- richer multimodal expression.

These capabilities are not part of the validated scope of this milestone.

---

# Engineering Conclusion

The Personalized Welcome protocol has been successfully integrated into the Ambient Physical AI architecture.

The implementation validates the complete communication path between the Cognitive Runtime and the Echo Pyramid Voice Node while preserving compatibility with previous protocol revisions.

The resulting design maintains the architectural principles adopted throughout the project:

- implementation-driven engineering;
- clear responsibility boundaries;
- backward compatibility;
- modular runtime contracts;
- separation between cognition and expression.

---

# Validation Milestone

```text
PERSONALIZED_WELCOME_TRANSPORT_001

Status

VALIDATED
```