# Ambient Physical AI  
# System Integration Engineer — Phase 1  
# Engineering Closure Package

**Document type:** Engineering knowledge preservation  
**Mission:** System Integration Engineer — Phase 1  
**Project:** Ambient Physical AI  
**Repository:** `ambient-physical-ai`  
**Primary branch during integration:** `feature/ambient-runtime-node`  
**Mission status:** COMPLETED  
**Purpose:** Preserve the complete engineering knowledge generated during Phase 1 so that future integration and documentation work can continue without relying on this conversation.

---

## Document Control

This package is intentionally **not** a README and is not written as end-user documentation.

It is an engineering closure artifact intended to preserve:

- architecture before and after integration;
- implementation decisions;
- contracts;
- runtime behavior;
- validation evidence;
- known limitations;
- repository impact;
- handoff constraints;
- lessons learned;
- future integration boundaries.

This document should be treated as the primary source of truth for the future documentation mission.

Where an exact repository path or filename could not be reconstructed with absolute certainty from the mission record, that uncertainty is explicitly marked. Future maintainers must verify those specific paths with:

```bash
git status
git diff --name-only
git log --stat
```

No README file should be modified as part of this closure package.

---

# 1. Executive Summary

## 1.1 Original Mission

The original mission of System Integration Engineer — Phase 1 was to establish and validate the first complete semantic integration path between the physical Identity Layer and the Cognitive Runtime of the Ambient Physical AI system.

The mission began after individual subsystems had already achieved isolated technical milestones:

- Presence Node could detect presence and emit presence events.
- Identity Node could detect NFC cards, map UIDs to profiles, and generate Identity Packages.
- AX630C could receive UDP packets.
- Context Builder existed as an early transformation component.
- StackChan/MCP research had demonstrated the feasibility of exposing semantic tools, but not production context delivery.
- Ambient Runtime and Expression Layer existed as separate workstreams.

The central Phase 1 engineering problem was therefore not basic hardware bring-up. The problem was to turn an isolated transport test into a coherent semantic runtime pipeline that could support future integration with Ambient Runtime, Tab5, StackChan, and expression devices.

## 1.2 Initial System State

At the beginning of the mission, the effective integration path was approximately:

```text
M5Dial Identity Node
        │
        ▼
Identity Package
        │
        ▼
UDP Port 4444
        │
        ▼
AX630C Listener
        │
        ▼
Human-readable output / early context transformation
```

This initial state had several limitations:

1. The AX630C listener primarily demonstrated transport reception.
2. Role semantics were still coupled to Identity Node labels such as:
   - `owner`
   - `learner`
   - `visitor`
3. There was no canonical authorization model.
4. There was no stable event contract for downstream consumers.
5. Role-specific semantic event naming had been considered, which would have caused event proliferation.
6. There was no validated dispatcher boundary.
7. StackChan integration was still only a prototype/dry-run path.
8. The connection to Ambient Runtime and Tab5 had not started.
9. The Identity Node profile table still contained inconsistent role assignments for real users.
10. The runtime had no formal distinction between:
    - identity acquisition;
    - semantic context;
    - authorization;
    - event generation;
    - event dispatch.

## 1.3 Final Validated System State

At the end of Phase 1, the validated integration path is:

```text
Presence Layer
        │
        ▼
Identity Node
        │
        ▼
Identity Package Contract V1.1
        │
        ▼
UDP Port 4444
        │
        ▼
AX630C Cognitive Runtime
        │
        ▼
Context Builder
        │
        ▼
5W Context Package
        │
        ▼
Authorization Normalization
        │
        ▼
Canonical Semantic Event Generator
        │
        ▼
Semantic Dispatcher
        │
        ▼
StackChan Adapter / Consumer Boundary
        │
        ▼
Dry-run result today
Real consumers in Phase 2
```

The final system was validated with real hardware and real user profiles.

Validated identities included:

- Cláudio
- Mariana
- Hermínio
- Student
- Visitor/Unknown

The runtime successfully generated the canonical event:

```text
identity_authenticated
```

The runtime also generated:

- canonical role;
- access level;
- capabilities;
- active user context;
- environment;
- activity;
- validation state.

The final validated authorization examples were:

```text
owner      → researcher → full_research
researcher → researcher → full_research
learner    → student    → limited_research
visitor    → visitor    → ambient_only
```

## 1.4 Exactly What Changed

Phase 1 changed the system from a transport-oriented listener into a semantic integration runtime.

The principal changes were:

1. Identity profiles were corrected to use canonical researcher roles for Mariana and Hermínio.
2. Legacy role compatibility was retained for existing profiles such as `owner` and `learner`.
3. The Cognitive Runtime gained a formal authorization normalization stage.
4. The canonical event model was standardized around `identity_authenticated`.
5. Role-specific event names were intentionally rejected.
6. Semantic events gained:
   - `source_role`
   - canonical `role`
   - `access_level`
   - `capabilities`
7. A Semantic Dispatcher boundary was implemented and validated.
8. StackChan notification preparation was validated without claiming real delivery.
9. The full pipeline was validated on the AX630C.
10. Phase 1 established a stable handoff point for Tab5/Ambient Runtime integration.

---

# 2. Scope

## 2.1 Inside the Mission

Phase 1 included:

