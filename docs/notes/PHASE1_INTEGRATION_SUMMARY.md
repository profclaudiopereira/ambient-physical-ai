# PHASE 1 INTEGRATION SUMMARY

## Ambient Physical AI

### Technical Handoff for MASTER and the Next System Integration Chat

---

## Purpose

This document explains the work completed during the Phase 1 System Integration mission. It was written so the MASTER and the next integration chat can understand:
- what changed in the M5Dial Identity Layer;
- what changed in the AX630C Cognitive Runtime;
- why those changes were necessary;
- which architectural decisions are now stable;
- which limitations remain;
- where the next integration mission must begin.
This is not a README. This is not the full historical archive. The exhaustive record remains:

```text
ENGINEERING_CLOSURE_PACKAGE_PHASE1.md
```

This document is the practical handoff.

---

# 1. Mission Objective

The original communication path already worked at transport level:

```text
M5Dial
  ↓ UDP
AX630C
```

The Phase 1 objective was to transform that path into a semantic integration pipeline:

```text
Identity
  ↓
Context
  ↓
Authorization
  ↓
Semantic Event
  ↓
Dispatcher
  ↓
Future Consumers
```

The mission was completed when real Identity Packages from the M5Dial were received by the AX630C, converted into context, normalized into authorization data, transformed into the canonical event `identity_authenticated`, and passed to the Semantic Dispatcher.

---

# 2. Initial State

Before Phase 1, the system already had:
- NFC card detection on the M5Dial;
- UID-to-profile mapping;
- context selection through the encoder;
- Identity Package generation;
- UDP transmission to port 4444;
- AX630C packet reception;
- an early Context Builder;
- initial StackChan notification experiments.
The effective path was:

```text
NFC Card
  ↓
M5Dial Profile
  ↓
Identity Package
  ↓
UDP
  ↓
AX630C Listener
  ↓
Context Builder
  ↓
Console Output
```

This proved transport connectivity, but several architectural gaps remained. The main gaps were:
1. firmware roles were not canonical;
2. authorization was not explicit;
3. event naming was not standardized;
4. downstream consumers had no stable contract;
5. StackChan logic risked being coupled directly to event creation;
6. real researcher profiles were inconsistent.

---

# 3. Core Architectural Problems

## 3.1 Source Roles Were Ambiguous

The firmware used roles such as:

```text
owner
learner
visitor
```

These were acceptable inside the profile table, but not sufficient for the entire runtime. For example, `owner` could mean owner of the device, system administrator, project owner, or researcher. The AX630C therefore needed to normalize source roles into canonical runtime roles.

## 3.2 Authorization Was Missing

A role name alone does not say exactly what the user may do. The runtime needed three separate concepts:

```text
source_role
role
access_level
capabilities
```

## 3.3 Event Names Could Proliferate

An early design could have created:

```text
researcher_authenticated
student_authenticated
visitor_authenticated
```

This was rejected. The event should describe what happened. The role should remain data inside the payload. The canonical event became:

```text
identity_authenticated
```

## 3.4 No Consumer Boundary

Without a dispatcher, the event generator could become coupled directly to StackChan or Tab5. A Semantic Dispatcher was therefore added between event generation and consumers.

---

# 4. Final Architecture

The validated Phase 1 architecture is:

```text
Presence Layer
  ↓
Identity Layer
  ↓
Identity Package
  ↓
UDP Port 4444
  ↓
AX630C Cognitive Runtime
  ↓
Context Builder
  ↓
Context Registry
  ↓
Authorization Normalization
  ↓
Semantic Event Generator
  ↓
Semantic Dispatcher
  ↓
Consumer Adapters
```

Current consumer state:

```text
StackChan Adapter → dry-run
```

Next consumer:

```text
Ambient Runtime / Tab5
```

The Phase 1 boundary ends at the dispatcher. The Phase 2 boundary begins at the Tab5 consumer.

---

# 5. Identity Layer — M5Dial

## 5.1 Main File

The firmware change occurred in:

```text
firmware/nodes/identity-node/main/main.cpp
```

The mission did not redesign the FreeRTOS architecture. It preserved:
- UI task;
- NFC task;
- event queue;
- I2C synchronization;
- profile manager;
- context selector;
- UDP sender.

## 5.2 Existing Responsibilities

The M5Dial remains responsible for:
- detecting the NFC card;
- reading the UID;
- resolving the profile;
- showing identity state;
- selecting context;
- generating the Identity Package;
- sending the package to AX630C.
It is not responsible for final authorization.

