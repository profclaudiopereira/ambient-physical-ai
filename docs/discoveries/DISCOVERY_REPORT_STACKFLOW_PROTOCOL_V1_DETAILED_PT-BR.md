# DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1
## Ambient Physical AI
### Descoberta do Protocolo StackFlow no AX630C

**Data:** 2026-06-08

---

# Resumo Executivo

Este relatório documenta a identificação e validação do protocolo de comunicação utilizado pelo runtime StackFlow executando no AX630C Module LLM Kit.

A investigação demonstrou que o AX630C expõe uma API TCP que permite acesso direto ao modelo Qwen2.5 local sem necessidade de aplicações oficiais da M5Stack.

Esta descoberta transforma o AX630C de um kit de desenvolvimento em um Nó Cognitivo programável adequado ao Ambient Physical AI.

---

# Objetivo da Missão

Entender como o runtime local de IA pode ser acessado programaticamente.

Princípio:

Explore First. Integrate Later.

---

# Plataforma Validada

Hardware:
- AX630C Module
- LLM Mate
- Ethernet via LLM Mate
- Cabo Flat FPC

Sistema:
- Ubuntu 22.04 LTS
- Acesso ADB
- Shell root
- Conectividade Internet

Serviços:
- llm_sys
- llm_llm
- llm_asr
- llm_tts
- llm_vlm
- llm_yolo
- llm_kws

Modelo:
- Qwen2.5-0.5B-prefill-20e

---

# Discovery D01 — Identificação do Protocolo

Porta:

10001/TCP

Processo:

llm_sys

Configuração:

{
  "config_enable_tcp": 1
}

Protocolo:

JSON sobre TCP

Terminador:

nova linha (\n)

---

# Discovery D02 — Ping do Runtime

Requisição:

{
  "request_id":"safe_ping_001",
  "work_id":"sys",
  "action":"ping"
}

Resposta:

{
  "error":{"code":0}
}

Resultado:

Comunicação com o runtime validada.

---

# Discovery D03 — Criação de Sessão

O runtime não aceita inferência direta.

Primeiro é necessário criar uma sessão.

Requisição:

work_id = "llm"
action = "setup"

Resposta:

work_id = "llm.1000"

Descoberta importante:

Os identificadores de sessão são gerados dinamicamente.

---

# Discovery D04 — Inferência Local

Prompt:

Hello

Resposta:

Hello! How can I assist you today?

Foi observado streaming da resposta.

Resultado:

Inferência local totalmente validada.

---

# Discovery D05 — Cliente Python

Foi criado um cliente Python baseado em socket.

Capacidades:

- Conectar
- Criar sessão
- Enviar prompt
- Receber streaming
- Encerrar sessão

Local:

/root/client.py

---

# Estrutura das Mensagens

Campos:

- request_id
- work_id
- action
- object
- data

Fluxo:

Ping -> Setup -> Inference -> Stream -> Exit

---

# Arquitetura Descoberta

TCP 10001
    |
    v
 llm_sys
    |
    v
 llm_llm
    |
    v
 Qwen2.5-0.5B

---

# Impacto para Engenharia

O AX630C comporta-se como um servidor local de IA.

Possíveis clientes futuros:

- CoreS3 Lite
- StackChan
- PoE-P4
- Voice Node
- Ambient Node

---

# Conclusões

Validados:
- Interface TCP
- Protocolo JSON
- Gerenciamento de sessão
- Inferência em streaming
- Cliente Python

Status:

DESCOBERTA CONCLUÍDA
