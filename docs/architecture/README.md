# Architecture

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

> "Percebe. Identifica. Compreende. Decide. Transforma. Expressa."

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
* embodiment físico;
* experiências memoráveis.

---

# Architectural Flow

O ecossistema opera através do seguinte fluxo:

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

Cada camada possui responsabilidades específicas dentro da arquitetura.

---

# Presence Layer

Responsável por perceber que existe alguém ou algo no ambiente.

Pergunta respondida:

> Existe alguém aqui?

## Baseline Atual

Hardware:

```text
AtomS3 Lite
+
Unit Mini ToF-90 (VL53L0X)
```

Responsabilidades:

* detecção de presença;
* medição de distância;
* eventos de aproximação;
* gatilhos contextuais iniciais.

Status:

```text
Operational Baseline
```

---

# Identity Layer

Responsável por identificar quem está interagindo com o ambiente.

Pergunta respondida:

> Quem está aqui?

## Baseline Atual

Hardware:

```text
M5Dial
+
WS1850S NFC
```

Responsabilidades:

* identificação NFC;
* leitura de UID;
* mapeamento UID → perfil;
* seleção de contexto;
* geração de Identity Package;
* visualização da identidade.

Exemplo:

```text
UID
↓
Profile
↓
Identity Package
```

Status:

```text
Operational Baseline
```

---

# Cognitive Layer

Responsável por compreender contexto e tomar decisões.

Pergunta respondida:

> O que significa esta situação?

````

## Cognitive Runtime Node

Hardware:

```text
AX630C
+
LLM Mate
````

Software:

```text
StackFlow Runtime
```

Responsabilidades:

* interpretação contextual;
* coordenação cognitiva;
* inferência local;
* multimodalidade;
* memória;
* tomada de decisão;
* futuras capacidades de RAG.

Status:

```text
Integration Phase Next
```

---

# Ambient Transformation Layer

Responsável por transformar fisicamente o ambiente.

Pergunta respondida:

> Como o ambiente deve responder?

## Ambient Runtime Node

Hardware:

```text
PoE-P4
```

Responsabilidades:

* adaptação ambiental;
* displays;
* sincronização espacial;
* estados ambientais;
* feedback contextual;
* futuras transformações físicas.

Status:

```text
Active Development
```

---

# Expression Layer

Representa a forma como a inteligência do ambiente se manifesta para o usuário.

Pergunta respondida:

> Como o ambiente se expressa?

Exemplos:

```text
StackChan
Voice Pyramid
Displays
Lighting
Animations
Audio Feedback
```

Responsabilidades:

* embodiment;
* comunicação;
* feedback visual;
* feedback sonoro;
* interação natural.

Status:

```text
Future Integration
```

---

# StackFlow

StackFlow não é:

* MQTT;
* middleware;
* barramento de mensagens;
* abstração de firmware.

StackFlow é uma:

## Distributed Cognitive Coordination Fabric

Responsável por coordenar:

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

através de eventos, contexto e coordenação cognitiva distribuída.

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

# Current Architectural Roles

| Component             | Role                                      |
| --------------------- | ----------------------------------------- |
| AtomS3 Lite + VL53L0X | Presence Node                             |
| M5Dial                | Identity Node                             |
| AX630C + LLM Mate     | Cognitive Runtime Node                    |
| PoE-P4                | Ambient Runtime Node                      |
| StackChan             | Expression Node                           |
| Voice Pyramid         | Voice Expression Node                     |
| StackFlow             | Distributed Cognitive Coordination Fabric |

---

# Current Project Position

## Operational Baselines

Validated:

```text
Presence Layer
Identity Layer
```

In Progress:

```text
Ambient Layer
```

Next:

```text
Presence
↓
Identity
↓
AX630C Cognitive Runtime
```

---

# Design Principles

Priorizar:

* simplicidade;
* modularidade;
* cognição distribuída;
* interação contextual;
* validação em hardware real;
* demonstrações funcionais.

Evitar:

* overengineering;
* abstrações prematuras;
* arquitetura enterprise;
* cloud-first mentality;
* complexidade sem valor demonstrável.

---

# Guiding Principle

## Demo First. Complexity Later.

Toda evolução arquitetural deve ser validada através de:

* hardware real;
* firmware real;
* integração real;
* experiências reais.

A arquitetura deve evoluir a partir da prática e não de abstrações prematuras.

---

# Current Milestone

O projeto concluiu com sucesso as primeiras camadas operacionais:

```text
Presence
✓

Identity
✓
```

O próximo marco arquitetural é:

```text
Presence Event
↓
Identity Package
↓
AX630C Cognitive Runtime
↓
Decision
```

estabelecendo a primeira integração fim-a-fim do ecossistema Ambient Physical AI.