- validation of Identity Node to AX630C UDP transport;
- Identity Package parsing;
- context generation;
- 5W Context Package construction;
- runtime role normalization;
- authorization data generation;
- canonical semantic event generation;
- semantic event dispatch preparation;
- StackChan notification object preparation;
- dry-run dispatch validation;
- real-user profile correction in M5Dial firmware;
- validation with multiple identity classes;
- preservation of backward compatibility;
- establishment of a stable contract for Phase 2.

## 2.2 Intentionally Outside the Mission

The following were explicitly outside the Phase 1 scope:

- redesigning Presence Node hardware;
- redesigning the Identity Node FreeRTOS architecture;
- replacing NFC UID mapping with full NDEF production support;
- production MCP WebSocket transport;
- actual StackChan context push;
- local Qwen inference;
- decision engine implementation;
- Ambient Runtime command execution;
- Tab5 integration;
- Expression Layer activation;
- MQTT integration;
- cloud services;
- database persistence;
- authentication security hardening;
- production session persistence;
- multi-user concurrent sessions;
- full environment orchestration;
- README editorial refactoring.

## 2.3 Official End of Phase 1

Phase 1 ends when:

1. The Identity Node emits a valid Identity Package.
2. The AX630C receives and parses it.
3. The Context Builder generates a Context Package.
4. The runtime normalizes authorization.
5. The runtime generates `identity_authenticated`.
6. The dispatcher prepares delivery.
7. The pipeline is validated with researcher, student, and visitor roles.
8. Real hardware validation is complete.

All of these conditions were satisfied.

## 2.4 Official Start of Phase 2

Phase 2 begins at the downstream consumer boundary.

The first Phase 2 target is:

```text
Semantic Event Dispatcher
        │
        ▼
Ambient Runtime / Tab5
```

Phase 2 must focus on consuming validated semantic events and producing actual ambient behavior.

---

# 3. Architecture Before Integration

## 3.1 Pre-Phase-1 Architecture

```text
NFC Tag
   │
   ▼
M5Dial + WS1850S
   │
   ▼
Static UID Mapping
   │
   ▼
Identity Package JSON
   │
   ▼
UDP
   │
   ▼
AX630C Listener
   │
   ▼
Console Output
```

The pre-integration architecture proved that packets could travel between nodes, but it did not yet define a robust semantic contract for downstream services.

## 3.2 Identity Layer Before Phase 1

The Identity Node already contained:

- M5Dial display;
- rotary encoder;
- touch interface;
- buzzer;
- WS1850S NFC reader;
- profile table;
- UID mapping;
- context selector;
- Identity Package generation;
- UDP transmission.

The profile model used embedded firmware roles.

Representative profiles:

```cpp
{"unknown", "Unknown", "visitor"},
{"claudio", "Cláudio", "owner"},
{"student", "Student", "learner"},
{"mariana", "Mariana", "learner"},
{"herminio", "Hermínio", "learner"},
```

The central limitation was that firmware role labels were being treated as if they were already canonical runtime authorization roles.

They were not.

## 3.3 Cognitive Runtime Before Phase 1

The AX630C runtime already had:

- `identity_udp_listener.py`
- `identity_udp_listener_baseline.py`
- `context_builder.py`
- `context_registry.py`
- `semantic_services.py`
- `semantic_mcp_tools.py`
- `stackchan_notifier.py`
- MCP laboratory scripts

However, the effective execution path was still incomplete.

The runtime could:

- receive packets;
- parse JSON;
- generate context;
- maintain current context;
- expose local services;
- prepare MCP-style tools.

It did not yet have a fully validated authorization/event/dispatch sequence.

## 3.4 Limitations of the Initial Architecture

### Role Coupling

The meaning of a user depended directly on firmware strings.

Example:

```text
owner
```

could mean:

- repository owner;
- laboratory administrator;
- researcher;
- system owner;
- privileged user.

This ambiguity was unacceptable for downstream decision logic.

### Event Proliferation Risk

A role-specific event approach would produce events such as:

```text
researcher_authenticated
student_authenticated
visitor_authenticated
technician_authenticated
```

This would duplicate behavior and tightly couple consumers to profile categories.

### No Stable Authorization Contract

The initial Identity Package did not explicitly provide:

- normalized role;
- access level;
- capabilities.

### No Clear Dispatch Boundary

The runtime did not have a clean point where events became ready for real consumers.

### StackChan Ambiguity

Early direct notification experiments existed, but MCP research showed that production integration should be tool-based or broker-mediated.

---

# 4. Final Architecture

## 4.1 Final Logical Architecture

```text
Physical World
     │
     ▼
Presence Node
     │ presence_event
     ▼
Identity Node
     │ identity_package
     ▼
Cognitive Runtime / AX630C
     │
     ├── UDP Listener
     ├── Contract Validation
     ├── Context Builder
     ├── Context Registry
     ├── Authorization Layer
     ├── Semantic Event Generator
     ├── Semantic Dispatcher
     ├── Local Semantic Services
     └── MCP/Consumer Adapters
              │
              ▼
       Phase 2 Consumers
       ├── Ambient Runtime / Tab5
       ├── StackChan
       └── Expression Layer
```

## 4.2 New Components

### Authorization Layer

Purpose:

- separate identity labels from runtime permissions;
- normalize legacy roles;
- derive access level;
- derive capabilities;
- provide stable authorization semantics.

### Semantic Event Generator

Purpose:

- transform context + authorization into canonical events;
- prevent role-specific event proliferation;
- generate stable downstream contracts.

