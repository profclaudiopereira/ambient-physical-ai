# AX630C + StackFlow Lab

## Linux Command Reference

Este documento registra os principais comandos utilizados durante a fase de Bring-Up e Discovery do AX630C.

---

# Acesso ao Sistema

## Entrar no Linux via ADB

```bash
adb shell
```

### Objetivo

Abrir um terminal Linux diretamente no AX630C.

### Resultado Esperado

```text
sh-5.1#
```

---

# Informações do Sistema

## Usuário Atual

```bash
whoami
```

### Objetivo

Identificar o usuário logado.

### Exemplo

```text
root
```

---

## Informações do Kernel

```bash
uname -a
```

### Objetivo

Exibir:

* Kernel Linux
* Arquitetura
* Hostname

### Exemplo

```text
Linux m5stack-LLM 4.19.125 ...
```

---

## Informações do Sistema Operacional

```bash
cat /etc/os-release
```

### Objetivo

Identificar a distribuição Linux instalada.

### Exemplo

```text
Ubuntu 22.04 LTS
```

---

## Diretório Atual

```bash
pwd
```

### Objetivo

Exibir o diretório atual.

### Exemplo

```text
/
```

---

## Listar Arquivos

```bash
ls
```

### Objetivo

Listar arquivos e diretórios.

---

# Rede

## Interfaces de Rede

```bash
ip addr
```

### Objetivo

Exibir:

* interfaces disponíveis
* endereços IP
* estado da interface

### Exemplo

```text
eth0
lo
```

---

## Interface Específica

```bash
ip addr show eth0
```

### Objetivo

Verificar status da Ethernet.

### Antes do cabo

```text
NO-CARRIER
```

### Depois do cabo

```text
LOWER_UP
```

---

## Rotas

```bash
ip route
```

### Objetivo

Exibir:

* gateway padrão
* redes configuradas

### Exemplo

```text
default via 192.168.77.1
```

---

## Estado do Link

```bash
ip link show eth0
```

### Objetivo

Verificar status físico da interface.

---

## Teste de Gateway

```bash
ping -c 4 192.168.77.1
```

### Objetivo

Validar comunicação com o roteador.

---

## Teste de Internet

```bash
ping -c 4 github.com
```

### Objetivo

Validar:

* DNS
* Internet

---

# Armazenamento

## Espaço em Disco

```bash
df -h
```

### Objetivo

Verificar:

* armazenamento total
* espaço livre

### Exemplo

```text
28G total
25G livre
```

---

# Memória

## RAM

```bash
free -h
```

### Objetivo

Verificar:

* memória total
* memória livre

### Exemplo

```text
958 MiB
```

---

# Serviços

## Serviços Ativos

```bash
systemctl --type=service --state=running
```

### Objetivo

Listar serviços em execução.

### Descobertas

```text
llm_asr
llm_audio
llm_camera
llm_kws
llm_llm
llm_melotts
llm_skel
llm_sys
llm_tts
llm_vlm
llm_yolo
```

---

## Status de Serviço

```bash
systemctl status llm-llm --no-pager
```

### Objetivo

Inspecionar um serviço específico.

---

# Processos

## Listar Processos

```bash
ps -ef
```

### Objetivo

Visualizar todos os processos.

---

## Filtrar Runtime LLM

```bash
ps -ef | grep llm
```

### Objetivo

Localizar serviços StackFlow.

---

# Pacotes Instalados

## Pacotes LLM

```bash
dpkg -l | grep -i llm
```

### Objetivo

Identificar modelos e componentes instalados.

### Descobertas

```text
llm-qwen2.5-0.5b-prefill-20e
llm-asr
llm-tts
llm-vlm
llm-yolo
```

---

# Portas TCP

## Portas em Escuta

```bash
ss -lntp
```

### Objetivo

Descobrir serviços TCP.

### Descobertas

```text
22     SSH
23     Telnet
10001  llm_sys
```

---

## Portas UDP

```bash
ss -lnup
```

### Objetivo

Descobrir serviços UDP.

---

# Arquivos de Configuração

## Configuração Principal

```bash
cat /opt/m5stack/share/sys_config.json
```

### Objetivo

Verificar parâmetros globais.

### Descoberta

```json
{
  "config_enable_tcp": 1
}
```

---

# Modelos

## Configuração do Modelo

```bash
cat /opt/m5stack/data/models/mode_qwen2.5-0.5B-prefill-20e.json
```

### Objetivo

Inspecionar:

* capacidades
* entradas
* saídas
* tokenizer
* arquivos axmodel

---

# Busca de Arquivos

## Procurar Arquivos

```bash
find /opt/m5stack -type f
```

### Objetivo

Explorar a instalação.

---

## Buscar Arquivos de Modelo

```bash
find /opt -type f | grep -i qwen
```

### Objetivo

Localizar modelos Qwen.

---

## Buscar Configurações

```bash
find /opt -type f | grep -i json
```

### Objetivo

Localizar arquivos de configuração.

---

# Logs

## Últimos Eventos

```bash
journalctl -u llm-llm -n 50 --no-pager
```

### Objetivo

Analisar logs da LLM.

### Descoberta

```text
LLM init ok
load_mode success
```

---

# Encerramento Seguro

## Sincronizar Escritas

```bash
sync
```

### Objetivo

Garantir gravação dos buffers.

---

## Desligar Sistema

```bash
poweroff
```

ou

```bash
shutdown -h now
```

### Objetivo

Desligar o Ubuntu corretamente.

---

# Comandos Mais Importantes Descobertos

```bash
adb shell
ip addr
ip route
ping github.com
df -h
free -h
systemctl --type=service --state=running
dpkg -l | grep -i llm
ps -ef | grep llm
ss -lntp
cat /opt/m5stack/share/sys_config.json
journalctl -u llm-llm -n 50 --no-pager
poweroff
```

Esses comandos formam o kit básico de diagnóstico do AX630C + StackFlow.