## 5.3 Profile Table Before the Change

Relevant profiles were conceptually:

```cpp
{"claudio", "Cláudio", "owner"},
{"mariana", "Mariana", "learner"},
{"herminio", "Hermínio", "learner"},
{"student", "Student", "learner"},
{"unknown", "Unknown", "visitor"},
```

The problem was that Mariana and Hermínio are researchers, not learners.

## 5.4 Profile Table After the Change

The corrected profiles became:

```cpp
{"claudio", "Cláudio", "owner"},
{"mariana", "Mariana", "researcher"},
{"herminio", "Hermínio", "researcher"},
{"student", "Student", "learner"},
{"unknown", "Unknown", "visitor"},
```

## 5.5 Why Cláudio Remained `owner`

Cláudio remained `owner` for backward compatibility. The AX630C converts:

```text
owner → researcher
```

This preserves the original source role while producing the correct canonical role.

## 5.6 Why Student Remained `learner`

The Student profile remained `learner`. The AX630C converts:

```text
learner → student
```

This avoids unnecessary firmware migration.

## 5.7 Identity Boundary

The final rule is:

```text
M5Dial identifies.
AX630C authorizes.
```

The M5Dial produces identity metadata. The AX630C produces canonical role, access level, and capabilities.

---

# 6. Identity Runtime Flow

The M5Dial runtime flow is:

```text
BOOT
  ↓
Initialize hardware
  ↓
Connect network
  ↓
IDLE
  ↓
Presence detected or user interaction
  ↓
Prompt NFC
  ↓
Wait for card
  ↓
Read UID
  ↓
Resolve profile
  ↓
Read current context
  ↓
Generate Identity Package
  ↓
Send UDP
```

## 6.1 NFC

Known UIDs include:

```text
8804DC32 → Claudio
88048667 → Student
8804EB36 → Hermínio
8804D225 → Mariana
```

The long-term strategy remains:

```text
If NDEF exists:
    use NDEF/JSON
Else:
    use UID Mapping
```

Full production NDEF support remains pending.

## 6.2 Context Selection

Available contexts include:

```text
Meeting
Demo
Classroom
Lab
```

The encoder selects the current context. The validated final tests mainly used:

```text
Lab
```

## 6.3 UI Behavior

The UI already supports:
- current identity;
- selected context;
- NFC waiting state;
- authentication feedback.
Pending UI work:
- show profile-specific image;
- replace the generic silhouette;
- revalidate the Presence-triggered NFC prompt in the full journey.

## 6.4 Presence Trigger

Expected flow:

```text
Presence Node
  ↓
presence_event
  ↓
M5Dial prompt
  ↓
Tap NFC card
```

Recent integration observations suggested that Presence transmission worked while M5Dial reception/logging required revalidation. That issue is separate from the completed M5Dial-to-AX630C semantic pipeline.

---

# 7. Identity Package Contract

Representative package:

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

Field meanings:
- `timestamp`: package generation time;
- `contract_version`: schema version;
- `type`: packet family;
- `profile`: identity metadata;
- `current_context`: selected context;
- `context`: compatibility field;
- `nfc`: physical card evidence;
- `source`: producer identifier.
Important limitation: The device timestamps observed during validation were not synchronized with the 2026 project date. Time synchronization remains pending.

---

# 8. Cognitive Runtime — AX630C

## 8.1 Runtime Location

Deployment path:

```text
/root/ambient-runtime/runtime/cognitive/stackflow
```

Execution:

```bash
cd /root/ambient-runtime/runtime/cognitive/stackflow
python3 identity_udp_listener.py
```

## 8.2 Files Changed or Added

```text
runtime/cognitive/stackflow/identity_udp_listener.py
runtime/cognitive/stackflow/stackchan_notifier.py
runtime/cognitive/stackflow/semantic_dispatcher.py
runtime/cognitive/stackflow/semantic_event.py
runtime/cognitive/stackflow/semantic_event_generator.py
```

Deployment tool:

```text
tools/deploy_runtime_ax630c.bat
```

---

# 9. `identity_udp_listener.py`

## 9.1 Before Phase 1

The listener mainly:
- opened UDP port 4444;
- received JSON;
- parsed the Identity Package;
- invoked basic context logic;
- printed results.

## 9.2 After Phase 1

The listener became the pipeline orchestrator:

```text
Receive packet
  ↓
Parse JSON
  ↓
Validate type
  ↓
Build context
  ↓
Update registry
  ↓
Normalize authorization
  ↓
Generate semantic event
  ↓
Dispatch event
  ↓
Report result
```

This is a major architectural evolution. The listener is no longer only a transport test.

## 9.3 Important Constraint

The listener should orchestrate components, not absorb all business logic. Future work must preserve separation between:
- network reception;
- context construction;
- authorization;
- event generation;
- dispatch;
- consumer adapters.

---

# 10. Context Builder

The Context Builder converts the Identity Package into a semantic 5W Context Package. Model:

```text
Who
Where
When
What
Why
```

Representative result:

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

The Context Builder is owned by the Cognitive Runtime. The M5Dial should not generate this structure directly.

---

# 11. Context Registry

The Context Registry stores the latest valid context. Current model:

```text
single active context
```

Lifecycle:

```text
New valid Identity Package
  ↓
New Context Package
  ↓
Replace current context
```

This is sufficient for the current single-user demo. It is not a complete session manager. It does not yet provide:
- persistence;
- history;
- session expiry;
- multiple users;
- conflict resolution;
- distributed synchronization.

---

# 12. Authorization Layer

The Authorization Layer is a central Phase 1 addition. It converts source identity data into canonical runtime permissions. Input example:

```text
source_role = owner
```

Output example:

```text
role = researcher
access_level = full_research
capabilities = [...]
```

## 12.1 Normalization

Validated mapping:

```text
owner      → researcher
researcher → researcher
learner    → student
student    → student
visitor    → visitor
```

## 12.2 Why Normalization Belongs on AX630C

Without central normalization, each consumer would need to interpret legacy roles independently. That would duplicate logic in:
- Tab5;
- StackChan;
- Expression Layer;
- future services.
The Cognitive Runtime is the correct owner of canonical authorization.

## 12.3 `source_role`

The original firmware role is preserved. Example:

```text
source_role = owner
role = researcher
```

This supports:
- backward compatibility;
- debugging;
- auditing;
- future migration.

## 12.4 `access_level`

Validated access levels:

```text
researcher → full_research
student    → limited_research
visitor    → ambient_only
```

Role and access level are intentionally separate.

## 12.5 `capabilities`

Validated researcher capabilities:

```text
ambient_runtime
expression_layer
research_equipment
cognitive_runtime
administration
```

Future consumers should preferably authorize by capability rather than only by role.

---

# 13. Canonical Semantic Event

The canonical event is:

```text
identity_authenticated
```

Meaning:

```text
A user identity was resolved and validated.
```

Rejected model:

```text
researcher_authenticated
student_authenticated
visitor_authenticated
```

Reason for rejection:
- event proliferation;
- duplicated handlers;
- role coupling;
- difficult future extension.
Chosen model:

```text
event_type = identity_authenticated
role = researcher
access_level = full_research
```

---

# 14. `semantic_event.py`

This file defines or supports the canonical Semantic Event contract. The contract contains:
- schema version;
- event ID;
- event type;
- target;
- priority;
- timestamp;
- source;
- payload;
- context projection.
It becomes the stable internal message for downstream consumers.

---

# 15. `semantic_event_generator.py`

This component receives validated context and authorization. Input:

```text
Context Package
Authorization data
```

Output:

```text
identity_authenticated
```

It must remain consumer-independent. It should not contain Tab5-specific or StackChan-specific behavior. Representative event:

```json
{
  "schema_version": "1.0",
  "type": "semantic_event",
  "event_id": "d252e8f1-59c3-417c-a185-b8530c026d4d",
  "event_type": "identity_authenticated",
  "target": "cognitive_runtime",
  "priority": "high",
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
  }
}
```

---

# 16. `semantic_dispatcher.py`

The Semantic Dispatcher separates generation from consumption. Its responsibilities are:
- accept the canonical event;
- identify applicable consumers;
- call consumer adapters;
- return dispatch results;
- prevent direct coupling.
Architecture:

```text
Semantic Event Generator
  ↓
Semantic Dispatcher
  ├── StackChan Adapter
  ├── Future Tab5 Adapter
  ├── Future Expression Adapter
  └── Future Services
```

Current status:

```text
Implemented
Validated in dry-run
```

---

# 17. `stackchan_notifier.py`

The notifier was adapted to prepare a StackChan notification around the canonical event. Representative wrapper:

```json
{
  "type": "stackchan_notification",
  "semantic_event": {
    "...": "canonical semantic event"
  },
  "source": "ax630c_cognitive_runtime"
}
```