### Semantic Dispatcher

Purpose:

- provide a consumer-independent distribution point;
- allow future multiple consumers;
- preserve current dry-run behavior;
- avoid coupling event generation to StackChan.

## 4.3 Canonical Event Decision

Chosen event:

```text
identity_authenticated
```

Why:

- authentication is the event;
- role is data;
- access level is data;
- capabilities are data;
- downstream behavior should inspect payload, not event name.

This prevents combinatorial event growth.

## 4.4 Separation of Concerns

Final responsibilities:

| Layer | Responsibility |
|---|---|
| Presence | Detect that a person is present. |
| Identity | Determine who the person is. |
| Context Builder | Organize identity/environment into semantic context. |
| Authorization | Determine allowed capability scope. |
| Semantic Event Generator | Produce canonical runtime events. |
| Dispatcher | Route events to consumers. |
| Ambient Runtime | Transform the environment. |
| Expression Layer | Express feedback to the user. |

---

# 5. Firmware Changes — Identity Layer (M5Dial)

## 5.1 Hardware Baseline

- M5Dial V1.1
- ESP32-S3
- WS1850S NFC reader
- Rotary encoder
- Touch display
- Buzzer
- Wi-Fi
- UDP transport

## 5.2 Existing FreeRTOS Architecture

The Identity Node uses a task-oriented architecture.

Validated task responsibilities include:

```text
UI Task
NFC Task
Identity Event Queue
I2C synchronization
Profile Manager
Context Selector
Identity Package Generator
UDP Sender
```

The mission did not revert to a monolithic architecture.

## 5.3 Profile Management Evolution

The profile table was corrected so that real researchers are represented consistently.

Before:

```cpp
{"mariana", "Mariana", "learner"},
{"herminio", "Hermínio", "learner"},
```

After:

```cpp
{"mariana", "Mariana", "researcher"},
{"herminio", "Hermínio", "researcher"},
```

Cláudio remained compatible through:

```cpp
{"claudio", "Cláudio", "owner"},
```

The runtime maps:

```text
owner → researcher
```

The student profile remained:

```cpp
{"student", "Student", "learner"},
```

The runtime maps:

```text
learner → student
```

## 5.4 Engineering Rationale

The Identity Node is responsible for identity description, not final authorization.

The firmware may provide source roles that reflect embedded configuration or historical naming.

The Cognitive Runtime owns canonical runtime authorization.

This allows:

- backward compatibility;
- firmware evolution;
- consistent downstream behavior;
- reduced coupling.

## 5.5 Context Management

The Identity Node exposes selectable contexts such as:

```text
Meeting
Demo
Classroom
Lab
```

The current context is selected using the encoder.

The selected context is included in the Identity Package.

Both:

```text
current_context
```

and the legacy field:

```text
context
```

may be present for compatibility.

The default laboratory context used in the final validation was:

```text
Lab
```

## 5.6 UI Behavior

Validated UI responsibilities include:

- show current identity state;
- show selected context;
- show NFC waiting state;
- show authentication/profile result;
- react to Presence event with a prompt;
- allow encoder context changes.

Known UI limitation:

- final authenticated user photo/profile image behavior remains pending;
- silhouette replacement with profile-specific visual identity was not completed in this mission.

## 5.7 Encoder Behavior

The rotary encoder is the context selector.

Expected behavior:

```text
Rotate encoder
      │
      ▼
Change current context
      │
      ▼
Update UI
      │
      ▼
Use selected context in next Identity Package
```

## 5.8 NFC Behavior

The NFC subsystem:

1. polls WS1850S;
2. detects a card;
3. reads UID;
4. resolves the UID through profile mapping;
5. generates identity state;
6. builds Identity Package;
7. transmits it to AX630C.

Known UIDs validated during the wider project include:

```text
8804DC32 → Claudio
88048667 → Student
8804EB36 → Hermínio
8804D225 → Mariana
```

The final project identity strategy remains dual-path:

```text
If NDEF exists:
    parse NDEF/JSON profile
Else:
    fall back to UID Mapping
```

Full production NDEF validation remains outside this Phase 1 closure.

## 5.9 Presence-to-Identity Runtime Behavior

Presence Layer integration already established the expected trigger behavior:

```text
Presence event received
        │
        ▼
Identity Node displays:
"Presence detected / Tap NFC card"
        │
        ▼
User presents NFC card
```

The prompt window was previously implemented around a short interval (approximately 5 seconds in the integration workstream).

Recent integration observations had indicated that Presence packets were sent but Dial reception/log display required revalidation. That issue belongs to the broader System Integration workstream and is not part of the semantic pipeline validation completed here.

## 5.10 Communication Changes

The Identity Node sends UDP Identity Packages to AX630C on:

```text
UDP Port 4444
```

Source identifier:

```text
m5dial_identity_console_v1
```

Contract version observed:

```text
1.1
```

## 5.11 Packet Generation

A typical packet contains:

- timestamp;
- contract version;
- profile;
- current context;
- legacy context;
- NFC state;
- UID;
- source.

The Identity Node does not generate canonical access levels.

That is intentionally owned by AX630C.

---

# 6. Firmware Changes — Cognitive Runtime (AX630C)

## 6.1 Runtime Platform

- AX630C + LLM Mate
- Ubuntu 22.04 LTS
- Python 3
- UDP listener
- local semantic runtime
- future MCP integration

Deployment path used during validation:

