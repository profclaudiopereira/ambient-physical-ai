# Hardware

## Ambient Physical AI

### Distributed Cognitive Ecosystem Powered by StackFlow

Esta pasta centraliza a documentação, fotografias, observações técnicas, diagramas e referências dos dispositivos físicos utilizados no projeto Ambient Physical AI.

---

# Hardware Philosophy

No Ambient Physical AI os dispositivos não são apenas componentes eletrônicos.

Cada dispositivo possui um papel arquitetural específico dentro do ecossistema cognitivo distribuído.

---

# Architectural Layers

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

# Operational Baselines

## Presence Layer

Hardware oficial:

```text
AtomS3 Lite
+
Unit Mini ToF-90 (VL53L0X)
```

Função:

```text
Presence Node V1
```

Responsabilidades:

* detecção de presença;
* medição de distância;
* eventos PRESENT / NOT_PRESENT.

---

## Identity Layer

Hardware oficial:

```text
M5Dial
+
WS1850S NFC
```

Função:

```text
Identity Node V1
```

Responsabilidades:

* identificação NFC;
* mapeamento de perfis;
* contexto;
* Identity Package.

---

## Cognitive Layer

Hardware oficial:

```text
AX630C
+
LLM Mate
```

Função:

```text
Cognitive Runtime Node
```

Responsabilidades:

* inferência;
* memória;
* coordenação cognitiva;
* multimodalidade.

---

## Ambient Layer

Hardware oficial:

```text
PoE-P4
```

Função:

```text
Ambient Runtime Node
```

Responsabilidades:

* sensores ambientais;
* displays;
* sincronização espacial;
* transformação do ambiente.

---

## Expression Layer

Hardware atual:

```text
StackChan
Voice Pyramid
```

Funções:

```text
Expression Node
Voice Expression Node
```

Responsabilidades:

* embodiment;
* comunicação;
* interação humano-IA.

---

# Hardware Inventory

## Cognitive

```text
AX630C
LLM Mate
```

---

## Ambient

```text
PoE-P4
ENV-IV
PaHub V2.1
```

---

## Identity

```text
M5Dial
NTAG216
```

---

## Presence

```text
AtomS3 Lite
Unit Mini ToF-90
```

---

## Expression

```text
StackChan
Voice Pyramid
```

---

## Experimental Platforms

```text
CoreS3 Lite
AtomS3R
AtomS3R Cam
NanoC6
Stamp-P4
Stamp-C6
```

Utilizados para:

* investigações;
* prototipação;
* validação de conceitos;
* futuras evoluções do sistema.

---

# Recommended Organization

Cada dispositivo pode conter:

```text
device/

├── README.md
├── photos/
├── diagrams/
├── notes/
└── references/
```

---

# Hardware Photo Library

As fotografias oficiais dos dispositivos são armazenadas em:

```text
assets/photos/
```

Utilizadas para:

* GitHub;
* documentação;
* relatórios;
* posters;
* competição;
* apresentações.

---

# Guiding Principle

O hardware é parte integrante da cognição distribuída.

Cada dispositivo existe para cumprir um papel específico dentro do fluxo:

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

A separação clara de responsabilidades reduz complexidade, aumenta escalabilidade e facilita futuras evoluções da plataforma.
