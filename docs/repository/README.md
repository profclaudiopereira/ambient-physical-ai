# Repository Organization

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

Esta documentação descreve a organização oficial do repositório do projeto Ambient Physical AI.

O objetivo é garantir consistência arquitetural, simplicidade operacional e evolução sustentável durante o desenvolvimento do ecossistema.

---

# Philosophy

O repositório foi projetado para refletir diretamente a arquitetura do projeto.

Princípios adotados:

* Demo First. Complexity Later.
* Hardware First Validation.
* Distributed Cognition.
* Local First.
* Incremental Integration.
* Real Systems Over Simulations.

A estrutura deve evoluir a partir de hardware real, firmware real e integrações reais.

---

# Repository Structure

```text
ambient-physical-ai/

├── firmware/
├── runtime/
├── docs/
├── assets/
├── hardware/
├── demos/
├── tools/
├── scripts/
└── .github/
```

---

# Firmware

Contém todos os firmwares embarcados do ecossistema.

```text
firmware/

├── shared/
└── nodes/
```

---

## Shared

Componentes reutilizáveis por múltiplos nós.

Exemplos:

```text
drivers
sensor abstractions
common utilities
shared contracts
future communication helpers
```

---

## Nodes

Firmwares dos dispositivos físicos.

Estado atual:

```text
firmware/nodes/

├── identity-node/
├── presence-node-v1/
├── presence-node-legacy/
└── ambient-runtime-node/
```

---

### identity-node

Hardware:

```text
M5Dial
```

Responsabilidades:

```text
NFC identification
UID mapping
Context selection
Identity package generation
Identity visualization
```

Status:

```text
Operational Baseline
```

---

### presence-node-v1

Hardware:

```text
AtomS3 Lite
+
Unit Mini ToF-90 (VL53L0X)
```

Responsabilidades:

```text
Presence detection
Distance measurement
Presence events
```

Status:

```text
Operational Baseline
```

---

### presence-node-legacy

Histórico das investigações anteriores envolvendo:

```text
CoreS3 Lite
VL53L0X
```

Mantido para referência técnica.

Não representa o baseline atual.

---

### ambient-runtime-node

Hardware:

```text
PoE-P4
```

Responsabilidades:

```text
Environmental sensing
Ambient state management
Displays
Future ambient transformation
```

Status:

```text
Active Development
```

---

# Runtime

Contém os componentes executados no runtime cognitivo e ambiental.

```text
runtime/

├── cognitive/
├── ambient/
├── shared/
└── infrastructure/
```

---

## Cognitive

Representa o Cognitive Runtime Node.

Hardware principal:

```text
AX630C + LLM Mate
```

Responsabilidades:

```text
Context interpretation
Reasoning
Memory
Multimodal processing
StackFlow Runtime
Decision making
```

Status:

```text
Discovery Completed
Integration Phase Next
```

---

## Ambient

Representa o Ambient Runtime Node.

Hardware principal:

```text
PoE-P4
```

Responsabilidades:

```text
Environmental adaptation
Ambient displays
Spatial synchronization
Future contextual responses
```

---

## Shared

Artefatos compartilhados entre os runtimes.

Exemplos:

```text
schemas
contracts
shared definitions
```

---

## Infrastructure

Infraestrutura local mínima.

Exemplos futuros:

```text
Mosquitto
deployment scripts
runtime services
```

---

# Documentation

```text
docs/
```

Centraliza toda a documentação técnica do projeto.

Estrutura atual:

```text
docs/

├── architecture/
├── discoveries/
├── notes/
├── repository/
└── snapshots/
```

---

## architecture

Arquitetura oficial do projeto.

---

## discoveries

Registro das descobertas técnicas realizadas durante investigações.

Exemplo:

```text
AX630C
StackFlow
Protocols
Context behavior
```

---

## notes

Laboratórios, observações técnicas e investigações.

---

## snapshots

Pontos de controle do projeto.

---

# Assets

```text
assets/
```

Contém os ativos visuais do projeto.

Exemplos:

```text
photos
diagrams
renders
screenshots
competition material
posters
```

---

# Hardware

```text
hardware/
```

Documentação específica dos dispositivos físicos.

Estrutura recomendada:

```text
device/

├── README.md
├── photos/
├── diagrams/
├── notes/
└── references/
```

---

# Demos

```text
demos/
```

Contém cenários de demonstração e validação do ecossistema.

Objetivos:

```text
Contextual Intelligence
Ambient Transformation
Identity Recognition
Embodied AI
Natural Interaction
```

---

# Architectural Model

O projeto é organizado em camadas funcionais:

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

---

## Presence Layer

Baseline atual:

```text
AtomS3 Lite
+
VL53L0X
```

---

## Identity Layer

Baseline atual:

```text
M5Dial
+
WS1850S NFC
```

---

## Cognitive Layer

Baseline atual:

```text
AX630C
+
LLM Mate
+
StackFlow
```

---

## Ambient Layer

Baseline atual:

```text
PoE-P4
```

---

## Expression Layer

Dispositivos atuais:

```text
StackChan
Voice Pyramid
```

---

# Repository Evolution Policy

Evitar:

```text
Overengineering
Premature abstractions
Enterprise-style architecture
Protocol-driven directory structures
```

Priorizar:

```text
Clarity
Modularity
Incremental evolution
Functional demonstrations
Real hardware validation
```

Mudanças estruturais significativas devem surgir apenas a partir de necessidades observadas durante integração e validação real.

---

# Guiding Principle

## Demo First. Complexity Later.

Toda evolução do repositório deve ser guiada por:

* hardware real;
* firmware real;
* integração real;
* experiências reais.

A organização deve permanecer alinhada com a visão central do projeto:

**Ambient Physical AI — Distributed Cognitive Ecosystem Powered by StackFlow**