```text
/root/ambient-runtime/runtime/cognitive/stackflow
```

Execution:

```bash
cd /root/ambient-runtime/runtime/cognitive/stackflow
python3 identity_udp_listener.py
```

## 6.2 Runtime Modifications

Phase 1 completed the transition from transport validation to semantic runtime integration.

The listener now orchestrates:

```text
Receive
Parse
Validate
Build Context
Normalize Authorization
Generate Semantic Event
Prepare Dispatch
Report Result
```

## 6.3 Context Builder

The Context Builder generates a 5W Context Package.

Model:

```text
Who
Where
When
What
Why
```

Responsibilities:

- convert transport-specific input into semantic structure;
- preserve identity data;
- create runtime timestamps;
- separate event semantics from raw UDP payload;
- preserve debug message generation.

## 6.4 Context Registry

The Context Registry stores the latest validated context.

Current model:

```text
single active context
```

Lifecycle:

```text
new valid Identity Package
        │
        ▼
new Context Package
        │
        ▼
replace current runtime context
```

The current implementation is intentionally lightweight and in-memory.

It is not a persistent database.

## 6.5 Authorization Model

The authorization stage consumes the source role and produces:

- normalized role;
- access level;
- capabilities.

Validated researcher result:

```json
{
  "source_role": "researcher",
  "role": "researcher",
  "access_level": "full_research",
  "capabilities": [
    "ambient_runtime",
    "expression_layer",
    "research_equipment",
    "cognitive_runtime",
    "administration"
  ]
}
```

Validated student result:

```json
{
  "source_role": "learner",
  "role": "student",
  "access_level": "limited_research"
}
```

Validated visitor result:

```json
{
  "role": "visitor",
  "access_level": "ambient_only"
}
```

## 6.6 Semantic Event Architecture

Canonical event:

```text
identity_authenticated
```

Event responsibilities:

- identify the runtime occurrence;
- carry authorization data;
- carry active context;
- be independent from hardware-specific details;
- serve as input to future consumers.

## 6.7 Session Model

Phase 1 did not implement a durable multi-user session manager.

The effective session model is:

```text
latest validated identity becomes active runtime identity
```

A new valid identity replaces the current context.

This is suitable for the current single-user demo flow.

Future multi-user or concurrent occupancy models must not be assumed from Phase 1.

## 6.8 Semantic Dispatcher

The dispatcher accepts generated semantic events and prepares consumer delivery.

Current consumer:

```text
StackChan adapter
```

Current result:

```json
{
  "stackchan": false
}
```

This means:

- event generation succeeded;
- notification preparation succeeded;
- real StackChan delivery did not occur;
- no false claim of successful external reaction is permitted.

## 6.9 StackChan Notification Adapter

Generated wrapper:

```json
{
  "type": "stackchan_notification",
  "semantic_event": {},
  "source": "ax630c_cognitive_runtime"
}
```

Status:

```text
StackChan notification prepared: PASS
StackChan delivery: PENDING
StackChan reaction observed: PENDING
```

This boundary must remain explicit in future documentation.

---

# 7. Data Contracts

## 7.1 Identity Package Contract V1.1

### Example

```json
{
  "timestamp": "2023-08-22T07:30:25.997170",
  "contract_version": "1.1",
  "type": "identity_package",
  "profile": {
    "id": "mariana",
    "name": "Mariana",
    "role": "researcher"
  },
  "current_context": "Lab",
  "context": "Lab",
  "nfc": {
    "detected": true,
    "card_present": true,
    "uid": "8804D225"
  },
  "source": "m5dial_identity_console_v1"
}
```

### Field Ownership

| Field | Owner | Meaning |
|---|---|---|
| `timestamp` | Identity Node | Time of package generation. |
| `contract_version` | Identity Node contract | Package schema version. |
| `type` | Identity Node | Contract type. |
| `profile` | Identity Node | Resolved identity profile. |
| `current_context` | Identity Node | Current selected context. |
| `context` | Identity Node | Legacy compatibility field. |
| `nfc` | Identity Node | NFC evidence. |
| `source` | Identity Node | Producer identifier. |

### Lifecycle

```text
NFC detection
    ↓
Profile resolution
    ↓
Context capture
    ↓
Package generation
    ↓
UDP transmission
    ↓
AX630C processing
```

## 7.2 Context Package Contract

### Example

```json
{
  "schema_version": "1.0",
  "package_type": "context_package",
  "who": {
    "id": "mariana",
    "name": "Mariana",
    "role": "researcher",
    "identity_source": "nfc",
    "uid": "8804D225",
    "identified": true
  },
  "where": {
    "environment": "Lab",
    "presence_zone": null,
    "location_source": "identity_context"
  },
  "when": {
    "timestamp": "2023-08-21T23:30:25.997416+00:00",
    "presence_detected_at": null,
    "identity_confirmed_at": "2023-08-21T23:30:25.997416+00:00"
  },
  "what": {
    "activity": "identity_authentication",
    "state": "validated",
    "event_type": "identity_package"
  },
  "why": {
    "intent": null,
    "reasoning": "pending semantic reasoning",
    "confidence": 0.0
  },
  "source": {
    "identity_package_source": "m5dial_identity_console_v1",
    "context_builder": "context_builder_v1"
  }
}
```

### Ownership

The Context Package is owned by the Cognitive Runtime.

The Identity Node must not generate this package directly.

