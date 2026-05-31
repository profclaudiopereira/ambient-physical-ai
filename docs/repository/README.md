# Repository Organization

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

Esta documentação descreve a organização oficial do repositório do projeto Ambient Physical AI.

O objetivo é garantir consistência, simplicidade e evolução sustentável durante o desenvolvimento do ecossistema.

---

# Filosofia

O repositório foi projetado para refletir a arquitetura conceitual do projeto.

A organização prioriza:

* clareza arquitetural;
* modularidade;
* desenvolvimento incremental;
* demonstrações funcionais;
* simplicidade operacional;
* evolução guiada por protótipos reais.

Princípio orientador:

> Demo First. Complexity Later.

---

# Estrutura Geral

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

```text
firmware/

├── shared/
└── nodes/
```

## shared

Componentes reutilizáveis entre múltiplos firmwares.

Exemplos futuros:

* MQTT helpers
* drivers
* abstrações de sensores
* bibliotecas comuns

## nodes

Firmwares dos dispositivos físicos do ecossistema.

Exemplos futuros:

* env-node
* voice-node
* stackchan-node
* presence-node
* display-node

---

# Runtime

```text
runtime/

├── cognitive/
├── ambient/
├── shared/
└── infrastructure/
```

## cognitive

Representa o Cognitive Runtime Node.

Responsável por:

* StackFlow
* raciocínio
* memória
* multimodalidade
* coordenação cognitiva

Hardware principal:

* AX630C

---

## ambient

Representa o Ambient Runtime Node.

Responsável por:

* iluminação
* displays
* sincronização ambiental
* transformação física do ambiente

Hardware principal:

* PoE-P4

---

## shared

Artefatos compartilhados entre os runtimes.

Exemplos:

* contratos
* schemas
* definições comuns

---

## infrastructure

Infraestrutura mínima necessária ao funcionamento local.

Exemplos futuros:

* Mosquitto
* configurações locais
* scripts de implantação

---

# Documentation

```text
docs/
```

Centraliza toda a documentação do projeto.

Categorias previstas:

* architecture
* hardware
* repository
* demos
* notes

---

# Assets

```text
assets/
```

Contém os ativos visuais do projeto.

Exemplos:

* fotografias
* diagramas
* renders
* screenshots
* material de demonstração

---

# Hardware

```text
hardware/
```

Documentação específica dos dispositivos físicos.

Cada diretório pode conter:

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

Contém demonstrações e cenários de apresentação.

O foco do projeto é demonstrar:

* inteligência contextual;
* transformação ambiental;
* embodiment;
* interação natural.

---

# Convenções

## Estrutura

Evitar criar diretórios sem necessidade prática.

Novas estruturas devem surgir a partir de:

* hardware real;
* firmware real;
* experimentos reais;
* necessidades observadas.

---

## Arquitetura

Evitar:

* overengineering;
* abstrações prematuras;
* organização baseada em protocolos;
* arquitetura enterprise.

Priorizar:

* simplicidade;
* clareza;
* modularidade;
* demonstrações funcionais.

---

# Evolução

A arquitetura deverá evoluir gradualmente conforme o projeto avançar.

Mudanças estruturais significativas devem ser justificadas por necessidades observadas durante:

* integração;
* prototipação;
* validação de hardware;
* construção das demonstrações.

A organização do repositório deve permanecer alinhada com a visão central do projeto:

**Ambient Physical AI — Distributed Cognitive Ecosystem Powered by StackFlow**
