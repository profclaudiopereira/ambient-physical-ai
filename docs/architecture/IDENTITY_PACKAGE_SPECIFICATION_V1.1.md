# Identity Package Specification — Version 1.1

## Ambient Physical AI

### Identity Layer → Cognitive Runtime Semantic Contract

---

## 1. Purpose

This document defines the Version 1.1 semantic contract used by the Identity Layer to communicate a resolved identity and its initial context to the Cognitive Runtime.

The contract is produced by the Identity Node, currently implemented by the M5Dial, and consumed by the AX630C Cognitive Runtime.

The purpose of Version 1.1 is to evolve the existing `identity_package` without breaking the already validated Version 1 implementation.

Version 1.1 therefore follows an additive and backward-compatible migration strategy.

---

## 2. Architectural Position

```text
Presence Node
    ↓
Identity Node — M5Dial
    ↓
identity_package
    ↓
AX630C Cognitive Runtime
    ↓
Context Builder
    ↓
5W Context Package
    ↓
Context Registry
    ↓
StackFlow Semantic Services
    ↓
Ambient Runtime — Tab5
```

The Identity Node resolves the physical identity reference available locally.

In Version 1, the AX630C acts as the authoritative session coordinator. It validates, enriches and normalizes the received identity information before that information is exposed to the remaining runtime.

---

## 3. Compatibility Strategy

Version 1.1 does not remove the Version 1 field:

```json
"context": "Lab"
```

Instead, it adds explicit context semantics:

```json
"default_context": "Lab",
"current_context": "Lab",
"context_source": "profile_default"
```

During migration, consumers must support both formats.

### Context resolution rule

```text
current_context
    ↓
context
    ↓
Unknown
```

```python
effective_context = identity_package.get(
    "current_context",
    identity_package.get("context", "Unknown")
)
```

The legacy `context` field remains present during the transition so that existing consumers continue operating.

---

## 4. Version 1 Baseline

```json
{
  "type": "identity_package",
  "profile": {
    "id": "claudio",
    "name": "Claudio",
    "role": "owner"
  },
  "context": "Lab",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "8804DC32"
  },
  "source": "m5dial_identity_console_v1"
}
```

Version 1 provides one undifferentiated context field. It does not explicitly distinguish the profile default context, the currently active context, the context source or the identity-resolution mechanism.

---

## 5. Version 1.1 Contract

```json
{
  "type": "identity_package",
  "contract_version": "1.1",
  "profile": {
    "id": "claudio",
    "name": "Claudio",
    "role": "owner"
  },
  "context": "Lab",
  "default_context": "Lab",
  "current_context": "Lab",
  "context_source": "profile_default",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "8804DC32",
    "resolution_source": "uid_fallback"
  },
  "source": "m5dial_identity_console_v1"
}
```

---

## 6. Field Definitions

### 6.1 Root fields

| Field | Type | Requirement | Description |
|---|---:|---|---|
| `type` | string | Required | Must be `identity_package`. |
| `contract_version` | string | Recommended in V1.1 | Contract version emitted by the producer. |
| `profile` | object | Required | Identity information resolved locally by the Identity Node. |
| `context` | string | Transitional | Legacy effective-context field retained for Version 1 compatibility. |
| `default_context` | string | Required in V1.1 | Context associated with the profile before any runtime context change. |
| `current_context` | string | Required in V1.1 | Context believed active when the package is produced. |
| `context_source` | string | Required in V1.1 | Origin of the current context selection. |
| `nfc` | object | Required | NFC detection and identity-reference metadata. |
| `source` | string | Required | Logical producer identifier. |

### 6.2 `profile`

```json
"profile": {
  "id": "claudio",
  "name": "Claudio",
  "role": "owner"
}
```

| Field | Type | Requirement | Description |
|---|---:|---|---|
| `id` | string | Required | Stable logical profile identifier. |
| `name` | string | Required | Human-readable profile name. |
| `role` | string | Required | Role associated with the locally resolved profile. |

The profile sent by the M5Dial is a locally resolved profile. It may be validated or enriched by the AX630C Profile Registry.

The AX630C Profile Registry is the official profile authority for the runtime session.

### 6.3 Context fields

#### `context`

Legacy Version 1 field. It must contain the same value as `current_context` while compatibility is required.