## 7.3 Semantic Event Contract

### Complete Example

```json
{
  "schema_version": "1.0",
  "type": "semantic_event",
  "event_id": "d252e8f1-59c3-417c-a185-b8530c026d4d",
  "event_type": "identity_authenticated",
  "target": "cognitive_runtime",
  "priority": "high",
  "timestamp": "2023-08-21T23:30:25.998302+00:00",
  "source": "semantic_event_generator_v1",
  "payload": {
    "user_id": "mariana",
    "user_name": "Mariana",
    "source_role": "researcher",
    "role": "researcher",
    "access_level": "full_research",
    "capabilities": [
      "ambient_runtime",
      "expression_layer",
      "research_equipment",
      "cognitive_runtime",
      "administration"
    ],
    "environment": "Lab",
    "activity": "identity_authentication",
    "state": "validated"
  },
  "context": {
    "active_user": "Mariana",
    "user_id": "mariana",
    "role": "researcher",
    "access_level": "full_research",
    "capabilities": [
      "ambient_runtime",
      "expression_layer",
      "research_equipment",
      "cognitive_runtime",
      "administration"
    ],
    "environment": "Lab",
    "activity": "identity_authentication",
    "state": "validated"
  }
}
```

### Field Explanation

| Field | Meaning |
|---|---|
| `schema_version` | Event schema version. |
| `type` | Contract family. |
| `event_id` | Unique event identifier. |
| `event_type` | Canonical semantic event name. |
| `target` | Intended runtime domain. |
| `priority` | Event importance. |
| `timestamp` | Runtime generation time. |
| `source` | Event generator identity. |
| `payload` | Event-specific semantic data. |
| `context` | Consumer-friendly active context projection. |

## 7.4 StackChan Notification Contract

```json
{
  "type": "stackchan_notification",
  "semantic_event": {
    "...": "canonical semantic event"
  },
  "source": "ax630c_cognitive_runtime"
}
```

Lifecycle:

```text
semantic event generated
        ↓
adapter wraps event
        ↓
dispatcher calls adapter
        ↓
dry-run result today
```

## 7.5 Presence Event Contract

Representative presence event from the wider integration baseline:

```json
{
  "type": "presence_event",
  "state": "PRESENT",
  "distance_mm": 1950,
  "source": "presence_node_v1"
}
```

Presence is not authentication.

It only triggers the identity interaction flow.

---

# 8. Runtime Sequence

## 8.1 Complete Intended Sequence

```text
Person enters environment
        │
        ▼
Presence Node detects presence
        │
        ▼
presence_event sent to Identity Node
        │
        ▼
Identity UI prompts for NFC
        │
        ▼
User presents NFC card
        │
        ▼
Identity Node resolves profile
        │
        ▼
Identity Package V1.1 generated
        │
        ▼
UDP packet sent to AX630C:4444
        │
        ▼
Identity UDP Listener parses packet
        │
        ▼
Context Builder generates 5W Context Package
        │
        ▼
Context Registry updates current runtime context
        │
        ▼
Authorization Layer normalizes role
        │
        ▼
access_level and capabilities generated
        │
        ▼
Semantic Event Generator emits identity_authenticated
        │
        ▼
Semantic Dispatcher prepares delivery
        │
        ├── StackChan adapter (dry-run)
        │
        └── Phase 2 Ambient Runtime / Tab5 consumer
        ▼
Ambient Runtime transforms environment
```

## 8.2 Important Boundary

The last step validated in Phase 1 is:

```text
Semantic Dispatcher prepares event delivery
```

The first new Phase 2 step is:

```text
Tab5 consumes semantic event and performs ambient action
```

---

# 9. Architectural Decisions

## Decision 1 — One Canonical Authentication Event

**Problem:** Role-specific events would proliferate.

**Alternatives:**

- `researcher_authenticated`
- `student_authenticated`
- `visitor_authenticated`
- one generic event

**Chosen solution:**

```text
identity_authenticated
```

**Rationale:** Authentication is the event; role is payload data.

**Future implication:** Consumers must inspect authorization fields.

---

## Decision 2 — Normalize Roles in Cognitive Runtime

**Problem:** Firmware roles included legacy naming.

**Alternatives:**

- force immediate firmware migration;
- let each consumer interpret roles;
- normalize centrally.

**Chosen solution:** Central runtime normalization.

**Rationale:** Maintains backward compatibility and reduces coupling.

---

## Decision 3 — Preserve `source_role`

**Problem:** Canonicalization could hide original identity metadata.

**Chosen solution:** Preserve both:

```text
source_role
role
```

**Rationale:** Supports debugging, auditing, migration, and compatibility.

---

## Decision 4 — Separate Access Level from Role

**Problem:** A role name alone is not sufficient for authorization.

**Chosen solution:** Add:

```text
access_level
```

**Rationale:** Downstream decisions can use explicit permission tiers.

---

## Decision 5 — Add Capabilities

**Problem:** Access level remains too coarse for future orchestration.

**Chosen solution:** Add capability arrays.

**Rationale:** Enables fine-grained consumer behavior.

---

## Decision 6 — Dispatcher Boundary Before Real Consumer Integration

**Problem:** Coupling event generation directly to StackChan or Tab5 would make the runtime brittle.

**Chosen solution:** Semantic Dispatcher.

**Rationale:** Supports multiple consumers and phased integration.

---

## Decision 7 — StackChan Dry-Run Only

