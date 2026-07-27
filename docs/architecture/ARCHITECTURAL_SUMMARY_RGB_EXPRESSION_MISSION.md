# Ambient Physical AI

## Architectural Summary — RGB Expression Mission

### Scope

This document summarizes the architectural decisions established during the RGB Expression mission. It focuses exclusively on responsibilities, semantic contracts, runtime behavior, profile propagation, ambient-lighting coordination, and the final role of each Expression Layer device.

---

# 1. Cognitive Runtime

The Cognitive Runtime on the AX630C remains the system component responsible for converting interpreted context into semantic intent.

The main architectural change was the extension of the RGB expression path so that authenticated profile information is propagated together with the semantic event.

The Runtime now performs the following responsibilities:

1. receives the authenticated identity and current context;
2. determines the semantic state of the interaction;
3. generates a normalized semantic event;
4. includes the authenticated `user_id` when the event depends on profile;
5. routes the event through the appropriate Expression Layer adapter;
6. transmits semantic intent rather than hardware commands.

The new relevant semantic field is:

```json
{
  "payload": {
    "user_id": "profile_identifier"
  }
}
```

The Runtime does not decide RGB values, LED intensity, animation, or device-specific calibration.

Its responsibility ends at semantic meaning.

For profile-aware illumination, the Runtime communicates:

```text
Identity successfully authenticated
+
Authenticated profile identifier
+
Expression target
```

The Expression Layer then determines how that meaning is physically represented.

---

# 2. StackFlow

StackFlow continues to act as the distributed cognitive coordination fabric between cognition and physical expression.

## Semantic Dispatcher

The Semantic Dispatcher remains responsible for receiving normalized semantic events and forwarding them to registered adapters according to their target.

It does not interpret hardware behavior.

It does not calculate colors.

It does not contain profile-specific lighting rules.

Its architectural responsibility is:

```text
Semantic Event
        ↓
Target resolution
        ↓
Registered adapter
        ↓
Physical subsystem
```

## Adapters

The RGB expression adapter was extended to preserve and forward profile context.

Its responsibility is now:

```text
Receive normalized semantic event
        ↓
Preserve event type
        ↓
Preserve target
        ↓
Preserve payload.user_id
        ↓
Transmit to RGB Expression Nodes
```

The adapter remains transport-oriented and does not become a lighting engine.

## Semantic Contracts

The established semantic contract is:

```json
{
  "type": "semantic_event",
  "event": "identity_authenticated",
  "target": "rgb_strip",
  "payload": {
    "user_id": "claudio"
  }
}
```

Architecturally:

- `type` identifies the message class;
- `event` identifies semantic intent;
- `target` identifies the expression capability or routing destination;
- `payload.user_id` provides contextual identity information.

## Compatibility

Compatibility was preserved in three ways:

- the existing event type was retained;
- the existing RGB target was retained;
- the new profile information was added as an optional payload.

Events without profile data remain valid.

Nodes that do not use profile context can continue interpreting the event without architectural breakage.

---

# 3. Expression Layer

The Expression Layer is responsible for converting semantic intent into device-specific physical expression.

It does not perform cognition.

It does not authenticate users.

It does not decide system policy.

It interprets already-normalized semantic events.

## StickC Plus2 RGB Node

Official responsibility:

```text
Runtime State Expression Node
```

The StickC Plus2 represents the operational or cognitive state of the runtime.

Its role is to expose states such as:

- idle;
- waiting;
- processing;
- successful interaction;
- error;
- other future runtime states.

It is not the primary ambient-lighting device.

It communicates system state in a compact, local, device-oriented form.

## Atom Matrix RGB Node

Official responsibility:

```text
Ambient Lighting Node
```

The Atom Matrix interprets semantic events and authenticated profile context to produce localized ambient expression.

Its role is to:

- participate in environmental illumination;
- represent presence and authentication transitions;
- apply profile-aware ambient behavior;
- preserve the semantic meaning defined by the Runtime;
- use hardware-specific calibration.

It is one of the two coordinated Ambient Lighting Nodes.

## RGB Strip Node

Official responsibility:

```text
Ambient Lighting Node
```

