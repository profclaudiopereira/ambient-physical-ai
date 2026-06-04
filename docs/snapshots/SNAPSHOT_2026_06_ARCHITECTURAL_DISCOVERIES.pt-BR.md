# Ambient Physical AI

## SNAPSHOT_2026_06_DESCOBERTAS_ARQUITETURAIS

Data: Junho de 2026

Status: Consolidação Arquitetural Concluída

---

# Objetivo

Este snapshot registra as principais descobertas arquiteturais, refinamentos conceituais e decisões estratégicas consolidadas durante a transição entre a fase de arquitetura e a fase de implementação.

Este documento marca a passagem entre:

* Fase de Arquitetura
* Fase de Validação de Hardware
* Fase de Bringup dos Runtimes

---

# Visão Atual do Ecossistema

```text
Pessoa
   ↓
Presença (ToF)
   ↓
Identidade (M5Dial)
   ↓
Contexto Físico
   ↓
Cognição (AX630C + StackFlow)
   ↓
Transformação Ambiental (PoE-P4)
   ↓
Embodiment (StackChan)
   ↓
Experiência
```

---

# Refinamentos Arquiteturais

## Cognitive Runtime Node

Hardware:

* AX630C Module LLM Kit
* LLM Mate

Responsabilidades:

* raciocínio
* memória
* interpretação de contexto
* processamento multimodal
* futuras capacidades de RAG
* tomada de decisão

Definição atual:

```text
AX630C
+
StackFlow
=
Cognitive Runtime Node
```

---

## Ambient Runtime Node

Hardware:

* Unit PoE-P4

Responsabilidades:

* gerenciamento de displays
* adaptação ambiental
* sincronização física
* orquestração do ambiente
* respostas perceptíveis ao usuário

Definição atual:

```text
PoE-P4
=
Ambient Runtime Node
```

O PoE-P4 transforma o ambiente.

---

## Ambient Expression Nodes

O projeto evoluiu de um conceito centralizado para um modelo distribuído de expressão ambiental.

Exemplos:

* LEDs RGB do StackChan
* LEDs RGB da Voice Pyramid
* Barra RGB baseada em NanoC6
* Atom Matrix
* Mini OLED

Responsabilidades:

* feedback visual
* expressão emocional
* indicação contextual
* representação de estado ambiental

Esses nós executam localmente as expressões coordenadas pelo Ambient Runtime.

---

## Identity & Contextual Interaction Node

Hardware:

* M5Dial V1.1

Responsabilidades:

* identificação de usuários
* autenticação NFC/RFID
* seleção de intenção
* interação contextual
* confirmação de perfil
* seleção de modos operacionais

Definição atual:

```text
Identidade
+
Intenção
+
Seleção de Contexto
```

O M5Dial deixou de ser visto apenas como um leitor RFID.

Ele passou a atuar como a porta de entrada entre identidade e comportamento do ambiente.

---

## Physical Context Layer

Uma das descobertas mais importantes surgiu durante a investigação sobre NFC.

Modelo inicial:

```text
Presença
↓
Identidade
↓
Cognição
```

Modelo refinado:

```text
Presença
↓
Identidade
↓
Contexto Físico
↓
Cognição
```

Artefatos físicos tornam-se marcadores de contexto.

---

# Refinamento do StackFlow

O StackFlow deixou de ser visto apenas como transporte de mensagens.

Definição atual:

```text
Distributed Cognitive Coordination Fabric
```

---

# Resumo da Descoberta sobre o StackChan

## Fase de Descoberta Concluída

Hipótese inicial:

```text
Robô
+
LLM
```

Avaliação atual:

```text
Plataforma de Agente de IA Física
```

---

## Avaliação Arquitetural

Papel recomendado:

```text
AX630C
↓
Runtime Cognitivo

StackFlow
↓
Camada de Coordenação

StackChan
↓
Camada de Embodiment e Expressão
```

---

# Decisões Estratégicas

* Demo First. Complexity Later.
* Explore First. Integrate Later.
* Exploration Outside. Validated Knowledge Inside.
* Hardware Before Assumptions.
* Architecture Before Integration.

---

# Consciência do Cronograma da Competição

Competição:

M5Stack Global Innovation Contest 2026

Meta de submissão:

27 de Julho de 2026

---

# Próxima Fase

## AX630C Bringup

Objetivos:

* validação do Linux
* validação de rede
* instalação do StackFlow
* descoberta de APIs
* descoberta de endpoints
* validação da cognição local

---

# Conclusão

A arquitetura principal permanece estável.

As descobertas desta fase reforçaram a visão original do projeto.

O projeto entra oficialmente na fase de implementação, onde os próximos refinamentos deverão surgir principalmente a partir de hardware real, experimentação e demonstrações.