Validated:

```text
Event received
Wrapper prepared
Dispatcher called
```

Not validated:

```text
Production transport
Context injection
StackChan reaction
```

Observed result:

```json
{
  "stackchan": false
}
```

This is expected in dry-run mode. It means delivery was not enabled. It does not mean the semantic pipeline failed.

---

# 18. Why StackChan Was Left in Dry-Run

Earlier investigation indicated that production StackChan integration should use a stable MCP/tool-provider path rather than an ad hoc direct notification mechanism. Premature direct integration would create:
- temporary transport code;
- coupling;
- rework;
- unclear ownership.
Therefore Phase 1 intentionally stopped at:

```text
StackChan notification prepared
```

The next chat must not describe StackChan as fully integrated.

---

# 19. Deployment Tool

The file:

```text
tools/deploy_runtime_ax630c.bat
```

was updated so the evolved runtime can be deployed to AX630C. This tool must include all new semantic runtime files. It belongs to deployment tooling and should remain in a separate commit from firmware and runtime logic.

---

# 20. Complete Runtime Sequence

```text
Person enters
  ↓
Presence Node detects person
  ↓
Presence event reaches M5Dial
  ↓
M5Dial prompts NFC
  ↓
User presents card
  ↓
UID is read
  ↓
Profile is resolved
  ↓
Context is selected
  ↓
Identity Package is generated
  ↓
UDP sent to AX630C:4444
  ↓
identity_udp_listener.py receives
  ↓
JSON is parsed
  ↓
Context Builder creates 5W context
  ↓
Context Registry updates
  ↓
Authorization normalizes role
  ↓
Access level is generated
  ↓
Capabilities are generated
  ↓
semantic_event_generator.py emits identity_authenticated
  ↓
semantic_dispatcher.py dispatches
  ↓
StackChan adapter runs in dry-run
  ↓
Phase 2 will connect Tab5
```

---

# 21. Validation Results

## 21.1 UDP Reception

```text
M5Dial → AX630C
```

Result:

```text
PASS
```

## 21.2 Context Builder

Result:

```text
PASS
```

## 21.3 Mariana

Input:

```text
role = researcher
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

## 21.4 Hermínio

Input:

```text
role = researcher
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

## 21.5 Cláudio Compatibility

Input:

```text
source_role = owner
```

Output:

```text
role = researcher
access_level = full_research
```

Result:

```text
PASS
```

## 21.6 Student Compatibility

Input:

```text
source_role = learner
```

Output:

```text
role = student
access_level = limited_research
```

Result:

```text
PASS
```

## 21.7 Visitor

Output:

```text
role = visitor
access_level = ambient_only
```

Result:

```text
PASS
```

## 21.8 Canonical Event

Expected:

```text
identity_authenticated
```

Result:

```text
PASS
```

## 21.9 Dispatcher

Dispatcher execution:

```text
PASS
```

StackChan production delivery:

```text
PENDING
```

---

# 22. Stable Decisions

The next integration chat must preserve:
1. `identity_authenticated` as the canonical event.
2. Authorization ownership on AX630C.
3. `owner → researcher`.
4. `learner → student`.
5. Preservation of `source_role`.
6. Explicit `access_level`.
7. Explicit `capabilities`.
8. Semantic Dispatcher as the consumer boundary.
9. Consumer-independent event generation.
10. Identity Node responsibility limited to identity acquisition.

---

# 23. Completed Work

Completed in Phase 1:
- M5Dial profile corrections;
- Identity Package reception;
- Context Builder integration;
- Context Registry baseline;
- role normalization;
- access-level generation;
- capability generation;
- canonical event generation;
- Semantic Dispatcher;
- StackChan dry-run adapter;
- AX630C deployment support;
- real hardware validation.

---

# 24. Pending Work

Pending:
- Presence-to-Identity final revalidation;
- Tab5 consumer;
- Ambient Runtime action mapping;
- Expression Layer integration;
- StackChan production integration;
- MCP production path;
- NDEF production validation;
- time synchronization;
- persistent sessions;
- multi-user context;
- secure transport;
- retries and acknowledgements;
- observability.

---

# 25. Known Limitations

## Time

Device timestamps were incorrect relative to the 2026 project timeline.

## Sessions

Only the latest active context is stored.

## Reliability

UDP has no acknowledgement or retry.

## Security

Packets are not signed or encrypted.

## Policy

Roles and capabilities are currently code-defined.