#### `default_context`

The context assigned when an identity is initially resolved. The local value is a fallback. The AX630C may replace or enrich it using the official Profile Registry.

#### `current_context`

The effective context believed active when the Identity Package is generated. For an initial identity package:

```text
current_context = default_context
```

A later context change must use the dedicated context-change contract rather than representing a new identity event.

#### `context_source`

Initial Version 1.1 values:

```text
profile_default
identity_encoder
voice_pyramid
ax630c_registry
unknown
```

For the initial identity package produced immediately after NFC resolution:

```json
"context_source": "profile_default"
```

### 6.4 `nfc`

```json
"nfc": {
  "detected": true,
  "card_present": true,
  "uid": "8804DC32",
  "resolution_source": "uid_fallback"
}
```

| Field | Type | Requirement | Description |
|---|---:|---|---|
| `detected` | boolean | Required | Indicates that the NFC subsystem detected a usable identity interaction. |
| `card_present` | boolean | Required | Indicates whether the card is present at package-generation time. |
| `uid` | string | Required | UID read from the NFC card, or an empty string when unavailable. |
| `resolution_source` | string | Required in V1.1 | Mechanism used locally to resolve the identity. |

Initial values:

```text
ndef
uid_fallback
unknown
```

The current firmware does not yet implement NDEF resolution. Therefore its normal Version 1.1 value is:

```json
"resolution_source": "uid_fallback"
```

---

## 7. Identity Resolution Precedence

```text
AX630C Profile Registry
    ↓
NDEF profile information
    ↓
UID Mapping
    ↓
Unknown profile
```

This precedence describes the complete distributed resolution process.

The M5Dial sends what it can resolve locally. The AX630C then validates and enriches that information using the official Profile Registry.

### Local M5Dial behavior

Current implementation:

```text
UID Mapping
    ↓
Unknown profile
```

Future implementation:

```text
NDEF
    ↓
UID Mapping
    ↓
Unknown profile
```

### AX630C behavior

```text
Receive locally resolved identity
    ↓
Consult official Profile Registry
    ↓
Validate profile
    ↓
Enrich profile and default context
    ↓
Create or update the runtime session
```

---

## 8. Producer Responsibilities — M5Dial

The Identity Node is responsible for:

- detecting the NFC interaction;
- reading the available identity reference;
- resolving an initial local profile;
- assigning the local fallback default context;
- setting `current_context` equal to `default_context` for a new identity;
- identifying the local resolution mechanism;
- emitting a syntactically valid Identity Package;
- retaining the legacy `context` field during the compatibility period.

The Identity Node must not be treated as the authoritative session registry. It provides the best locally available identity information.

---

## 9. Consumer Responsibilities — AX630C

The AX630C Cognitive Runtime is responsible for:

- accepting Version 1 packages;
- accepting Version 1.1 packages;
- validating the message type;
- normalizing the effective context;
- consulting the official Profile Registry;
- validating or enriching the profile;
- determining the authoritative default context;
- creating or updating the active single-user session;
- transforming the Identity Package into a 5W Context Package;
- updating the Current Runtime Context;
- producing normalized semantic state for downstream consumers.

The AX630C must not reject a valid Version 1 package only because Version 1.1 fields are absent.

---

## 10. Consumer Normalization Rules

### 10.1 Version detection

A consumer may inspect:

```json
"contract_version": "1.1"
```

A package without `contract_version` must be treated as Version 1-compatible input.

### 10.2 Effective context

```python
current_context = payload.get(
    "current_context",
    payload.get("context", "Unknown")
)
```

### 10.3 Default context

```python
default_context = payload.get(
    "default_context",
    current_context
)
```

### 10.4 Context source

```python
context_source = payload.get(
    "context_source",
    "legacy_context"
)
```

### 10.5 Resolution source

```python
resolution_source = payload.get(
    "nfc", {}
).get(
    "resolution_source",
    "legacy_uid"
)
```

These fallback values allow Version 1 data to be represented explicitly inside the Cognitive Runtime without modifying the original input.

---

## 11. Unknown Identity Example

