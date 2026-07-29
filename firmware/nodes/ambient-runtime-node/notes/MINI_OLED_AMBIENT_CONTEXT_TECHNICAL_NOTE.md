# NOTA TÉCNICA — INTEGRAÇÃO DO MINI OLED AO AMBIENT RUNTIME TAB5

## Ambient Physical AI

**Documento:** Nota Técnica de Engenharia  
**Escopo:** Ambient Runtime Tab5, driver SH1107 e integração com o Cognitive Runtime / StackFlow  
**Status:** integração funcional e validada no fluxo atual  
**Finalidade:** registrar tecnicamente as alterações realizadas antes da revisão documental definitiva do projeto

---

## 1. Objetivo desta nota

Esta nota técnica registra o trabalho executado para integrar o Mini OLED ao Ambient Runtime baseado no M5Stack Tab5 e conectá-lo ao fluxo cognitivo do projeto Ambient Physical AI.

O documento foi produzido para permitir uma revisão posterior, com mais calma, pelo MASTER e pela equipe do projeto, preservando:

- as decisões técnicas adotadas;
- as alterações realizadas no firmware do Tab5;
- as correções implementadas no driver do display OLED;
- as modificações feitas no StackFlow;
- os novos serviços de aquisição de contexto externo;
- o contrato de comunicação entre AX630C e Tab5;
- o estado atual de validação;
- os arquivos envolvidos;
- os pontos que ainda devem ser revisados antes do fechamento documental definitivo.

Esta nota não substitui o README final nem o relatório de encerramento. Ela funciona como registro técnico intermediário consolidado do trabalho realizado.

---

# 2. Visão geral do resultado

Antes deste trabalho, o Mini OLED conectado ao Tab5 não estava completamente integrado ao Ambient Runtime.

Existiam três problemas principais:

1. o driver SH1107 apresentava clipping de caracteres;
2. o Tab5 ainda não possuía um fluxo consolidado para apresentar contexto global e personalizado;
3. o StackFlow ainda não gerava automaticamente um `ambient_context` após a autenticação de um usuário.

O fluxo final passou a ser:

```text
Presence Layer
        ↓
Identity Node — M5Dial
        ↓
Identity Package
        ↓ UDP 4444
Cognitive Runtime — AX630C
        ↓
identity_udp_listener.py
        ↓
Context Builder
        ↓
Context Registry
        ↓
Ambient Context Service
        ↓
External APIs
        ↓
Normalized ambient_context
        ↓ UDP 5555
Ambient Runtime — Tab5
        ↓
semantic_event_receiver
        ↓
ambient_context_snapshot
        ↓
oled_context_presenter
        ↓
SH1107 Driver
        ↓
Mini OLED
```

Com isso, o OLED passou a apresentar:

```text
Contexto global
+
Contexto personalizado do usuário autenticado
```

Exemplos:

```text
Claudio
→ clima e UV
→ cotação USD/BRL
```

```text
Herminio
→ clima e UV
→ contexto do Sport Club do Recife
```

```text
Student
→ clima e UV
→ promoção da Steam
```

```text
Mariana
→ clima e UV
→ conteúdo de cinema via TMDB
```

---

# 3. Modificações no Ambient Runtime — Tab5

## 3.1. Responsabilidade preservada

O Tab5 permanece como Ambient Runtime do projeto.

Sua responsabilidade continua sendo:

- receber dados normalizados;
- manter o estado local do ambiente;
- apresentar informações nos displays;
- controlar os próprios periféricos;
- não executar lógica de API externa;
- não armazenar credenciais de serviços externos;
- não decidir a relevância de conteúdo para cada perfil.

A decisão arquitetural foi preservar o Tab5 como runtime de apresentação e atuação local.

O Tab5 não consulta diretamente:

- Open-Meteo;
- AwesomeAPI;
- TheSportsDB;
- Steam;
- TMDB.

Essas consultas ficam exclusivamente no AX630C.

---

## 3.2. Novo tipo de mensagem aceito

O receptor UDP do Tab5 já trabalhava com mensagens semânticas.

O fluxo foi ampliado para aceitar também:

```json
{
  "type": "ambient_context"
}
```

O receptor passou a distinguir duas classes principais:

```text
semantic_event
ambient_context
```

