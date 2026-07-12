# Discoveries

## Ambient Physical AI

### Cognitive Runtime Discovery Archive

Esta área centraliza as descobertas técnicas realizadas durante a exploração das tecnologias que compõem o Cognitive Runtime do projeto Ambient Physical AI.

O objetivo não é armazenar documentação definitiva nem arquitetura final.

Seu propósito é preservar conhecimento adquirido durante investigações, validações e experimentos realizados antes da integração oficial ao ecossistema.

---

# Philosophy

Princípio adotado:

```text
Explore First
Understand First
Validate First
Integrate Later
```

Toda descoberta registrada nesta área deve contribuir para decisões arquiteturais futuras baseadas em evidências práticas.

---

# Scope

Esta área concentra principalmente investigações relacionadas a:

```text
AX630C
LLM Mate
StackFlow Runtime
Context Processing
Memory
ASR
TTS
Vision
Multimodal Services
```

---

# Discovery Timeline

## 2026-06

---

## Discovery 01 — StackFlow Protocol

### Objective

Identificar como acessar programaticamente o Cognitive Runtime.

### Results

Validado:

```text
TCP Port 10001
llm_sys service
llm_llm service
JSON over TCP protocol
Session setup
Inference request
Streaming response
Session termination
```

### Architecture Insight

```text
Client
↓
TCP 10001
↓
llm_sys
↓
llm_llm
↓
Qwen2.5-0.5B
```

### Status

```text
COMPLETED
```

---

## Discovery 02 — Context and Session Behavior

### Objective

Compreender:

```text
Session lifecycle
Context injection
Multi-turn behavior
Memory behavior
```

### Validated

```text
Session creation
Context injection
Single-turn inference
Streaming responses
```

### Not Yet Validated

```text
Persistent memory
Long-term context retention
Internal conversational state
```

### Current Hypothesis

```text
Event
↓
Context Package
↓
Inference
↓
Response
```

mais do que:

```text
Conversation
↓
Persistent Memory
↓
Response
```

### Status

```text
PARTIAL
```

---

## Discovery 03 — Presence and Identity Baselines

### Objective

Estabelecer as primeiras camadas físicas do ecossistema.

### Results

Presence Layer validated:

```text
AtomS3 Lite
+
Unit Mini ToF-90
(VL53L0X)
```

Identity Layer validated:

```text
M5Dial
+
WS1850S NFC
```

### Architectural Impact

O projeto agora possui duas camadas operacionais independentes:

```text
Presence
↓
Identity
```

criando a base necessária para integração com o Cognitive Runtime.

### Status

```text
COMPLETED
```

---

# Current Discovery Status

| Discovery               | Status    |
| ----------------------- | --------- |
| StackFlow Protocol V1   | Completed |
| Context Behavior V1     | Partial   |
| Presence Layer Baseline | Completed |
| Identity Layer Baseline | Completed |
| Memory V1               | Planned   |
| ASR V1                  | Planned   |
| TTS V1                  | Planned   |
| VLM V1                  | Planned   |
| YOLO V1                 | Planned   |

---

# Next Investigation Phase

O próximo objetivo do projeto não é mais validar hardware isoladamente.

O próximo objetivo é validar integração entre camadas.

Fluxo alvo:

```text
Presence Event
↓
Identity Package
↓
AX630C Cognitive Runtime
↓
Decision
```

---

## Cognitive Integration V1

Primeira integração prevista:

```text
Presence Layer
↓
Identity Layer
↓
AX630C
```

Objetivos:

```text
Receive Identity Package
Inject Context
Execute Inference
Generate Decision
```

Status:

```text
NEXT
```

---

# Future Discoveries

## Memory V1

Objetivo:

```text
Session
↓
Context
↓
Memory
```

e seus limites.

---

## ASR V1

Investigar:

```text
llm_asr
```

---

## TTS V1

Investigar:

```text
llm_tts
llm_melotts
```

---

## VLM V1

Investigar:

```text
llm_vlm
llm_camera
```

---

## YOLO V1

Investigar:

```text
llm_yolo
```

---

# Architectural Relevance

As descobertas desta pasta fundamentam diretamente a evolução do ecossistema:

```text
Presence
↓
Identity
↓
Cognition
↓
Ambient Transformation
↓
Expression
```

Todo conhecimento registrado aqui deve servir como base para futuras integrações e decisões arquiteturais do Ambient Physical AI.

---

# Current Project Position

```text
Presence Layer ............ Operational Baseline
Identity Layer ............ Operational Baseline
Cognitive Layer ........... Integration Phase Next
Ambient Layer ............. Active Development
Expression Layer .......... Future Integration
```

---

# Guiding Principle

## Discover. Validate. Integrate.

Nenhuma integração deve ocorrer sem compreensão prévia do comportamento real dos sistemas envolvidos.
