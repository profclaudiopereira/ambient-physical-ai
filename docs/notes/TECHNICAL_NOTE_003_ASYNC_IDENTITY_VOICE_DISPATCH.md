# TECHNICAL NOTE 003 — Asynchronous Identity Voice Dispatch

## Ambient Physical AI

### Status

```text
Competition-time performance adjustment
Validated bottleneck: synchronous identity TTS execution
Architecture impact: localized
Post-competition review: required
```

---

## 1. Purpose

This note records a localized Cognitive Runtime performance adjustment applied
before the competition demonstration.

The objective is to remove the visible delay caused by personalized identity
voice generation without redesigning the runtime architecture, changing network
contracts or modifying the Echo Pyramid firmware.

---

## 2. Observed behavior

Repeated NFC authentication showed that the Cognitive Runtime remained in
`thinking` while the StackFlow TTS subprocess completed.

Representative laboratory traces showed identity cycles of approximately
10–11 seconds. The delay was not tied to one profile.

---

## 3. Root cause

`identity_udp_listener.py` invoked the StackFlow TTS client through blocking
`subprocess.run()` execution inside the main UDP processing loop.

The listener therefore waited for TTS configuration, synthesis, PCM reception,
WAV assembly, Echo Pyramid transfer and subprocess completion before continuing
Semantic Event processing and returning the Runtime State to `idle`.

---

## 4. Applied adjustment

The existing TTS client remains unchanged.

The listener now starts one daemon worker thread for personalized identity
voice. The worker continues using `subprocess.run()` internally, preserving
process cleanup, logs and exit-code validation, while the main UDP listener no
longer waits for TTS completion.

```text
Identity Package
        │
        ├── Ambient Context refresh
        ├── Semantic Event generation and dispatch
        ├── Runtime State responding / idle
        │
        └── background identity voice worker
                ↓
            StackFlow TTS
                ↓
            Echo Pyramid
```

---

## 5. Competition-time concurrency policy

Only one identity greeting may run at a time.

If another NFC identity is processed while a greeting is active, the newer
greeting is skipped instead of queued.

This policy prevents overlapping audio, stale greetings and queue growth. The
competition demonstration is expected to present one NFC card at a time with
controlled spacing.

---

## 6. Preserved behavior

The adjustment does not change Identity Package contracts, UDP ports, Context
Builder behavior, Context Registry behavior, Semantic Event V1 contracts,
Semantic Dispatcher adapters, the StackFlow TTS client, Echo Pyramid firmware,
Ambient Runtime behavior or profile-aware lighting.

---

## 7. Known limitation

A greeting can be skipped when authentications occur faster than TTS completion.

Post-competition alternatives include a latest-wins worker, explicit cancellation
of stale synthesis jobs, a bounded queue, sequence-aware suppression and detailed
TTS latency metrics.

---

## 8. Rollback

Before deployment:

```bash
cd /root/ambient-runtime/runtime/cognitive/stackflow
cp identity_udp_listener.py identity_udp_listener.py.backup-pre-async-tts
```

Rollback:

```bash
cd /root/ambient-runtime/runtime/cognitive/stackflow
cp identity_udp_listener.py.backup-pre-async-tts identity_udp_listener.py
python3 -m py_compile identity_udp_listener.py
systemctl restart ambient-cognitive-runtime.service
```

---

## 9. Validation

```bash
python3 -m py_compile identity_udp_listener.py
systemctl restart ambient-cognitive-runtime.service
systemctl status ambient-cognitive-runtime.service
journalctl -u ambient-cognitive-runtime.service -f
```

Controlled sequence:

```text
Claudio
Student
Mariana
Hermínio
Claudio
```

Confirm that the Runtime leaves `thinking` without waiting for TTS completion,
visual updates remain correct, Echo Pyramid speaks after the main runtime cycle,
no greetings overlap and skipped greetings are explicitly logged.

---

## 10. Engineering conclusion

This is a localized, reversible competition-time mitigation for a measured
synchronous TTS bottleneck. The final voice concurrency model remains a
post-competition engineering topic.