A mensagem `semantic_event` continua sendo usada para ações semânticas do sistema.

A mensagem `ambient_context` passou a ser utilizada para atualização do conteúdo contextual do Mini OLED.

---

## 3.3. Contrato do Ambient Context

O contrato adotado possui duas áreas independentes:

```text
global
personal
```

Estrutura geral:

```json
{
  "type": "ambient_context",
  "sequence": 1,
  "ttl_seconds": 900,
  "authenticated": true,
  "profile_id": "claudio",
  "global": {
    "available": true,
    "location": "RECIFE",
    "weather_summary": "PARTLY CLOUDY",
    "temperature_c": 28.4,
    "uv_index": 7.1,
    "uv_label": "HIGH"
  },
  "personal": {
    "available": true,
    "title": "USD BRL",
    "value": "R$ 5.42",
    "secondary": "+0.31%"
  }
}
```

Esse contrato permite que o Tab5 receba dados já tratados e prontos para apresentação.

---

## 3.4. Ambient Context Snapshot

O Ambient Runtime mantém uma representação local do último contexto recebido.

Essa representação é responsável por armazenar:

- sequência da mensagem;
- tempo de validade;
- perfil autenticado;
- disponibilidade do bloco global;
- disponibilidade do bloco pessoal;
- conteúdo global;
- conteúdo personalizado.

O objetivo é desacoplar:

```text
recepção UDP
```

de:

```text
renderização do OLED
```

Esse desacoplamento reduz dependências entre rede e apresentação.

---

## 3.5. OLED Context Presenter

O componente de apresentação do OLED passou a converter o `ambient_context` em conteúdo compacto para a tela.

Exemplo de saída validada:

```text
RECIFE

28C PARTLY

UV 7.1 HIGH

----------------

USD BRL

R$5.42

+0.31%

@CLAUDIO
```

A apresentação foi organizada para mostrar:

1. localização;
2. temperatura;
3. resumo meteorológico;
4. índice UV;
5. separador;
6. título pessoal;
7. valor principal;
8. informação secundária;
9. identificação do perfil.

O conteúdo do OLED é deliberadamente resumido, pois o display possui área limitada.

---

## 3.6. Separação entre display principal e Mini OLED

A arquitetura do Ambient Runtime permanece com responsabilidades distintas:

```text
Display principal de 5"
→ sensores locais
→ estado do sistema
→ rede
→ hardware
→ status cognitivo
```

```text
Mini OLED
→ contexto externo
→ informação global
→ conteúdo personalizado
```

Essa separação reduz poluição visual no display principal e dá ao OLED uma função contextual específica.

---

# 4. Correção do driver SH1107

## 4.1. Sintoma observado

O Mini OLED funcionava parcialmente, mas apresentava clipping no último caractere de determinadas linhas.

O comportamento observado indicava que:

- a comunicação I2C estava funcionando;
- o PaHub estava selecionando o canal correto;
- o display inicializava;
- parte do texto era desenhada;
- a janela visível não correspondia corretamente à geometria interna do controlador.

Ajustes simples de coluna apenas deslocavam o problema.

Em algumas tentativas, o clipping mudava de lado, mas não desaparecia.

---

## 4.2. Causa raiz

A sequência de inicialização utilizava:

```c
0xA8, 0x3F
```

Esse valor configura 64 linhas de multiplexação.

Entretanto, o controlador SH1107 utilizado pelo Unit OLED trabalha com organização interna que exige:

```c
0xA8, 0x7F
```

A diferença não era apenas um deslocamento de coluna.

O problema estava relacionado ao mapeamento interno de multiplexação do controlador.

Enquanto o display era inicializado com:

```text
A8 3F
```

a geometria interna não era corretamente representada.

A correção foi:

```text
A8 7F
```

---

## 4.3. Resultado da correção

Após a alteração:

- a inicialização do SH1107 passou a refletir a geometria correta;
- o clipping do último caractere desapareceu;
- a primeira coluna permaneceu utilizável;
- a última coluna passou a ser renderizada corretamente;
- palavras completas passaram a aparecer;
- o font rendering 5x7 foi validado;
- o display foi validado em hardware real.

Status técnico:

```text
SH1107 DRIVER
VALIDATED ON REAL HARDWARE
```

