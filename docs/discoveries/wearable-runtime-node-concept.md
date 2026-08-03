> Status: Parking Lot Concept (Not Part of Current Critical Path)

# Wearable Runtime Node Concept

## Ambient Physical AI

### Status

Concept / Parking Lot

### Priority

Low for current implementation
High potential for demonstration impact

---

# Context

During the development of the Ambient Physical AI project, a possible wearable extension was identified using existing M5Stack hardware:

```text
StickS3
+
Watch / Wrist Strap Kit
+
Vibration HAT
```

This combination creates a compact wearable node capable of providing visual and haptic feedback directly to the user.

The concept emerged from the observation that Ambient Physical AI already supports environmental and conversational interactions, but could eventually extend its interaction model to include personal tactile feedback.

---

# Concept

The Wearable Runtime Node would act as a personal feedback channel within the Ambient Physical AI ecosystem.

Instead of interacting only through:

* displays;
* voice;
* StackChan;
* ambient lighting;

the system could also communicate directly with the user through haptic feedback.

---

# Architectural Role

```text
Wearable Runtime Node
=
Personal Ambient Feedback Node
```

Possible responsibilities:

* haptic notifications;
* contextual alerts;
* silent feedback;
* user attention cues;
* lightweight status display.

---

# Multimodal Interaction Value

The current Ambient Physical AI demonstration already explores:

```text
Vision
→ Display, OLED, Atom Matrix, StackChan
```

```text
Audio
→ Voice Pyramid, StackChan
```

The wearable node adds:

```text
Touch
→ Vibration / Haptic Feedback
```

This creates a stronger multimodal interaction model:

```text
Vision
+
Audio
+
Touch
```

---

# Example Use Cases

## User Recognized

```text
NFC identifies user
↓
AX630C processes context
↓
Wearable vibrates briefly
```

Meaning:

```text
The environment recognized me.
```

---

## Context Change

```text
Developer mode activated
↓
Wearable displays short status
↓
Vibration confirms transition
```

---

## Attention Cue

```text
Important ambient event detected
↓
Silent vibration on wrist
```

The user receives feedback without requiring visual or auditory attention.

---

# Implementation Simplicity

The first implementation can be intentionally simple.

## Minimal Firmware

```text
StickS3
↓
Wi-Fi
↓
Receives command
↓
Activates vibration motor
↓
Optionally updates display
```

Example command:

```json
{
  "action": "vibrate",
  "duration_ms": 300
}
```

Optional message command:

```json
{
  "action": "notify",
  "text": "Developer Mode"
}
```

---

# Possible Communication Options

Future communication may use:

* Wi-Fi;
* HTTP;
* MQTT;
* StackFlow events.

Initial implementation should avoid unnecessary complexity.

Recommended first version:

```text
HTTP or MQTT command
↓
Vibration feedback
```

---

# Competition Value

This node is not required for the V1 architecture.

However, if the main goals are achieved with schedule margin, the wearable node could add a strong demonstration effect.

It reinforces the idea that Ambient Physical AI is not limited to screens or voice.

The environment can also communicate through the body.

---

# Scope Decision

The Wearable Runtime Node must not enter the critical path now.

Current priority remains:

```text
Presence
↓
Identity
↓
Cognition
↓
Ambient Runtime
↓
Expression
```

The wearable node is eligible for inclusion in the competition V1 only if the core system is stable before the submission target.

---

# Previous Validation

An early prototype was successfully tested using:

* M5StickC Plus 2;
* Vibration HAT;
* M5Unified;
* IMU acceleration readings;
* local display feedback;
* haptic alert feedback.

The prototype implemented a simple state machine:

```text
STATE_CALM
↓
STATE_ALERT
```

The transition was based on IMU movement detection.

When movement exceeded a predefined threshold for a specified period, the device entered alert mode, changed the screen color and activated the vibration motor.

The prototype demonstrated that visual and haptic feedback can be combined effectively on a wearable M5Stack device.

This validation confirms that the wearable concept is technically feasible using hardware already available in the project inventory.

---

# Implementation Risk

The implementation risk is considered low because:

* the hardware is already available;
* display feedback has already been tested;
* IMU readings have already been tested;
* vibration feedback has already been tested;
* the remaining missing piece is primarily network integration.

The first network-enabled version could simply receive a command such as:

```json
{
  "action": "vibrate",
  "duration_ms": 300
}
```

and immediately trigger haptic feedback.

---

# MASTER Decision

```text
Status:
Approved as Parking Lot Concept

V1 Competition Candidate:
Yes, Conditional

Condition:
Only after the main Ambient Physical AI flow is operational.
```

---

# Key Message

The Wearable Runtime Node offers a high-impact, low-complexity path to introduce haptic interaction into Ambient Physical AI.

Unlike many future ideas that require new hardware, significant software development or architectural changes, this concept leverages hardware that already exists and has been partially validated.

For this reason, it represents a realistic candidate for future inclusion if schedule margin becomes available before the competition deadline.

It should be treated as a future enhancement and potential WOW factor, not as a required component of the current implementation roadmap.
