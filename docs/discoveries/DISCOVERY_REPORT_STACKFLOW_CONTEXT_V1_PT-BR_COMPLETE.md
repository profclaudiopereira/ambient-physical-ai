# DISCOVERY_REPORT_STACKFLOW_CONTEXT_V1_PT-BR

## Ambient Physical AI
## AX630C + StackFlow Lab

Data: 2026-06-08

# Resumo Executivo

Esta investigação teve como objetivo compreender o comportamento do Runtime Cognitivo StackFlow executando sobre o AX630C.

Após a conclusão da Discovery Protocol V1, onde foram identificados o protocolo JSON over TCP, a porta 10001, o fluxo Setup → Inference → Streaming → Exit e a criação do primeiro cliente Python funcional, o foco passou a ser o entendimento de contexto, sessão e estado conversacional.

# Ambiente Validado

- Ubuntu 22.04 LTS
- AX630C + LLM Mate
- ADB
- Ethernet
- Internet
- llm_sys
- llm_llm
- Modelo Qwen2.5-0.5B-prefill-20e

# Objetivos

- Entender ciclo de vida das sessões
- Verificar preservação de contexto
- Testar conversação multi-turn
- Avaliar injeção de contexto
- Avaliar arquitetura orientada a eventos

# Descobertas

## D01 – Ciclo de Vida

Fluxo validado:

Setup
→ work_id dinâmico
→ Inference
→ Streaming Response
→ Exit

Exemplo:

llm → setup → llm.1000

## D02 – Conversação Multi-turn

Tentativas de múltiplas inferências na mesma sessão resultaram em timeout.

Resultado:
NÃO VALIDADO

## D03 – Injeção de Contexto

Prompt:

Context: My name is Claudio.
Question: What is my name?

Resposta:

My name is Claudio.

Resultado:
VALIDADO

## D04 – Eventos

Os testes orientados a eventos foram planejados mas não concluídos devido à instabilidade observada nas sessões multi-turn.

Status:
PENDENTE

## D05 – Estado Conversacional

Não foi possível comprovar memória conversacional persistente.

Hipótese atual:

Evento → Contexto → Inferência → Resposta

# Estabilidade Observada

Padrão estável:

Setup
→ Uma inferência
→ Streaming
→ Exit

Padrão instável:

Setup
→ Múltiplas inferências
→ Timeout

Recuperação:

systemctl restart llm-llm
systemctl restart llm-sys

# Implicações para Ambient Physical AI

A arquitetura atual deve assumir que cada evento transporta todo o contexto necessário.

Exemplo:

Usuário: Claudio
Papel: Professor
Ambiente: Embedded Systems Lab
Evento: Presentation Mode

# Conclusão

Validado:

- Sessões
- Context Injection
- Single Turn
- Streaming

Não validado:

- Memória multi-turn
- Persistência de contexto
- Estado conversacional interno

Status:

PARCIALMENTE CONCLUÍDA