## StackChan

Delivery remains dry-run.

## Presence

The complete Presence-to-Identity journey needs revalidation.

---

# 26. Repository Impact

Modified:

```text
firmware/nodes/identity-node/main/main.cpp
runtime/cognitive/stackflow/identity_udp_listener.py
runtime/cognitive/stackflow/stackchan_notifier.py
tools/deploy_runtime_ax630c.bat
```

Added:

```text
runtime/cognitive/stackflow/semantic_dispatcher.py
runtime/cognitive/stackflow/semantic_event.py
runtime/cognitive/stackflow/semantic_event_generator.py
```

Do not version:

```text
firmware/nodes/identity-node/managed_components/
```

---

# 27. Recommended Commits

## Identity Layer

```text
fix(identity-node): align researcher profile roles
```

## Cognitive Runtime

```text
feat(stackflow): add semantic event generation and dispatch
```

## Deployment Tool

```text
chore(tools): update AX630C runtime deployment script
```

This three-commit structure preserves technical responsibility and rollback clarity.

---

# 28. Exact Phase 2 Starting Point

Phase 2 must begin here:

```text
Semantic Dispatcher
  ↓
Ambient Runtime / Tab5 Consumer
```

It must not restart Phase 1. It must not redesign:
- Identity Package;
- Context Builder;
- authorization;
- canonical event;
- dispatcher boundary.

---

# 29. Suggested Phase 2 Mission

Mission name:

```text
System Integration Engineer — Phase 2
Semantic Event to Ambient Runtime Integration
```

Primary objective:

```text
Deliver identity_authenticated
from AX630C Semantic Dispatcher
to Ambient Runtime / Tab5
and validate a real ambient response.
```

---

# 30. Phase 2 Milestones

## Milestone 1

Review the stable Phase 1 contracts.

## Milestone 2

Define the Tab5 consumer adapter.

## Milestone 3

Select and validate transport.

## Milestone 4

Receive `identity_authenticated` on Tab5.

## Milestone 5

Display active identity and authorization.

## Milestone 6

Map capabilities to ambient behavior.

## Milestone 7

Validate real hardware end-to-end.

## Milestone 8

Only after Tab5 stabilization, connect Expression Layer.

---

# 31. Guidance for the Next Chat

The next chat should:
- preserve the Phase 1 baseline;
- work in small milestones;
- validate on real hardware;
- avoid redesign;
- avoid unrelated features;
- maintain contract compatibility;
- document each result;
- commit by responsibility.
The required hardware journey is:

```text
M5Dial
  ↓
AX630C
  ↓
Tab5
```

---

# 32. Suggested Initial Prompt

```markdown
# SYSTEM INTEGRATION ENGINEER — PHASE 2

Continue the Ambient Physical AI integration from the validated Phase 1 baseline.

Do not redesign Phase 1.

Stable pipeline:

Identity Package
↓
Context Builder
↓
Authorization
↓
identity_authenticated
↓
Semantic Dispatcher

Mission:

Connect the Semantic Dispatcher to Ambient Runtime / Tab5.

Preserve:

- identity_authenticated;
- source_role;
- normalized role;
- access_level;
- capabilities;
- dispatcher boundary.

Review:

- PHASE1_INTEGRATION_SUMMARY.md
- ENGINEERING_CLOSURE_PACKAGE_PHASE1.md
- runtime/cognitive/stackflow/identity_udp_listener.py
- runtime/cognitive/stackflow/semantic_event.py
- runtime/cognitive/stackflow/semantic_event_generator.py
- runtime/cognitive/stackflow/semantic_dispatcher.py
- runtime/cognitive/stackflow/stackchan_notifier.py
- firmware/nodes/identity-node/main/main.cpp
- tools/deploy_runtime_ax630c.bat

First objective:

Validate a minimal Semantic Event reception path on Tab5 without changing the Phase 1 contracts.
```

---

# 33. Final Assessment

Phase 1 did not merely connect two devices. It established the semantic identity core of Ambient Physical AI. The system now distinguishes:

```text
Identity
Context
Authorization
Event
Dispatch
Consumption
```

This separation is the main architectural achievement. The next integration chat can now focus on environmental reaction instead of reopening identity semantics.

---

# 34. Final Handoff

Phase 1 is complete. Stable endpoint:

```text
Semantic Dispatcher
```

Next starting point:

```text
Ambient Runtime / Tab5 Consumer
```

The next chat must continue from this boundary and preserve the validated contracts described here.