**Problem:** Direct delivery had not been validated as production architecture.

**Alternatives:**

- claim direct delivery;
- implement ad hoc UDP;
- retain dry-run until MCP path is production-ready.

**Chosen solution:** Dry-run.

**Rationale:** Avoids false claims and architectural debt.

---

## Decision 8 — Correct Mariana and Hermínio Profiles

**Problem:** Real researchers were labeled as learners.

**Chosen solution:** Firmware profile update to `researcher`.

**Rationale:** Align source identity semantics with actual laboratory roles.

---

## Decision 9 — Preserve Cláudio `owner` Compatibility

**Problem:** Existing profile and legacy contract used `owner`.

**Chosen solution:** Keep runtime mapping:

```text
owner → researcher
```

**Rationale:** Compatibility without blocking canonical behavior.

---

## Decision 10 — Preserve Student `learner` Compatibility

**Problem:** Existing student profile used `learner`.

**Chosen solution:**

```text
learner → student
```

**Rationale:** Same compatibility principle.

---

# 10. Validation

## 10.1 Platforms

### Windows

Used for local development and pipeline validation.

Result:

```text
PASS
```

### AX630C

Used for real runtime validation.

Result:

```text
PASS
```

## 10.2 Real Hardware Path

```text
M5Dial
    ↓ UDP
AX630C
```

Result:

```text
PASS
```

## 10.3 Researcher — Hermínio

Input:

```json
{
  "id": "herminio",
  "name": "Hermínio",
  "role": "researcher"
}
```

Output:

```text
source_role = researcher
role = researcher
access_level = full_research
```

Capabilities:

```text
ambient_runtime
expression_layer
research_equipment
cognitive_runtime
administration
```

Result:

```text
PASS
```

## 10.4 Researcher — Mariana

Input:

```json
{
  "id": "mariana",
  "name": "Mariana",
  "role": "researcher"
}
```

Output:

```text
source_role = researcher
role = researcher
access_level = full_research
```

Result:

```text
PASS
```

## 10.5 Researcher — Cláudio Compatibility

Input:

```text
source_role = owner
```

Expected normalized output:

```text
role = researcher
access_level = full_research
```

Result:

```text
PASS
```

## 10.6 Student Compatibility

Input:

```text
source_role = learner
```

Expected output:

```text
role = student
access_level = limited_research
```

Result:

```text
PASS
```

## 10.7 Visitor

Expected output:

```text
role = visitor
access_level = ambient_only
```

Result:

```text
PASS
```

## 10.8 Dispatcher

Observed:

```json
{
  "stackchan": false
}
```

Interpretation:

- dispatcher executed;
- adapter prepared notification;
- real delivery not performed.

Result:

```text
PASS for dry-run behavior
PENDING for production delivery
```

## 10.9 Validation Summary

| Test | Result |
|---|---|
| UDP reception | PASS |
| JSON parsing | PASS |
| Context Builder | PASS |
| Context Package | PASS |
| Context Registry | PASS |
| Role normalization | PASS |
| Access level generation | PASS |
| Capability generation | PASS |
| Canonical event generation | PASS |
| Dispatcher invocation | PASS |
| StackChan notification preparation | PASS |
| StackChan real reaction | PENDING |
| Tab5 consumption | NOT STARTED |

---

# 11. Known Limitations

1. StackChan delivery is not production-ready.
2. MCP production transport is not integrated into the semantic dispatcher.
3. Context push was not validated.
4. Tab5 has not consumed semantic events.
5. Ambient transformations are not connected.
6. Expression Layer is not connected to the canonical event pipeline.
7. Session model is single-current-context only.
8. No persistent context storage.
9. No cryptographic authentication of UDP packets.
10. No replay protection.
11. No schema validation library enforcement documented.
12. Device timestamps observed in logs were incorrect/out-of-date.
13. Presence timestamp fields were `null` in the validated Context Package.
14. Presence-to-Identity reception required revalidation in the broader integration flow.
15. Full NDEF/JSON profile path remains future work.
16. Multi-user presence is unsupported.
17. Error recovery and retry semantics for downstream consumers are not defined.
18. Capability policy is currently static.
19. Role policy is currently code-defined.
20. No central configuration service.
21. No integration test automation across hardware nodes.
22. No production observability/metrics.
23. No idempotency policy for semantic events.
24. No dead-letter queue.
25. No persistent event bus.

---

# 12. Phase 2 Handoff

## 12.1 Stable Components

Phase 2 may trust:

- Identity Package reception;
- Context Builder;
- Context Package structure;
- Context Registry baseline;
- role normalization;
- access level generation;
- capability generation;
- canonical event `identity_authenticated`;
- Semantic Dispatcher dry-run boundary.

## 12.2 Components That Must Not Be Redesigned Without Explicit Approval

Do not redesign:

1. canonical event naming;
2. authorization separation;
3. role normalization principle;
4. `source_role` preservation;
5. capability-based payload;
6. Identity → Context → Authorization → Event sequence;
7. dispatcher consumer boundary.

## 12.3 Development Resume Point

Resume at:

```text
Semantic Dispatcher
        │
        ▼
Ambient Runtime / Tab5 Consumer Adapter
```

## 12.4 Recommended Phase 2 Milestones

### Milestone 1

Define Tab5 consumer contract.

### Milestone 2

Deliver `identity_authenticated` to Tab5.

### Milestone 3

