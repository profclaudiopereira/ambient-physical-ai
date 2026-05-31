# Architecture

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

> "Percebe. Contextualiza. Pensa. Responde. Transforma o ambiente."

---

# Overview

Ambient Physical AI é um ecossistema cognitivo distribuído projetado para transformar ambientes em espaços inteligentes, contextuais e responsivos.

O objetivo não é construir:

* uma plataforma IoT tradicional;
* um dashboard;
* uma arquitetura cloud-first;
* um chatbot conectado a sensores.

O objetivo é criar:

* cognição distribuída;
* inteligência contextual;
* interação natural;
* transformação ambiental;
* experiências memoráveis.

---

# Arquitetura Conceitual

O ecossistema opera através do seguinte fluxo:

```text
Presence
→ Identity
→ Cognition
→ Ambient Transformation
→ Experience
```

Cada etapa possui responsabilidades específicas dentro da arquitetura.

---

# Presence Layer

Responsável por perceber que existe alguém ou algo no ambiente.

Exemplos:

* ToF
* sensores ambientais
* sensores de presença
* sensores contextuais

Pergunta respondida:

> Existe alguém aqui?

---

# Identity Layer

Responsável por identificar quem está interagindo com o ambiente.

Exemplo principal:

* M5Dial
* NFC
* RFID
* touch interaction
* mode switching

Pergunta respondida:

> Quem está aqui?

---

# Cognitive Layer

Responsável por compreender contexto e tomar decisões.

Implementação principal:

## Cognitive Runtime Node

Hardware principal:

* AX630C

Software principal:

* StackFlow

Responsabilidades:

* raciocínio contextual;
* memória;
* coordenação semântica;
* multimodalidade;
* inferência local;
* futuras capacidades de RAG;
* tomada de decisão.

Pergunta respondida:

> O que significa esta situação?

---

# Ambient Transformation Layer

Responsável por transformar fisicamente o ambiente.

Implementação principal:

## Ambient Runtime Node

Hardware principal:

* PoE-P4

Responsabilidades:

* iluminação;
* displays;
* sincronização espacial;
* estados ambientais;
* resposta contextual;
* adaptação do ambiente.

Pergunta respondida:

> Como o ambiente deve responder?

---

# Experience Layer

Representa a experiência percebida pelo usuário.

Pode envolver:

* StackChan
* displays
* voz
* iluminação
* animações
* feedback contextual

Pergunta respondida:

> O que o usuário percebe?

---

# StackFlow

StackFlow não é:

* uma camada MQTT;
* um middleware;
* um barramento de mensagens;
* uma abstração de firmware.

StackFlow é uma:

## Distributed Cognitive Coordination Fabric

Responsável por coordenar:

* percepção;
* identidade;
* cognição;
* embodiment;
* transformação ambiental;
* interação contextual.

---

# Runtime Model

```text
runtime/

├── cognitive/
│   └── stackflow/
│
├── ambient/
│
├── shared/
│
└── infrastructure/
```

---

# Architectural Roles

| Componente  | Papel                                     |
| ----------- | ----------------------------------------- |
| ToF         | Presence Node                             |
| M5Dial      | Identity Node                             |
| AX630C      | Cognitive Runtime Node                    |
| PoE-P4      | Ambient Runtime Node                      |
| StackChan   | Embodiment Node                           |
| ESP32 Nodes | Perception & Interaction Nodes            |
| StackFlow   | Distributed Cognitive Coordination Fabric |

---

# Design Principles

Priorizar:

* simplicidade;
* modularidade;
* cognição distribuída;
* interação contextual;
* demonstrações funcionais;
* inteligência ambiental.

Evitar:

* overengineering;
* abstrações prematuras;
* arquitetura enterprise;
* cloud-first mentality;
* complexidade sem valor demonstrável.

---

# Current Status

Fase atual:

## FOUNDATION PHASE

Prioridades:

1. Estruturação do repositório;
2. Hardware bringup;
3. Primeiros firmwares;
4. Runtime StackFlow;
5. Integração cognitiva;
6. Primeira demonstração funcional.

---

# Guiding Principle

## Demo First. Complexity Later.

Toda evolução arquitetural deve ser validada através de:

* hardware real;
* protótipos reais;
* integração real;
* experiências reais.

A arquitetura deve evoluir a partir da prática e não de abstrações prematuras.
