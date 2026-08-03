# AX630C + StackFlow Lab

## Linux Command Reference

This document records the main commands used during the AX630C Bring-Up and Discovery phase.

---

# System Access

## Access Linux via ADB

```bash
adb shell
```

### Purpose

Open a Linux terminal directly on the AX630C.

### Expected Result

```text
sh-5.1#
```

---

# System Information

## Current User

```bash
whoami
```

### Purpose

Identify the currently logged-in user.

### Exemplo

```text
root
```

---

## Kernel Information

```bash
uname -a
```

### Purpose

Display:

* Kernel Linux
* Architecture
* Hostname

### Exemplo

```text
Linux m5stack-LLM 4.19.125 ...
```

---

## System Information Operacional

```bash
cat /etc/os-release
```

### Purpose

Identify the installed Linux distribution.

### Exemplo

```text
Ubuntu 22.04 LTS
```

---

## Current Directory

```bash
pwd
```

### Purpose

Display the current working directory.

### Exemplo

```text
/
```

---

## List Files

```bash
ls
```

### Purpose

List files and directories.

---

# Networking

## Interfaces de Networking

```bash
ip addr
```

### Purpose

Display:

* available interfaces
* IP addresses
* interface status

### Exemplo

```text
eth0
lo
```

---

## Specific Interface

```bash
ip addr show eth0
```

### Purpose

Check Ethernet status.

### Before connecting the cable

```text
NO-CARRIER
```

### After connecting the cable

```text
LOWER_UP
```

---

## Routing

```bash
ip route
```

### Purpose

Display:

* default gateway
* configured networks

### Exemplo

```text
default via 192.168.77.1
```

---

## Link Status

```bash
ip link show eth0
```

### Purpose

Check the physical interface status.

---

## Gateway Test

```bash
ping -c 4 192.168.77.1
```

### Purpose

Validate communication with the router.

---

## Internet connectivity Connectivity Test

```bash
ping -c 4 github.com
```

### Purpose

Validate:

* DNS
* Internet connectivity

---

# Storage

## Disk Space

```bash
df -h
```

### Purpose

Check:

* total storage
* free space

### Exemplo

```text
28G total
25G free
```

---

# Memory

## RAM

```bash
free -h
```

### Purpose

Check:

* memória total
* memória free

### Exemplo

```text
958 MiB
```

---

# Services

## Services Ativos

```bash
systemctl --type=service --state=running
```

### Purpose

List running services.

### Findings

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

## Service Status

```bash
systemctl status llm-llm --no-pager
```

### Purpose

Inspect a specific service.

---

# Processes

## Listar Processes

```bash
ps -ef
```

### Purpose

View all running processes.

---

## Filter LLM Runtime

```bash
ps -ef | grep llm
```

### Purpose

Locate StackFlow services.

---

# Installed Packages

## LLM Packages

```bash
dpkg -l | grep -i llm
```

### Purpose

Identify installed models and components.

### Findings

```text
llm-qwen2.5-0.5b-prefill-20e
llm-asr
llm-tts
llm-vlm
llm-yolo
```

---

# TCP Ports

## Listening Ports

```bash
ss -lntp
```

### Purpose

Discover TCP services.

### Findings

```text
22     SSH
23     Telnet
10001  llm_sys
```

---

## UDP Ports

```bash
ss -lnup
```

### Purpose

Discover UDP services.

---

# Configuration Files

## Main Configuration

```bash
cat /opt/m5stack/share/sys_config.json
```

### Purpose

Inspect global parameters.

### Descoberta

```json
{
  "config_enable_tcp": 1
}
```

---

# Modelos

## Model Configuration

```bash
cat /opt/m5stack/data/models/mode_qwen2.5-0.5B-prefill-20e.json
```

### Purpose

Inspect:

* capabilities
* inputs
* outputs
* tokenizer
* arquivos axmodel

---

# File Search

## Search Files

```bash
find /opt/m5stack -type f
```

### Purpose

Explore the installation.

---

## Search Model Files

```bash
find /opt -type f | grep -i qwen
```

### Purpose

Locate Qwen models.

---

## Search Configuration Files

```bash
find /opt -type f | grep -i json
```

### Purpose

Locate configuration files.

---

# Logs

## Recent Events

```bash
journalctl -u llm-llm -n 50 --no-pager
```

### Purpose

Analyze LLM logs.

### Descoberta

```text
LLM init ok
load_mode success
```

---

# Safe Shutdown

## Flush File System Buffers

```bash
sync
```

### Purpose

Ensure pending writes are flushed.

---

## Shut Down the System

```bash
poweroff
```

ou

```bash
shutdown -h now
```

### Purpose

Shut down Ubuntu properly.

---

# Most Important Commands Identified

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

These commands provide the essential diagnostic toolkit for the AX630C + StackFlow platform.