Display active profile and authorization state.

### Milestone 4

Map capabilities to ambient actions.

### Milestone 5

Connect Expression Layer.

### Milestone 6

Introduce reliable transport and retry policy.

---

# 13. Repository Impact

## 13.1 Confirmed Runtime Files

The following files are confirmed as part of the Cognitive Runtime subsystem and were created, evolved, or directly involved in Phase 1:

```text
runtime/cognitive/stackflow/identity_udp_listener.py
runtime/cognitive/stackflow/identity_udp_listener_baseline.py
runtime/cognitive/stackflow/context_builder.py
runtime/cognitive/stackflow/context_registry.py
runtime/cognitive/stackflow/semantic_services.py
runtime/cognitive/stackflow/semantic_mcp_tools.py
runtime/cognitive/stackflow/stackchan_notifier.py
```

The Phase 1 mission also used or introduced components corresponding to:

```text
semantic_event_generator.py
semantic_dispatcher.py
authorization layer/module
```

The exact repository filenames for the authorization and dispatcher implementation must be verified with Git history if they differ from these logical names.

## 13.2 Identity Firmware

The M5Dial firmware file containing:

```cpp
static const Profile profiles[]
```

was modified to change Mariana and Hermínio to `researcher`.

The exact source path must be confirmed with:

```bash
git diff --name-only
git log --stat
```

Do not modify:

```text
firmware/nodes/identity-node/managed_components/
```

## 13.3 Role of Each File

| File | Role |
|---|---|
| `identity_udp_listener.py` | Main runtime entry point and pipeline orchestrator. |
| `identity_udp_listener_baseline.py` | Preserved transport baseline. |
| `context_builder.py` | Builds semantic Context Package. |
| `context_registry.py` | Stores latest runtime context. |
| `semantic_services.py` | Exposes local semantic services. |
| `semantic_mcp_tools.py` | Maps services to MCP-style tools. |
| `stackchan_notifier.py` | Prepares StackChan notification object. |
| semantic event generator | Produces canonical events. |
| semantic dispatcher | Routes events to consumers. |
| identity firmware profile table | Defines embedded identity profiles. |

---

# 14. Recommended Commits

## 14.1 Preferred Single Functional Commit

Use when all runtime and firmware changes belong to one validated integration unit:

```text
feat(stackflow): finalize semantic authorization pipeline
```

Rationale:

- authorization;
- semantic event generation;
- dispatcher;
- real hardware validation;
- canonical contract.

## 14.2 Preferred Split Commits

### Commit 1 — Identity Firmware

```text
fix(identity-node): align researcher profile roles
```

Includes:

- Mariana role correction;
- Hermínio role correction.

### Commit 2 — Cognitive Runtime

```text
feat(stackflow): add canonical authorization and semantic dispatch
```

Includes:

- role normalization;
- access levels;
- capabilities;
- semantic event generator;
- dispatcher;
- notifier integration.

### Commit 3 — Closure Documentation

```text
docs(integration): add phase 1 engineering closure package
```

Includes only this closure document.

## 14.3 Verification Commands

```bash
git status
git diff --stat
git diff
```

Stage selectively:

```bash
git add <identity-firmware-file>
git commit -m "fix(identity-node): align researcher profile roles"
```

Then:

```bash
git add runtime/cognitive/stackflow/
git commit -m "feat(stackflow): add canonical authorization and semantic dispatch"
```

Then:

```bash
git add <closure-document-path>
git commit -m "docs(integration): add phase 1 engineering closure package"
```

Push:

```bash
git push origin feature/ambient-runtime-node
```

Capture evidence:

```bash
git log -3 --oneline
git status
```

Expected final state:

```text
nothing to commit, working tree clean
```

---

# 15. Lessons Learned

## 15.1 Transport Validation Is Not Semantic Integration

Receiving JSON over UDP does not mean a system is integrated.

A stable semantic layer requires:

- context;
- authorization;
- canonical events;
- consumer boundaries.

## 15.2 Roles Should Not Be Events

The event should describe what happened.

The role should describe who is involved.

This distinction simplified the architecture significantly.

## 15.3 Preserve Original and Canonical Values

Keeping both:

```text
source_role
role
```

was important for:

- debugging;
- migration;
- historical compatibility;
- engineering traceability.

## 15.4 Authorization Belongs in the Cognitive Runtime

The embedded Identity Node should identify.

It should not be the final authority on system capabilities.

## 15.5 Capabilities Are More Extensible Than Role Checks

A downstream consumer should eventually ask:

```text
Does this event include capability X?
```

rather than:

```text
Is this user role Y?
```

## 15.6 Dry-Run States Must Be Explicit

The logs correctly distinguished:

```text
notification prepared
delivery pending
reaction pending
```

This avoids overstating integration maturity.

## 15.7 Real Hardware Profiles Matter

The Mariana/Hermínio issue showed that test data and real laboratory identity semantics can diverge.

Profile tables must be treated as part of system architecture, not incidental demo data.

## 15.8 Device Time Cannot Be Trusted Yet

The AX630C/device logs showed timestamps in 2023 during a 2026 project timeline.

Time synchronization must be addressed before relying on event chronology.

## 15.9 Context Registry Is Useful but Not a Full Session Manager

The registry provides current state.

It does not provide:

- history;
- concurrency;
- session expiry;
- conflict resolution.

## 15.10 StackChan Integration Must Follow the Validated MCP Architecture

