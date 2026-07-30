# Echo Pyramid Voice Integration — Technical Milestone Note

## Ambient Physical AI

**Node:** Echo Pyramid Voice Node  
**Branch:** `feature/ambient-runtime-node`  
**Milestone:** Dynamic personalized voice delivery through StackFlow TTS  
**Status:** VALIDATED  
**Date:** 2026-07-30

---

## 1. Purpose of this note

This technical note freezes the engineering state reached during the Echo Pyramid voice integration milestone.

It records:

- what was implemented;
- which files were created or modified;
- which architectural decisions were made;
- which tests succeeded;
- which approaches failed or proved incomplete;
- which limitations remain;
- what must be preserved when documentation and README files are revised later.

This note is intentionally stored close to the firmware because the milestone spans both the embedded Echo Pyramid firmware and the AX630C Cognitive Runtime.

---

## 2. Milestone summary

The Ambient Physical AI system can now perform the following end-to-end journey:

```text
Presence detected
        ↓
Identity Node requests NFC identification
        ↓
NFC badge is read
        ↓
Identity Package reaches the AX630C
        ↓
Cognitive Runtime builds the current context
        ↓
A personalized spoken message is selected
        ↓
StackFlow TTS generates the complete PCM stream
        ↓
The PCM stream is wrapped as a WAV-compatible payload
        ↓
The AX630C sends the audio through TCP port 5006
        ↓
The Echo Pyramid reproduces the personalized message
```

Validated example:

```text
Bem-vindo ao laboratório, Hermínio.
```

The current local TTS model is English, so Portuguese pronunciation is imperfect, but the complete phrase and user name are intelligible.

---

## 3. Final architecture

### 3.1 Responsibility boundaries

The final design preserves centralized cognition:

```text
AX630C / Cognitive Runtime
    decides what must be spoken
    generates the text
    invokes StackFlow TTS
    receives the generated PCM
    sends the final audio stream

Echo Pyramid Voice Node
    receives compact semantic commands
    receives PCM audio
    renders display and RGB states
    reproduces the audio
```

The Echo Pyramid does not decide authorization, profile semantics, user identity, or message content.

### 3.2 Independent communication channels

Two channels are intentionally preserved:

```text
UDP port 5005
    semantic execution commands
    WELCOME
    WELCOME|<name>
    RED
    GREEN
    BLUE
    OFF

TCP port 5006
    ordered voice stream
    APAI Voice Protocol V1
    PCM 16 kHz
    mono
    signed 16-bit little-endian
```

UDP remains appropriate for compact visual/status commands. TCP is required for complete ordered audio delivery.

---

## 4. Embedded firmware work

### 4.1 M5Echo-Pyramid library adaptation

The M5Echo-Pyramid component was adapted to operate with the already existing ESP-IDF v5 I2C master bus.

Relevant additions included:

```text
I2CBus.attach(...)
M5EchoPyramid.begin(existing_bus_handle, ...)
```

The original hardware initialization sequence was preserved. The change avoided creating a second conflicting I2C bus and allowed the display, codec, audio path, and existing peripherals to coexist.

### 4.2 Audio bridge

Created:

```text
main/audio_bridge.h
main/audio_bridge.cpp
```

The bridge evolved from a simple test-tone interface into a reusable audio boundary.

Final capabilities include:

```text
audio_bridge_init(...)
audio_bridge_play_pcm(...)
audio_bridge_record_pcm(...)
audio_bridge_record_playback_test(...)
```

The bridge isolates the application from codec and I2S details.

### 4.3 Audio hardware validation

The following were validated on the physical Echo Pyramid:

- codec initialization;
- microphone capture;
- speaker playback;
- record-and-playback test;
- shared I2C operation;
- I2S data path;
- display;
- RGB;
- Wi-Fi;
- UDP semantic commands.

The microphone successfully recorded audio and the speaker reproduced the same recording.

### 4.4 Voice receiver component

Created:

```text
components/voice_receiver/
├── CMakeLists.txt
├── voice_receiver.c
└── include/
    └── voice_receiver.h
```

The component receives voice streams through TCP and uses a callback instead of depending directly on `audio_bridge`.

Contract:

```c
typedef esp_err_t (*voice_receiver_playback_cb_t)(
    const int16_t *samples,
    size_t sample_count
);
```

This keeps the component reusable and prevents reverse coupling with the application layer.

### 4.5 APAI Voice Protocol V1

The receiver accepts a compact binary header followed by PCM data.

```text
Magic: APAI
Version: 1
Channels: 1
Bits per sample: 16
Sample rate: 16000 Hz
Sample count: total mono samples
Payload: signed 16-bit little-endian PCM
```

Default TCP port:

```text
5006
```

### 4.6 Main firmware integration

The embedded application now:

1. initializes the Echo Pyramid hardware;
2. initializes the audio subsystem;
3. connects to Wi-Fi;
4. starts the TCP voice receiver on port 5006;
5. preserves the UDP semantic server on port 5005;
6. keeps display, RGB, heartbeat, and personalized welcome behavior operational;
7. forwards received PCM to `audio_bridge_play_pcm()`.

Validated logs included:

```text
Voice receiver listening on TCP port 5006
UDP server listening on port 5005
```

The pre-existing semantic event:

```text
WELCOME|Hermínio
```

continued to drive display and RGB correctly after the TCP audio integration.

---

## 5. Cognitive Runtime work

### 5.1 Semantic message selection

Modified:

```text
runtime/cognitive/stackflow/semantic_services.py
```

Added canonical message selection functions:

```python
build_identity_voice_message(identity)
get_current_identity_voice_message()
```

Validated behavior:

```text
Hermínio
→ Bem-vindo ao laboratório, Hermínio.

Mariana
→ Bem-vinda ao laboratório, Mariana.

Other recognized profile
→ Bem-vindo ao laboratório, <name>.

Unknown or unauthenticated badge
→ Crachá não identificado. Por favor, procure a administração.
```

The function tolerates compatible field variants such as:

```text
profile_id / id
display_name / name
authenticated / recognized / identified
```

### 5.2 Echo Pyramid adapter

Modified:

```text
runtime/cognitive/stackflow/echo_pyramid_adapter.py
```

The adapter now preserves the validated UDP path and adds the TCP voice path.

Capabilities:

```python
notify(semantic_event)
send_pcm(...)
send_wav(...)
speak(...)
```

The manually generated WAV test confirmed:

```text
AX630C
→ TCP 5006
→ Echo Pyramid
→ speaker playback
```

### 5.3 StackFlow TTS client

Created:

```text
runtime/cognitive/stackflow/stackflow_tts_to_echo_pyramid.py
```

This client communicates with:

```text
llm_sys
TCP port 10001
```

It performs:

```text
tts.setup
        ↓
tts.utf-8 inference
        ↓
receives base64 audio blocks
        ↓
decodes all PCM blocks
        ↓
waits for the complete stream
        ↓
assembles one PCM buffer
        ↓
wraps PCM as 16 kHz mono 16-bit WAV
        ↓
EchoPyramidAdapter.send_wav()
        ↓
releases the TTS work unit with exit
```

### 5.4 Identity listener automation

Modified:

```text
runtime/cognitive/stackflow/identity_udp_listener.py
```

After context construction, the listener now:

1. obtains the `who` section;
2. selects the canonical voice message;
3. invokes the validated StackFlow TTS client;
4. waits for the complete audio generation and delivery;
5. logs PASS or FAIL;
6. continues the remaining semantic pipeline even if voice delivery fails.

Voice delivery is observational and must not block:

- StackChan;
- RGB Strip;
- Ambient Runtime;
- runtime-state indicator;
- semantic dispatch.

---

## 6. StackFlow discovery and validation

Installed services discovered on the AX630C:

```text
/opt/m5stack/bin/llm_tts
/opt/m5stack/bin/llm_melotts
```

System services:

```text
llm-tts.service
llm-melotts.service
llm-sys.service
```

The external StackFlow endpoint was confirmed:

```text
0.0.0.0:10001
process: llm_sys
```

Installed models:

```text
single_speaker_english_fast
single_speaker_fast
melotts_zh-cn
```

Capabilities discovered:

```text
single_speaker_english_fast → English
single_speaker_fast         → Chinese
melotts_zh-cn                → Chinese-oriented MeloTTS
```

No Portuguese TTS model is currently installed.

---

## 7. Important failed or incomplete approaches

### 7.1 Calling `llm_tts --help`

Attempt:

```text
/opt/m5stack/bin/llm_tts --help
```

Result:

- the executable started as a long-running StackFlow service;
- it did not expose a conventional CLI;
- it connected to the internal ZMQ infrastructure;
- manual interruption was required.

Conclusion:

```text
llm_tts and llm_melotts are services, not command-line text-to-WAV utilities.
```

### 7.2 External TTS command template

An initial adapter design used:

```text
ECHO_PYRAMID_TTS_COMMAND
```

This was not appropriate for the installed StackFlow services because they must be accessed through the StackFlow TCP protocol rather than by starting another service process.

The validated route is now:

```text
Python client
→ llm_sys TCP 10001
→ TTS unit
```

### 7.3 Assuming the response contained a complete WAV

The initial client requested:

```text
tts.base64.wav
```

but the installed service returned base64-encoded raw PCM blocks without a `RIFF` WAV header.

The client was corrected to:

- detect raw PCM;
- assemble all chunks;
- create the WAV container locally.

### 7.4 Stopping after the first PCM block

The first successful TTS attempt reproduced only the beginning of the sentence and omitted the user name.

Cause:

```text
The client treated the first returned PCM block as the complete speech.
```

Correction:

```text
Collect every PCM block until the post-audio inactivity boundary.
```