---

## 4.4. Arquitetura técnica preservada

A solução permaneceu totalmente nativa em ESP-IDF.

Foram preservados:

```text
ESP-IDF
driver/i2c_master
PaHub
custom oled_sh1107 component
```

Não foram introduzidos:

- Arduino;
- Arduino-ESP32;
- M5GFX;
- LovyanGFX;
- bibliotecas externas de display.

Isso mantém o firmware coerente com a arquitetura existente do Ambient Runtime.

---

## 4.5. Arquivos do driver

Os arquivos principais relacionados ao display permanecem em:

```text
firmware/nodes/ambient-runtime-node/
└── components/
    └── oled_sh1107/
        ├── oled_sh1107.c
        └── oled_sh1107.h
```

A documentação final deve revisar esses arquivos e registrar claramente:

- endereço I2C;
- canal do PaHub;
- geometria lógica;
- offsets;
- orientação;
- limitações;
- sequência de inicialização;
- contrato de desenho de texto.

---

# 5. Modificações no Cognitive Runtime / StackFlow

## 5.1. Situação anterior

O StackFlow já recebia o pacote de identidade pelo arquivo:

```text
runtime/cognitive/stackflow/identity_udp_listener.py
```

O fluxo existente era:

```text
Identity Package
        ↓
build_context()
        ↓
update_context()
        ↓
get_current_context()
        ↓
build_human_message()
        ↓
generate_semantic_events()
        ↓
Semantic Dispatcher
```

Esse pipeline já estava validado e não deveria ser redesenhado.

O objetivo foi adicionar o Ambient Context sem quebrar:

- Context Builder;
- Context Registry;
- Semantic Event Generator;
- Semantic Dispatcher;
- StackChan;
- RGB Strip;
- Ambient Runtime semantic notifier;
- Echo Pyramid;
- Runtime State Notifier.

---

## 5.2. Novo serviço criado

Foi criada a pasta:

```text
runtime/cognitive/stackflow/services/ambient_context/
```

Estrutura:

```text
ambient_context/
├── ambient_context_service.py
├── send_test_context.py
├── .env.example
├── ambient-context.service
├── README.md
└── MASTER_CLOSURE_REPORT.md
```

O arquivo principal é:

```text
ambient_context_service.py
```

Ele concentra:

- aquisição de dados externos;
- normalização;
- seleção do conteúdo pessoal;
- criação do contrato `ambient_context`;
- transmissão UDP para o Tab5.

---

## 5.3. Integração com o Identity Listener

A integração foi feita no arquivo:

```text
runtime/cognitive/stackflow/identity_udp_listener.py
```

Novo import:

```python
from services.ambient_context.ambient_context_service import (
    send_ambient_context,
)
```

Após a atualização do contexto:

```python
update_context(context)
current_context = get_current_context()
```

o perfil autenticado é obtido do pacote 5W:

```python
profile_id = current_context.get("who", {}).get("id", "unknown")
```

Em seguida, o serviço é chamado:

```python
send_ambient_context(
    profile_id=profile_id,
    tab5_host="192.168.77.25",
)
```

Essa chamada ocorre antes da continuação normal da geração de eventos semânticos.

Fluxo final:

```text
update_context()
        ↓
get_current_context()
        ↓
extract who.id
        ↓
send_ambient_context()
        ↓
continue semantic pipeline
```

---

## 5.4. Preservação do pipeline semântico

A integração do OLED não substituiu o fluxo existente.

Após a tentativa de envio do Ambient Context, o runtime continua normalmente:

```python
message = build_human_message(current_context)
semantic_events = generate_semantic_events(current_context)
```

Em seguida:

```text
Semantic Dispatcher
        ├── StackChan
        ├── RGB Strip
        ├── Ambient Runtime
        └── Echo Pyramid
```

Isso significa que o novo serviço foi adicionado de forma complementar.

O Ambient Context não interfere no contrato dos eventos semânticos.

---

## 5.5. Tratamento de falhas

A chamada do Ambient Context foi protegida por `try/except`.

Isso garante que uma falha em:

- internet;
- API externa;
- DNS;
- Tab5;
- socket UDP;
- credencial;
- parsing;

não interrompa o restante do Cognitive Runtime.

Princípio preservado:

```text
External context failure
must not stop
semantic event processing
```

Esse comportamento é importante para a robustez do sistema.

---

## 5.6. Relação com o Context Builder

O Context Builder gera o pacote semântico 5W.

O perfil autenticado fica em:

```python
current_context["who"]["id"]
```

A implementação final usa acesso defensivo:

```python
current_context.get("who", {}).get("id", "unknown")
```

Essa escolha evita falha por ausência de chave em pacotes incompletos.

---

# 6. APIs e fontes externas

## 6.1. Local das chamadas

Todas as chamadas de API ficam concentradas em:

```text
runtime/cognitive/stackflow/services/ambient_context/
└── ambient_context_service.py
```

O Tab5 não executa nenhuma chamada HTTP.

O `identity_udp_listener.py` também não contém lógica específica de API.

Ele apenas solicita:

```python
send_ambient_context(profile_id, tab5_host)
```

O serviço decide:

- qual fonte consultar;
- como interpretar os dados;
- como tratar falhas;
- como normalizar o resultado;
- como montar o JSON final.

---

## 6.2. Open-Meteo

Responsabilidade:

```text
contexto global
```

Dados utilizados:

- temperatura atual;
- condição meteorológica;
- índice UV máximo;
- localização configurada.

Conteúdo destinado a todos os perfis:

```text
Recife
Weather
Temperature
UV
```

A mesma chamada fornece informações de condição atual e previsão diária.

---

## 6.3. AwesomeAPI

Perfil associado:

```text
claudio
```

Conteúdo:

```text
USD/BRL
cotação
variação percentual
```

Exemplo:

```text
USD BRL
R$ 5.42
+0.31%
```

---

## 6.4. TheSportsDB

Perfil associado:

```text
herminio
```

Conteúdo:

```text
próximo jogo do Sport Club do Recife
```

O adaptador reduz o conteúdo da API para um formato curto compatível com o OLED.

---

## 6.5. Steam Storefront

Perfil associado:

```text
student
```

Conteúdo:

```text
uma promoção ou oferta destacada da Steam
```

O objetivo não é exibir uma lista completa, mas selecionar uma única informação relevante e compacta.

---

## 6.6. TMDB

Perfil associado:

```text
mariana
```

Conteúdo:

```text
filme em cartaz no Brasil
```

A integração depende de:

```text
TMDB_BEARER_TOKEN
```

A credencial deve ficar no ambiente Linux e nunca no firmware do Tab5.

Importante:

```text
TMDB now playing in Brazil
```

não significa necessariamente programação específica de cinemas do Recife.

Caso o projeto futuramente exija agenda local, a fonte poderá ser substituída sem mudar o contrato enviado ao Tab5.

---

# 7. Normalização dos dados

As APIs externas possuem formatos completamente diferentes.

O serviço converte todas para um contrato único.

Exemplo:

```json
"personal": {
  "available": true,
  "title": "USD BRL",
  "value": "R$ 5.42",
  "secondary": "+0.31%"
}
```

Independentemente da origem, o Tab5 recebe apenas:

```text
title
value
secondary
```

Isso permite trocar uma API sem alterar:

- semantic_event_receiver;
- ambient_context_snapshot;
- oled_context_presenter;
- driver SH1107.

---

# 8. Graceful degradation

O sistema foi projetado para continuar funcionando quando uma fonte externa falha.

Exemplos:

```text
Open-Meteo indisponível
→ global.available = false
```

```text
AwesomeAPI indisponível
→ personal.available = false
```

```text
TMDB sem token
→ personal.available = false
```

A falha de uma seção não invalida obrigatoriamente a outra.

O Tab5 pode receber:

```text
global válido
personal indisponível
```

ou:

```text
global indisponível
personal válido
```

Esse comportamento evita que uma única API comprometa o OLED inteiro.

---

# 9. Ferramentas de teste

## 9.1. send_test_context.py

Arquivo:

```text
runtime/cognitive/stackflow/services/ambient_context/send_test_context.py
```

Finalidade:

- enviar mensagens determinísticas;
- validar o contrato UDP;
- testar o Tab5 sem depender de internet;
- testar cada perfil;
- validar o OLED;
- isolar erros de rede, parsing e apresentação.

Exemplo:

```bash
python3 send_test_context.py 192.168.77.25 --profile claudio
```

---

## 9.2. Dry run

O serviço pode consultar APIs e imprimir o resultado sem transmitir ao Tab5.

Exemplo:

```bash
python3 ambient_context_service.py \
  --profile claudio \
  --once \
  --dry-run
```

Esse modo é útil para distinguir:

```text
problema de API
```

de:

```text
problema de UDP ou OLED
```

---

## 9.3. Teste de envio direto

O serviço também pode transmitir um contexto completo:

```bash
python3 ambient_context_service.py \
  --tab5-host 192.168.77.25 \
  --profile claudio \
  --once
```

---

# 10. Deploy para o AX630C

O script de deploy foi atualizado para copiar toda a árvore do StackFlow.

Em vez de manter uma lista manual de arquivos, o deploy passou a utilizar cópia recursiva:

```bat
scp -r "%LOCAL%\." "%HOST%:%REMOTE%/"
```

Isso garante o envio de:

- módulos principais;
- adaptadores;
- serviços;
- `services/ambient_context`;
- documentação;
- arquivos auxiliares.

O script também:

- testa a conexão SSH;
- cria o diretório remoto;
- remove `__pycache__`;
- remove arquivos `.pyc`;
- executa `py_compile` no servidor;
- diferencia erro de cópia e erro de validação.

Arquivo de deploy atualizado:

```text
tools/deploy_runtime_ax630c_full.bat
```

A localização final deve ser confirmada durante a revisão do repositório.

---

# 11. Validações executadas

Foram executadas validações em diferentes níveis.

## 11.1. Sintaxe Python

```text
ambient_context_service.py
→ PASS
```

```text
identity_udp_listener.py
→ PASS
```

---

## 11.2. Importação do serviço

O módulo foi preparado para ser importado diretamente pelo listener.

Resultado esperado e validado no fluxo atual:

```text
from services.ambient_context.ambient_context_service import send_ambient_context
```

---

## 11.3. Comunicação UDP

O Tab5 recebeu mensagens do tipo:

```text
ambient_context
```

Status:

```text
Ambient context consumed
```

---

## 11.4. OLED

Foram validados:

- texto completo;
- ausência de clipping;
- bloco global;
- bloco pessoal;
- identificação do perfil;
- atualização do conteúdo.

---

## 11.5. Integração com identidade

O fluxo atual passou a permitir:

```text
NFC authentication
        ↓
profile_id
        ↓
Ambient Context generation
        ↓
Tab5 update
```

Isso remove a necessidade de executar manualmente um script para selecionar cada perfil durante o uso normal.

---

# 12. Arquivos principais envolvidos

## Ambient Runtime — Tab5

```text
firmware/nodes/ambient-runtime-node/
├── main/
└── components/
    ├── semantic_event_receiver/
    ├── ambient_context_snapshot/
    ├── oled_context_presenter/
    └── oled_sh1107/
        ├── oled_sh1107.c
        └── oled_sh1107.h
```

Os nomes e caminhos exatos devem ser confirmados na revisão final da árvore do repositório.

---

## Cognitive Runtime — StackFlow

```text
runtime/cognitive/stackflow/
├── identity_udp_listener.py
├── context_builder.py
├── context_registry.py
├── semantic_event_generator.py
├── semantic_dispatcher.py
└── services/
    └── ambient_context/
        ├── ambient_context_service.py
        ├── send_test_context.py
        ├── .env.example
        ├── ambient-context.service
        ├── README.md
        └── MASTER_CLOSURE_REPORT.md
```

---

# 13. Decisões arquiteturais importantes

## 13.1. APIs permanecem no AX630C

Razões:

- proteção de credenciais;
- maior facilidade para atualizar adaptadores;
- menor complexidade no firmware;
- normalização centralizada;
- melhor observabilidade;
- menor acoplamento com o Tab5.

---

## 13.2. Tab5 permanece independente das APIs

O Tab5 conhece apenas o contrato:

```text
ambient_context
```

Ele não conhece:

- endpoints;
- tokens;
- schemas externos;
- regras de seleção de perfil;
- tratamento HTTP.

---

## 13.3. Serviço acionado por autenticação

O fluxo atual usa o evento real de identidade.

