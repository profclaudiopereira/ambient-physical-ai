# Ambient Physical AI

## Ecossistema Cognitivo Distribuído Impulsionado por StackFlow

Ambient Physical AI é um projeto de pesquisa e engenharia que explora como IA moderna, consciência contextual, computação de borda e ambientes físicos podem ser integrados em um ecossistema cognitivo distribuído.

O projeto combina sistemas embarcados, Edge AI, Large Language Models (LLMs), interação multimodal, sensoriamento contextual e conceitos de computação ambiente para criar ambientes capazes de perceber, compreender e adaptar-se à presença e à intenção humana.

---

# Visão

O projeto é inspirado pelas ideias de **Mark Weiser**, criador da Computação Ubíqua, e pelos desenvolvimentos posteriores nas áreas de:

* Computação Sensível ao Contexto
* Inteligência Ambiente
* Edge AI
* Physical AI
* Sistemas Cognitivos Distribuídos

Em vez de criar um único dispositivo inteligente, o Ambient Physical AI explora como a inteligência pode emergir da colaboração entre múltiplos nós especializados distribuídos pelo ambiente.

O objetivo final é tornar a computação menos visível e mais integrada às experiências cotidianas.

---

# Princípios Arquiteturais

O Ambient Physical AI segue um modelo cognitivo em camadas:

```text
Presença
    ↓
Identidade
    ↓
Contexto
    ↓
Cognição
    ↓
Transformação Ambiental
    ↓
Experiência
```

Cada camada contribui com uma capacidade específica para o ecossistema.

---

# Arquitetura de Referência

```text
PESSOA
    ↓
Unit Mini ToF
    ↓ I2C
M5Dial
    ↓ Wi-Fi
AX630C + LLM Mate
    ↓ Ethernet
PoE-P4
```

Nós de apoio:

```text
StackChan
Voice Pyramid + AtomS3R
Atom Matrix
CoreS3 Lite
```

Periféricos ambientais:

```text
ENV-IV
Unit Mini OLED
Futuros Sensores I2C
```

---

# Runtime Nodes

## Presence Node

Responsável por detectar presença humana e atividade física.

Implementação atual:

```text
Unit Mini ToF-90
```

Funções:

* detecção de presença;
* percepção de proximidade;
* sensoriamento de atividade física.

---

## Identity Node

Responsável pela identificação do usuário e personalização contextual.

Implementação atual:

```text
M5Dial V1.1
+
NFC
```

Funções:

* identificação por NFC;
* seleção de modos contextuais;
* entrada de intenções do usuário.

---

## Cognitive Runtime Node

O núcleo cognitivo da arquitetura.

Implementação atual:

```text
AX630C
+
LLM Mate
```

Responsabilidades:

* raciocínio contextual;
* gerenciamento de memória;
* processamento multimodal;
* execução do Runtime StackFlow;
* futura integração com RAG.

Capacidades validadas incluem:

* Ubuntu 22.04;
* conectividade Ethernet;
* acesso SSH;
* inferência local de IA;
* Runtime StackFlow.

---

## Ambient Runtime Node

Responsável por transformar o ambiente físico de acordo com decisões contextuais.

Implementação atual:

```text
PoE-P4
```

Responsabilidades:

* adaptação ambiental;
* feedback contextual;
* agregação de sensores;
* controle de displays;
* futuras funcionalidades de iluminação e orquestração ambiental.

Arquitetura local atual:

```text
PoE-P4
    ↓
PCA9548A
    ├── ENV-IV
    ├── Unit Mini OLED
    └── Futuros Dispositivos I2C
```

---

## Expression Node

Responsável por fornecer uma presença física ao sistema cognitivo.

Implementação atual:

```text
StackChan
```

Responsabilidades:

* expressão visual;
* embodiment;
* interação contextual;
* comunicação emocional.

---

## Voice Node

Responsável pela interação conversacional.

Implementação atual:

```text
Voice Pyramid
+
AtomS3R
```

Responsabilidades:

* entrada de voz;
* saída de voz;
* experiências conversacionais;
* interação multimodal.

---

# StackFlow

O StackFlow atua como a malha de coordenação distribuída do ecossistema.

Seu propósito é conectar os runtime nodes e permitir o fluxo de:

* eventos;
* contexto;
* decisões;
* ações ambientais.

Conceitualmente:

```text
Presença
    ↓
Identidade
    ↓
Pacote de Contexto
    ↓
Cognitive Runtime
    ↓
Ambient Runtime
    ↓
Experiência
```

---

# Estrutura do Repositório

Documentação detalhada da organização do repositório:

```text
docs/repository/README.md
```

```text
ambient-physical-ai/

├── firmware/
├── runtime/
├── hardware/
├── docs/
├── demos/
├── assets/
├── tools/
├── scripts/
└── README.md
```

---

# Estado Atual

Validados:

* fundamentos da camada de Presença
* validação do hardware VL53L0X
* detecção do VL53L0X em ESP-IDF
* exploração do StackChan
* bring-up do AX630C
* descoberta do protocolo StackFlow
* inferência local com LLM
* experimentos de injeção de contexto

Em andamento:

* bring-up do Ambient Runtime Node
* implementação do Identity Node
* integração dos runtimes distribuídos

Planejado:

* camada de memória de longo prazo
* serviços de recuperação de contexto
* orquestração StackFlow multinó
* ambiente de demonstração para competição

---

# Competição

Este projeto está sendo desenvolvido como parte da preparação para a:

**M5Stack Global Innovation Contest 2026**

Marco interno atual:

```text
Data Alvo de Submissão
27 de Julho de 2026
```

---

# Filosofia

O Ambient Physical AI não é uma coleção de dispositivos conectados.

É uma exploração de como ambientes físicos podem tornar-se sensíveis ao contexto, coordenados cognitivamente e capazes de interação significativa.

O objetivo é criar ambientes que não apenas reajam a comandos, mas que percebam, compreendam e se adaptem às pessoas que os habitam.

---

## Autor

Claudio Pereira

Engenheiro de Sistemas Embarcados • Engenheiro Eletrônico • Professor

---

**Ambient Physical AI**

*Ecossistema Cognitivo Distribuído Impulsionado por StackFlow*