Direct notification injection was not the correct long-term architecture.

The MCP Tool Provider model remains the validated direction.

## 15.11 Documentation Must Separate Current Capability from Future Intent

Several components exist as prototypes.

Future README work must clearly label:

- validated;
- dry-run;
- suspended;
- future.

---

# 16. Appendix

## Appendix A — Canonical Role Matrix

| Source Role | Canonical Role | Access Level | Typical Capabilities |
|---|---|---|---|
| owner | researcher | full_research | ambient runtime, expression, research equipment, cognitive runtime, administration |
| researcher | researcher | full_research | ambient runtime, expression, research equipment, cognitive runtime, administration |
| technician | technician | technical_access | maintenance and technical capabilities, subject to final policy |
| learner | student | limited_research | restricted research and ambient interaction |
| student | student | limited_research | restricted research and ambient interaction |
| visitor | visitor | ambient_only | basic ambient interaction |

`technician` policy was architecturally anticipated but not fully validated in the final hardware logs.

## Appendix B — Identity State Machine

```text
BOOT
  │
  ▼
INITIALIZE UI / NFC / NETWORK
  │
  ▼
IDLE
  │
  ├── Presence event ──► PROMPT_NFC
  │                       │
  │                       ▼
  │                  WAIT_FOR_CARD
  │                       │
  │                       ▼
  │                  CARD_DETECTED
  │                       │
  │                       ▼
  │                  RESOLVE_PROFILE
  │                       │
  │                       ▼
  │                  BUILD_PACKAGE
  │                       │
  │                       ▼
  │                  SEND_UDP
  │                       │
  │                       ▼
  └──────────────────── AUTHENTICATED / RETURN_TO_IDLE
```

## Appendix C — Cognitive Runtime State Machine

```text
LISTENING
   │
   ▼
PACKET_RECEIVED
   │
   ▼
PARSE_JSON
   │
   ├── invalid ──► ERROR / DISCARD
   │
   ▼
BUILD_CONTEXT
   │
   ▼
UPDATE_REGISTRY
   │
   ▼
NORMALIZE_AUTHORIZATION
   │
   ▼
GENERATE_SEMANTIC_EVENT
   │
   ▼
DISPATCH
   │
   ├── StackChan dry-run
   └── Future Tab5 consumer
   │
   ▼
RETURN_TO_LISTENING
```

## Appendix D — Sequence Diagram

```text
Person       Presence       M5Dial        AX630C        Dispatcher       Tab5
  │             │              │             │               │             │
  │ enters      │              │             │               │             │
  ├────────────►│              │             │               │             │
  │             │ PRESENT      │             │               │             │
  │             ├─────────────►│             │               │             │
  │             │              │ prompt NFC  │               │             │
  │ tap card    │              │             │               │             │
  ├───────────────────────────►│             │               │             │
  │             │              │ IdentityPkg │               │             │
  │             │              ├────────────►│               │             │
  │             │              │             │ build context │             │
  │             │              │             │ authorize     │             │
  │             │              │             │ event         │             │
  │             │              │             ├──────────────►│             │
  │             │              │             │               │ Phase 2     │
  │             │              │             │               ├────────────►│
```

## Appendix E — Representative Logs

### Hermínio

```text
Profile: {'id': 'herminio', 'name': 'Hermínio', 'role': 'researcher'}
Current Context: Lab
UID: 8804EB36
```

Generated:

```text
event_type: identity_authenticated
source_role: researcher
role: researcher
access_level: full_research
```

### Mariana

```text
Profile: {'id': 'mariana', 'name': 'Mariana', 'role': 'researcher'}
Current Context: Lab
UID: 8804D225
```

Generated:

```text
event_type: identity_authenticated
source_role: researcher
role: researcher
access_level: full_research
```

### Dispatcher

```text
Semantic Event dispatch results:
{
  "stackchan": false
}

StackChan notification prepared: PASS
StackChan reaction observed: PENDING
```

## Appendix F — Engineering Notes for Future README Mission

The future editorial mission must not reduce this architecture to:

```text
M5Dial sends UDP to AX630C
```

That description would be technically incomplete.

The correct abstraction is:

```text
Identity acquisition
        ↓
Context construction
        ↓
Authorization normalization
        ↓
Canonical semantic event generation
        ↓
Consumer-independent dispatch
```

## Appendix G — Future Ideas

These are future directions, not current validated capability:

- Tab5 semantic event consumer;
- ambient transformation policy engine;
- capability-based action routing;
- MCP-backed StackChan semantic retrieval;
- Presence timestamps in Context Package;
- persistent event history;
- distributed context registry;
- technician policy validation;
- NDEF/JSON identity profiles;
- multi-user occupancy;
- secure transport;
- event acknowledgements;
- idempotent consumers;
- metrics and observability;
- schema registry;
- session expiration;
- policy configuration outside source code.

---

# Final Closure Statement

System Integration Engineer — Phase 1 successfully transformed the Ambient Physical AI identity path from a hardware transport demonstration into a validated semantic integration pipeline.

The following are now established engineering baselines:

```text
Identity Package Contract
Context Builder
5W Context Package
Context Registry
Authorization Normalization
Canonical Semantic Event
Semantic Dispatcher
Consumer Boundary
```

The mission is complete.

The next engineer should continue from the dispatcher-to-Tab5 boundary and must preserve the validated contracts and separation of responsibilities documented here.