Isso é superior a manter um serviço independente permanentemente configurado para um único perfil.

O modo contínuo e o arquivo `ambient-context.service` podem permanecer como recurso de laboratório, teste ou futura extensão, mas não representam necessariamente o fluxo principal de produção.

---

## 13.4. Sem redesign do Semantic Dispatcher

A integração foi adicionada sem alterar contratos já validados.

Essa decisão reduziu risco e preservou a estabilidade da Fase 1.

---

# 14. Pontos para revisão documental posterior

Durante a revisão do README e do relatório do MASTER, verificar:

1. atualizar o status de “aguardando validação” para o estado atual;
2. documentar a integração automática com `identity_udp_listener.py`;
3. diferenciar modo de teste isolado e modo de produção;
4. revisar a utilidade do arquivo `ambient-context.service`;
5. registrar a correção definitiva do SH1107;
6. confirmar caminhos reais dos componentes do Tab5;
7. confirmar se todos os perfis foram validados com API real;
8. confirmar o estado da credencial TMDB;
9. adicionar logs finais de validação;
10. registrar commit e hash de fechamento;
11. revisar IPs fixos e variáveis de ambiente;
12. revisar README para banca e reprodutibilidade;
13. remover instruções antigas que contradigam o fluxo atual;
14. confirmar se `MASTER_CLOSURE_REPORT.md` permanece nessa pasta ou será movido para documentação de integração;
15. confirmar o nome final desta nota dentro da pasta `notes`.

---

# 15. Local recomendado no repositório

Conforme decisão do projeto, esta nota deve ficar na pasta de notas do Ambient Runtime Tab5.

Sugestão:

```text
firmware/nodes/ambient-runtime-node/notes/
└── MINI_OLED_AMBIENT_CONTEXT_TECHNICAL_NOTE.md
```

Se a estrutura real utilizar outro diretório para notas do Ambient Runtime, preservar a estrutura existente e evitar criar uma nova pasta fora do padrão atual.

---

# 16. Estado técnico atual

```text
AMBIENT RUNTIME TAB5
FUNCTIONAL

SH1107 DRIVER
CORRECTED AND VALIDATED

MINI OLED RENDERING
VALIDATED

AMBIENT CONTEXT CONTRACT
VALIDATED

STACKFLOW SERVICE
IMPLEMENTED

IDENTITY-DRIVEN CONTEXT GENERATION
INTEGRATED

UDP DELIVERY TO TAB5
VALIDATED

EXISTING SEMANTIC PIPELINE
PRESERVED

DOCUMENTATION
PENDING FINAL REVIEW
```

---

# 17. Conclusão

O trabalho realizado transformou o Mini OLED de um periférico parcialmente funcional em uma interface contextual integrada ao ecossistema Ambient Physical AI.

A solução final mantém a separação correta de responsabilidades:

```text
AX630C
→ compreender o perfil
→ consultar fontes externas
→ normalizar o contexto
→ enviar o resultado
```

```text
Tab5
→ receber
→ armazenar
→ formatar
→ apresentar
```

A correção do driver SH1107 eliminou o problema de clipping sem introduzir bibliotecas externas ou alterar a base ESP-IDF do projeto.

A criação do Ambient Context Service adicionou uma camada clara e modular de aquisição e normalização de dados externos.

A integração com o `identity_udp_listener.py` conectou o serviço ao fluxo real de autenticação, permitindo que o conteúdo do OLED acompanhe automaticamente o usuário identificado.

O pipeline semântico existente foi preservado.

A arquitetura geral não foi redesenhada.

O próximo passo recomendado não é desenvolver uma nova solução, mas revisar com cuidado:

- README;
- relatório do MASTER;
- documentação de deploy;
- credenciais;
- caminhos de arquivos;
- commits;
- evidências finais de validação.

Esta nota deve ser usada como base técnica para essa revisão.

---

## Status da nota

```text
TECHNICAL NOTE
CREATED

PURPOSE
DOCUMENT THE CURRENT IMPLEMENTATION BEFORE FINAL DOCUMENTATION REVIEW

PROJECT
AMBIENT PHYSICAL AI

SUBSYSTEM
AMBIENT RUNTIME TAB5 + MINI OLED + STACKFLOW AMBIENT CONTEXT
```