After correction, the full phrase and user name were reproduced.

### 7.5 TTS unit not released before a second test

A second immediate execution became stuck during `tts.setup`.

Cause:

```text
The previous TTS work unit had not been fully released and acknowledged.
```

Correction:

- send the StackFlow `exit` action;
- wait for its acknowledgement;
- restart `llm-tts.service` only when recovering from a stale unit.

---

## 8. Physical validations completed

### Embedded node

```text
Display                         PASS
RGB                             PASS
Wi-Fi                           PASS
UDP semantic receiver           PASS
TCP voice receiver              PASS
Codec initialization            PASS
Microphone capture              PASS
Speaker playback                PASS
PCM playback callback           PASS
Record/playback diagnostic      PASS
```

### Cognitive Runtime

```text
Identity context                PASS
Personalized message selection  PASS
Unknown badge message           PASS
StackFlow TCP/10001             PASS
TTS setup                       PASS
Text inference                  PASS
Base64 PCM reception            PASS
Multi-block stream assembly     PASS
PCM-to-WAV wrapping             PASS
TCP/5006 delivery               PASS
TTS work-unit release           PASS
```

### End-to-end NFC journey

```text
Presence
→ NFC identity
→ AX630C context
→ personalized text
→ StackFlow TTS
→ PCM stream
→ Echo Pyramid
→ full spoken greeting
```

Status:

```text
VALIDATED
```

---

## 9. Known limitations

### 9.1 Portuguese pronunciation

The currently installed model is:

```text
single_speaker_english_fast
```

It pronounces Portuguese imperfectly.

The phrase is intelligible and complete, but this is not the final desired voice quality.

Future improvement:

- install or integrate a Portuguese-capable TTS model;
- preserve the current text-selection and PCM delivery architecture;
- replace only the TTS backend or model.

### 9.2 Blocking execution

The identity listener currently waits for the external TTS client process to complete.

This is acceptable for the present milestone and demo validation, but a future production refinement may move voice generation to:

- a dedicated worker thread;
- a queue;
- a persistent TTS client;
- or a voice service process.

Any future change must preserve semantic-pipeline fault isolation.

### 9.3 Wake Word not yet implemented

Wake Word remains the next milestone.

Initial target:

```text
Hi ESP
```

The Wake Word implementation must preserve:

- UDP port 5005;
- TCP port 5006;
- audio playback;
- display;
- RGB;
- existing Wi-Fi behavior;
- personalized NFC journey.

The audio system must prevent the Wake Word detector from reacting to audio reproduced by its own speaker.

---

## 10. Relevant files

### Embedded firmware

```text
firmware/nodes/expression-node/echo-pyramid-voice-node/
├── components/
│   ├── M5Echo-Pyramid/
│   └── voice_receiver/
│       ├── CMakeLists.txt
│       ├── voice_receiver.c
│       └── include/
│           └── voice_receiver.h
├── main/
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── audio_bridge.cpp
│   └── audio_bridge.h
├── sdkconfig
└── notes/
    └── ECHO_PYRAMID_VOICE_INTEGRATION_MILESTONE.md
```

### Cognitive Runtime

```text
runtime/cognitive/stackflow/
├── echo_pyramid_adapter.py
├── identity_udp_listener.py
├── semantic_services.py
└── stackflow_tts_to_echo_pyramid.py
```

### Deployment tool

```text
tools/deploy_runtime_ax630c_full.bat
```

The deployment script copies the complete StackFlow runtime tree and validates the Python syntax of the new voice integration files on the AX630C.

---

## 11. Files that should not be included in the milestone commit

The repository currently contains temporary or local artifacts that should remain outside this commit unless reviewed separately:

```text
saudacao.m4a
saudacao_echo_pyramid.wav
send_voice_pcm.py
firmware/nodes/expression-node/echo-pyramid-voice-node.zip
firmware/nodes/expression-node/echo-pyramid-voice-node/oi.txt
firmware/nodes/expression-node/echo-pyramid-voice-node/tree_echo.txt
firmware/nodes/expression-node/ver1.txt
firmware/nodes/expression-node/ver2.txt
firmware/nodes/expression-node/echo-pyramid-voice-node-integration/
tools/deploy_runtime_ax630c_full_tts.bat
```

The extra upstream library metadata and examples under `components/M5Echo-Pyramid/` must also be reviewed before staging. Only files intentionally required by the vendored component should enter the public repository.

---

## 12. Milestone conclusion

The Echo Pyramid is no longer only a visual semantic-expression node.

It is now a networked voice renderer integrated into the Ambient Physical AI journey.

The validated capability is:

```text
Identity-aware
personalized
dynamically synthesized
network-delivered
spoken interaction
```

The architectural separation remains correct:

```text
Cognition and language generation → AX630C
Voice rendering                   → Echo Pyramid
```

This milestone must be preserved before the Wake Word work begins.
