# Discoveries

## Ambient Physical AI

### AX630C + StackFlow Discovery Archive

Esta pasta centraliza todas as descobertas técnicas realizadas durante a exploração do Runtime Cognitivo StackFlow executando sobre a plataforma AX630C.

Princípio adotado:

```text
Explore First.
Understand First.
Integrate Later.
```

O objetivo desta área não é armazenar documentação de firmware ou arquitetura final, mas registrar o conhecimento adquirido durante a investigação do sistema.

---

# Discovery Timeline

## 2026-06

### Discovery 01 — StackFlow Protocol

Objetivo:

Identificar como acessar programaticamente o Runtime Cognitivo.

Resultados:

* Porta TCP 10001 identificada
* Serviço llm_sys identificado
* Serviço llm_llm identificado
* Protocolo JSON over TCP validado
* Fluxo Setup → Inference → Streaming → Exit validado
* Primeiro cliente Python validado

Arquivos:

* DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_PT-BR.md
* DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_EN.md
* DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_DETAILED_PT-BR.md
* DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1_DETAILED_EN.md

Principais descobertas:

```text
TCP 10001
    ↓
llm_sys
    ↓
llm_llm
    ↓
Qwen2.5-0.5B
```

---

### Discovery 02 — Context and Session Behavior

Objetivo:

Compreender:

* ciclo de vida da sessão
* contexto
* memória
* comportamento multi-turn

Resultados:

Validado:

* criação de sessão
* context injection
* inferência single-turn
* streaming response

Não validado:

* memória multi-turn
* persistência de contexto
* estado conversacional interno

Arquivos:

* DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_PT-BR_COMPLETE.md
* DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_EN_COMPLETE.md

Hipótese atual:

```text
Evento
    ↓
Pacote de Contexto
    ↓
Inferência
    ↓
Resposta
```

em vez de:

```text
Conversação
    ↓
Memória Persistente
    ↓
Resposta
```

---

# Discovery Status

| Discovery   | Status    |
| ----------- | --------- |
| Protocol V1 | Completed |
| Context V1  | Partial   |
| Memory V1   | Planned   |
| ASR V1      | Planned   |
| TTS V1      | Planned   |
| VLM V1      | Planned   |
| YOLO V1     | Planned   |

---

# Future Discoveries

Próximas investigações previstas:

## Memory V1

Objetivo:

Determinar:

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

Objetivo:

Mapear o serviço:

```text
llm_asr
```

e seu protocolo.

---

## TTS V1

Objetivo:

Mapear:

```text
llm_tts
llm_melotts
```

e integração de síntese de voz.

---

## VLM V1

Objetivo:

Investigar:

```text
llm_vlm
llm_camera
```

e capacidades multimodais.

---

## YOLO V1

Objetivo:

Investigar:

```text
llm_yolo
```

e capacidades de detecção visual.

---

# Architectural Relevance

As descobertas registradas nesta pasta fundamentam diretamente a evolução da arquitetura:

```text
Presence
    ↓
Identity
    ↓
Context
    ↓
Cognition
    ↓
Ambient Transformation
```

Todo conhecimento obtido nesta área deve ser considerado insumo para futuras integrações do projeto Ambient Physical AI.