```json
{
  "type": "identity_package",
  "contract_version": "1.1",
  "profile": {
    "id": "unknown",
    "name": "Unknown",
    "role": "visitor"
  },
  "context": "Lab",
  "default_context": "Lab",
  "current_context": "Lab",
  "context_source": "profile_default",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "A1B2C3D4",
    "resolution_source": "unknown"
  },
  "source": "m5dial_identity_console_v1"
}
```

An unknown local profile may still be validated and mapped by the AX630C Profile Registry.

---

## 12. Version 1 Compatibility Example

Input:

```json
{
  "type": "identity_package",
  "profile": {
    "id": "student",
    "name": "Student",
    "role": "learner"
  },
  "context": "Demo",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "88048667"
  },
  "source": "m5dial_identity_console_v1"
}
```

Normalized interpretation:

```json
{
  "contract_version": "1.0-compatible",
  "default_context": "Demo",
  "current_context": "Demo",
  "context_source": "legacy_context",
  "resolution_source": "legacy_uid"
}
```

The original Version 1 package remains valid.

---

## 13. Version 1.1 Initial Identity Example

```json
{
  "type": "identity_package",
  "contract_version": "1.1",
  "profile": {
    "id": "student",
    "name": "Student",
    "role": "learner"
  },
  "context": "Lab",
  "default_context": "Lab",
  "current_context": "Lab",
  "context_source": "profile_default",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "88048667",
    "resolution_source": "uid_fallback"
  },
  "source": "m5dial_identity_console_v1"
}
```

---

## 14. Separation from Context Change

The Identity Package represents:

```text
Identity resolved
+
Initial context established
```

It must not be reused as the normal message for changing context during an active session.

A context change is a different semantic operation and will use `context_change_request`, followed by an AX630C response such as `context_change_confirmation`.

This preserves the distinction:

```text
Identity is stable
Context is dynamic
```

---

## 15. Downstream Boundary

Raw NFC implementation details belong to the Identity Layer.

The normalized semantic state sent toward the Tab5 should not require:

- UID interpretation;
- NDEF parsing;
- WS1850S knowledge;
- card-presence polling;
- local UID Mapping rules.

The Tab5 consumes normalized runtime semantics produced by the AX630C and StackFlow.

---

## 16. Migration Plan

### Stage 1 — Specification

Freeze this Version 1.1 contract.

### Stage 2 — Consumer-first compatibility

Update the AX630C runtime to accept and normalize Version 1 and Version 1.1.

### Stage 3 — Context Builder

Update the Context Builder to prefer `current_context` and fall back to `context`.

### Stage 4 — Identity producer

Update the M5Dial to emit Version 1.1 while retaining the legacy `context` field.

### Stage 5 — End-to-end validation

```text
M5Dial
↓
Identity Package V1.1
↓
AX630C
↓
5W Context Package
↓
Context Registry
↓
StackFlow
↓
Tab5
```

### Stage 6 — Future deprecation decision

After all consumers use `current_context`, decide whether the legacy `context` field remains indefinitely or is formally deprecated in a future major contract version.

No removal is authorized by this Version 1.1 specification.

---

## 17. Validation Criteria

Version 1.1 is considered valid when:

- the AX630C still accepts the existing Version 1 package;
- the AX630C accepts a Version 1.1 package;
- `current_context` takes precedence over `context`;
- a missing `default_context` falls back safely;
- a missing `context_source` does not break processing;
- a missing `resolution_source` does not break processing;
- the 5W Context Package is generated;
- the Context Registry is updated;
- existing semantic services continue functioning;
- the M5Dial-to-AX630C validated path remains operational throughout migration.

---

## 18. Status

```text
Specification: PROPOSED FOR IMPLEMENTATION
Compatibility model: ADDITIVE
Migration strategy: CONSUMER FIRST
Identity producer modification: PENDING
AX630C compatibility modification: PENDING
End-to-end validation: PENDING
```

---

## 19. Architectural Summary

```text
Identity Node resolves locally
        ↓
Identity Package V1.1
        ↓
AX630C validates and enriches
        ↓
Context Builder normalizes
        ↓
Context Registry stores runtime state
        ↓
StackFlow exposes semantic context
        ↓
Tab5 consumes normalized semantics
```

Version 1.1 evolves the Identity Package without invalidating the Version 1 baseline.

It preserves the validated implementation while introducing the explicit context and identity-resolution semantics required for the Ambient Physical AI session model.
