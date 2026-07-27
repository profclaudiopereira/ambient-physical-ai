# TECHNICAL_NOTE_RGB_STRIP_NODE_PROFILE_LIGHTING.md

# Ambient Physical AI

## Technical Note

### RGB Strip Node — Profile-Based Ambient Lighting Integration

Version: 1.0

Date: July 2026

---

# Purpose

This note documents the engineering work performed to evolve the RGB Strip Node from a generic semantic event consumer into a profile-aware ambient lighting node.

The objective was **not** to transform the RGB strip into a user identification indicator, but to use it as an **ambient expression device**, applying a predefined lighting profile after successful identity authentication.

This document is intended to support future README updates and maintenance activities.

---

# Initial Behavior

Originally the RGB Strip Node reacted only to semantic events.

Example:

```text
identity_authenticated
```

Regardless of which user authenticated, the node always executed:

```text
rgb_effects_identity_authenticated()
```

which produced a fixed blue illumination.

No user profile information reached the node.

---

# Architectural Evolution

The implementation introduced a complete profile propagation path.

Current execution flow:

```text
Identity Node
        │
        ▼
Identity Package
        │
        ▼
AX630C Cognitive Runtime
        │
        ▼
Semantic Dispatcher
        │
        ▼
RGB Strip Notifier
        │
        ▼
Semantic Event
        │
        ▼
RGB Strip Node
        │
        ▼
Semantic Consumer
        │
        ▼
Expression Processor
        │
        ▼
Profile Lighting
```

---

# Cognitive Runtime Modification (AX630C)

One Cognitive Runtime component was modified.

File:

```text
runtime/cognitive/stackflow/rgb_strip_notifier.py
```

The notifier originally transmitted only:

```json
{
    "type":"semantic_event",
    "event":"identity_authenticated",
    "target":"rgb_strip"
}
```

It now forwards the authenticated profile inside the payload:

```json
{
    "type":"semantic_event",
    "event":"identity_authenticated",
    "target":"rgb_strip",
    "payload":
    {
        "user_id":"mariana"
    }
}
```

This modification preserves backward compatibility.

Older Expression Nodes simply ignore the additional payload.

---

# RGB Strip Node Modifications

Several firmware modules were updated.

## expression_processor.h

A semantic context structure was introduced.

```text
semantic_context_t
```

Current information:

- authenticated user id

Future extensions may include:

- accessibility preferences
- preferred brightness
- preferred animation
- environmental preferences

without changing the communication protocol.

---

## semantic_consumer.c

The Semantic Consumer now extracts the optional payload.

Example:

```json
payload
    └── user_id
```

The parsed information is stored inside:

```text
semantic_context_t
```

and forwarded to the Expression Processor.

---

## expression_processor.c

The Expression Processor now receives:

```text
event_type
+
semantic_context
```

instead of only the semantic event.

When the event is:

```text
identity_authenticated
```

the processor selects the appropriate ambient lighting profile according to:

```text
user_id
```

instead of using one fixed illumination.

---

## rgb_effects.c

New profile-specific lighting functions were added.

The implementation follows the approved demonstration specification.

---

# Ambient Lighting Profiles

The RGB Strip is used as ambient lighting rather than as an identity indicator.

The approved lighting profiles are:

| Profile | Visual Temperature | Intensity | Predominant Color | Purpose |
|----------|-------------------|-----------|-------------------|---------|
| Claudio | Neutral-Cool | Low | Soft Blue | Concentration and software development |
| Hermínio | Warm | Medium | Amber / Yellow | Meetings and technical discussions |
| Mariana | Neutral | Medium-High | Warm White | Study and demonstrations |
| Visitor | Neutral | Medium | Soft Cyan (or Soft Green) | Welcome lighting during demonstrations |

---

# System States

The node also implements lighting states independent of authenticated users.

| State | Lighting |
|--------|----------|
| Idle | Very soft blue (low brightness) |
| Presence Detected | Soft yellow while waiting for authentication |
| Error | Persistent red with limited brightness |

---

# Expected Runtime Sequence

```text
Presence Detected
        │
        ▼
Neutral approach lighting
        │
        ▼
Identity Authenticated
        │
        ▼
Apply user lighting profile
        │
        ▼
Maintain ambient lighting
```

The selected profile remains active until:

- another user authenticates;

or

- the environment returns to the unoccupied state.

---

# Engineering Principles Preserved

The implementation intentionally preserves:

- semantic-event architecture;

- backward compatibility;

- separation between Cognitive Runtime and Expression Nodes;

- low coupling;

- modular expression processing;

- future extensibility.

No communication protocol was broken.

No existing semantic events were modified.

Only optional payload information was added.

---

# Files Modified

## Cognitive Runtime

```text
runtime/
└── cognitive/
    └── stackflow/
        └── rgb_strip_notifier.py
```

---

## RGB Strip Node

```text
main/
│
├── expression_processor.h
├── expression_processor.c
├── semantic_consumer.c
├── rgb_effects.h
└── rgb_effects.c
```

---

# Result

The RGB Strip Node now supports profile-aware ambient lighting.

The Cognitive Runtime provides the authenticated user identifier.

The Expression Node interprets this information locally and applies the predefined ambient lighting profile while preserving the project's modular architecture and semantic event model.