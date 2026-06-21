# NFC Identity Resolution Strategy

## Status

Future Implementation Note

Important for Final Delivery

---

# Purpose

This note records the intended NFC identity resolution strategy for Ambient Physical AI.

The current validated implementation uses NFC UID mapping.

Future implementation should support both:

```text
NDEF Payload Reading
UID Mapping Fallback
```

---

# Current Validated Flow

```text
NTAG216
↓
UID Read
↓
UID Mapping
↓
Profile
↓
Identity Package
```

Example:

```text
8804DC32 → Claudio / owner
```

---

# Target Flow

The intended final behavior is:

```text
If NDEF payload exists:
    Use NDEF data

Else:
    Use UID Mapping
```

---

# Conceptual Flow

```text
NTAG216
↓
Read UID
↓
Try Read NDEF
↓
If NDEF valid:
      Generate Identity Package from NDEF
  Else:
      Resolve Profile using UID Mapping
↓
Identity Package
```

---

# Example NDEF Payload

```json
{
  "id": "claudio",
  "role": "owner",
  "mode": "developer"
}
```

---

# Example Identity Package

```json
{
  "type": "identity_package",
  "profile": {
    "id": "claudio",
    "role": "owner",
    "mode": "developer"
  },
  "source": "m5dial_identity_console_v1"
}
```

---

# Rationale

UID Mapping is simple, robust and already validated.

NDEF Payload Reading is more flexible and allows richer identity data directly on the card.

The fallback model provides both robustness and flexibility.

---

# Recommended Implementation Priority

For V1:

```text
UID Mapping
```

For V1.1 / V2:

```text
NDEF Payload Reading
+
UID Mapping Fallback
```

---

# Important Rule

Do not remove UID Mapping when NDEF support is added.

UID Mapping must remain as fallback behavior.

---