The RGB Strip provides the broader environmental expression channel.

Its role is to:

- illuminate a larger physical area;
- reflect the same semantic state used by the Atom Matrix;
- apply authenticated profile preferences;
- reinforce presence, transition, identity, and error states;
- preserve its own hardware-specific calibration.

The RGB Strip and Atom Matrix share semantic meaning but do not need identical numerical output.

---

# 4. Semantic Events

The RGB architecture currently recognizes the following semantic states:

```text
boot
idle
presence_detected
identity_authenticated
processing
system_error
```

Primitive test events may also exist for direct color validation, but they are not the architectural language of the integrated system.

## Interpretation by the RGB Nodes

### StickC Plus2

Interprets semantic events primarily as runtime-state communication.

Example:

```text
processing
→ runtime is actively handling context
```

### Atom Matrix

Interprets semantic events as localized ambient expression.

Example:

```text
identity_authenticated + user_id
→ apply the corresponding ambient profile
```

### RGB Strip

Interprets semantic events as broad environmental expression.

Example:

```text
presence_detected
→ prepare the environment for interaction
```

## Semantic Intent versus Hardware Command

The Runtime does not send commands such as:

```text
set red to 10
set green to 8
set blue to 0
```

The Runtime sends:

```text
identity_authenticated
```

together with contextual data such as:

```text
user_id = herminio
```

The hardware node decides how that semantic meaning is rendered on its own platform.

This preserves hardware independence.

---

# 5. User Profiles

User profiles now participate in the architecture as contextual input to physical expression.

## Where the Profile Is Decided

The profile is established by the Identity Layer after successful authentication.

The identity mechanism resolves the physical credential into a logical user profile.

## Where the Profile Is Transported

The authenticated profile is carried through the Cognitive Runtime and StackFlow inside the semantic-event payload.

Conceptually:

```text
Authenticated identity
        ↓
Logical user profile
        ↓
payload.user_id
        ↓
Semantic Event
```

## Where the Profile Is Interpreted

The profile is interpreted inside the receiving Expression Node.

The node maps the logical identifier to a device-specific ambient expression.

## How It Influences Lighting

The profile does not directly define RGB numbers in the Runtime.

Instead, it selects a semantic lighting profile.

Example:

```text
claudio
→ neutral-cool, low-intensity ambient expression

herminio
→ warm, medium-intensity ambient expression

mariana
→ neutral, medium-high warm ambient expression

visitor
→ neutral, medium-intensity welcoming expression
```

Each hardware platform then applies its own calibrated implementation of that profile.

---

# 6. Ambient Lighting

Ambient Lighting is implemented as a distributed physical-expression capability composed of two coordinated nodes:

```text
Atom Matrix RGB Node
+
RGB Strip Node
```

Both consume the same semantic intent.

Both receive the same authenticated-profile context.

Both preserve the same conceptual lighting language.

However, each node renders that intent according to its physical role.

## Atom Matrix Role

The Atom Matrix provides:

- localized ambient indication;
- compact spatial expression;
- close-range visual reinforcement;
- hardware-specific profile calibration.

## RGB Strip Role

The RGB Strip provides:

- broader environmental illumination;
- room-scale visual reinforcement;
- stronger spatial continuity;
- hardware-specific profile calibration.

## How They Work Together

They are semantically synchronized, not electrically coupled.

The Runtime sends one semantic meaning.

Each node independently interprets it.

Conceptually:

```text
identity_authenticated
+
user_id = mariana
        ↓
Atom Matrix applies localized warm-white profile
        ↓
RGB Strip applies room-scale warm-white profile
```

The two outputs may use different internal RGB values while preserving the same perceived environmental intent.

## Future Use

The same architecture can support:

- smooth transitions;
- fade-in and fade-out;
- breathing effects;
- rhythm;
- context-sensitive intensity;
- accessibility profiles;
- event priority;
- multi-zone lighting;
- adaptive environmental behavior.

These future capabilities should remain inside the Expression Layer.

The Cognitive Runtime should continue sending semantic meaning only.

---

# 7. Runtime State Language

A visual language was established to represent the operational and cognitive condition of the system.

Examples include:

```text
Idle
Presence detected
Processing
Identity authenticated
Error
```

The official responsibility for the runtime-state visual language belongs to:

```text
StickC Plus2 RGB Node
```

Its architectural role is to communicate what the runtime is doing.

The Ambient Lighting Nodes may react to some of the same semantic events, but they do not replace the StickC Plus2 as the dedicated runtime-state indicator.

Their responsibilities are different:

```text
StickC Plus2
→ communicates runtime state

Atom Matrix
→ localized ambient expression

RGB Strip
→ environmental ambient expression
```

The same semantic event may therefore produce complementary outputs without merging device responsibilities.

---

# 8. Engineering Principles

The following principles were preserved throughout the mission:

1. **The Runtime sends semantic intent, not hardware commands.**

2. **Expression Nodes interpret semantic meaning locally.**

3. **Cognition remains centralized in the Cognitive Runtime.**

4. **Physical rendering remains distributed across Expression Nodes.**

5. **Identity is resolved before profile-aware expression.**

6. **Profile information is transported as contextual payload.**

7. **Profiles select semantic behavior, not raw RGB values.**

8. **Each hardware platform maintains its own calibration.**

9. **Semantic equivalence is more important than numerical equality.**

10. **Device responsibilities remain separated.**

11. **The StickC Plus2 owns runtime-state expression.**

12. **The Atom Matrix and RGB Strip own ambient-lighting expression.**

13. **The Semantic Dispatcher remains transport- and routing-oriented.**

14. **Adapters preserve semantic contracts rather than implementing cognition.**

15. **Backward compatibility is preserved through optional payload fields.**

16. **Unknown profiles use a deterministic fallback.**

17. **The architecture remains extensible to rhythm, animation, accessibility, and additional expression devices.**

18. **No device-specific optical rule is moved into the Cognitive Runtime.**

---

# 9. Final Architecture

```text
┌──────────────────────────────┐
│        Presence Layer        │
│ Detects physical presence    │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│         Identity Layer       │
│ Authenticates the person     │
│ Resolves logical profile     │
└──────────────┬───────────────┘
               │
               │ authenticated profile
               ▼
┌──────────────────────────────┐
│      Cognitive Runtime       │
│ Interprets context           │
│ Determines semantic intent   │
│ Attaches profile context     │
└──────────────┬───────────────┘
               │
               │ semantic_event
               │ event + target + payload.user_id
               ▼
┌──────────────────────────────┐
│          StackFlow           │
│ Semantic Dispatcher          │
│ Adapter routing              │
│ Contract preservation        │
└──────────────┬───────────────┘
               │
               ▼
┌─────────────────────────────────────────────────────────────┐
│                    Expression Layer                         │
│                                                             │
│  ┌───────────────────────┐                                  │
│  │ StickC Plus2 RGB Node │                                  │
│  │ Runtime State         │                                  │
│  │                       │                                  │
│  │ Communicates what the │                                  │
│  │ Cognitive Runtime is  │                                  │
│  │ currently doing       │                                  │
│  └───────────────────────┘                                  │
│                                                             │
│  ┌───────────────────────┐   ┌───────────────────────────┐  │
│  │ Atom Matrix RGB Node  │   │ RGB Strip Node            │  │
│  │ Ambient Lighting      │   │ Ambient Lighting          │  │
│  │                       │   │                           │  │
│  │ Localized expression  │   │ Environmental expression │  │
│  │ Profile-aware         │   │ Profile-aware             │  │
│  │ Platform-calibrated   │   │ Platform-calibrated       │  │
│  └───────────┬───────────┘   └─────────────┬─────────────┘  │
│              │                             │                │
│              └──────────────┬──────────────┘                │
│                             │                               │
│                             ▼                               │
│              Authenticated profile influences              │
│              the perceived ambient environment             │
└─────────────────────────────────────────────────────────────┘
```

The final architectural rule is:

```text
Presence establishes interaction opportunity.

Identity establishes who is present.

The Cognitive Runtime establishes semantic meaning.

StackFlow transports that meaning.

The Expression Layer materializes it.

The authenticated profile personalizes ambient lighting
without transferring hardware-specific decisions into cognition.
```
