# AX630C Cognitive Runtime Infrastructure and Deployment Guide (Version 2)

> **Ambient Physical AI -- Infrastructure Documentation**
>
> This document extends Version 1 by preserving all previously
> documented architecture while adding practical server administration
> procedures required to configure a new AX630C from scratch.

------------------------------------------------------------------------

# 1. Purpose

This guide documents the complete infrastructure required to transform a
factory-installed AX630C/LLM Mate into the Cognitive Runtime used by
Ambient Physical AI.

It includes:

-   Platform baseline
-   Network configuration
-   Static IP configuration
-   Runtime deployment
-   Automatic startup
-   Service management
-   Linux administration commands
-   Validation and troubleshooting

------------------------------------------------------------------------

# 2. Platform Baseline

-   Hardware: AX630C + LLM Mate
-   OS: Ubuntu 22.04 LTS
-   Kernel: Linux 4.19.125
-   Architecture: ARM64
-   Python: 3.10.12
-   Hostname: `m5stack-LLM`

------------------------------------------------------------------------

# 3. Runtime Architecture

    Power
     │
     ▼
    Linux Kernel
     │
     ▼
    systemd
     │
     ├── networking.service
     │       │
     │       └── /etc/network/interfaces
     │
     ├── rc-local.service
     │
     ├── Native llm_* services
     │
     └── ambient-cognitive-runtime.service
             │
             ▼
     identity_udp_listener.py
             │
             ├── Context Builder
             ├── Semantic Event Generator
             ├── Semantic Dispatcher
             ├── StackChan Notifier
             ├── RGB Strip Notifier
             ├── Ambient Runtime Notifier
             ├── Runtime State Notifier
             └── Shared Context Registry

------------------------------------------------------------------------

# 4. Runtime Directory

    /root/ambient-runtime/runtime/cognitive/stackflow

Entry point:

    identity_udp_listener.py

------------------------------------------------------------------------

# 5. Network Configuration

The validated system uses **ifupdown**.

It does **not** use:

-   Netplan
-   NetworkManager
-   systemd-networkd

The active configuration file is:

    /etc/network/interfaces

Validated configuration:

``` ini
auto lo
iface lo inet loopback

auto eth0
allow-hotplug eth0
iface eth0 inet static
    address 192.168.77.15
    netmask 255.255.255.0
    gateway 192.168.77.1
    dns-nameservers 192.168.77.1 8.8.8.8
```

## Changing the Static IP

1.  Edit the configuration:

``` bash
nano /etc/network/interfaces
```

2.  Modify:

-   address
-   netmask
-   gateway
-   dns-nameservers

3.  Save the file.

4.  Restart networking:

``` bash
systemctl restart networking
```

or reboot:

``` bash
reboot
```

5.  Validate:

``` bash
ip addr
ip route
ping 192.168.77.1
```

------------------------------------------------------------------------

# 6. Useful Network Commands

``` bash
hostnamectl
ip addr
ip route
cat /etc/network/interfaces
cat /etc/resolv.conf
resolvectl status
ping 8.8.8.8
ping github.com
```

------------------------------------------------------------------------

# 7. Automatic Startup

The runtime is managed by:

    /etc/systemd/system/ambient-cognitive-runtime.service

``` ini
[Unit]
After=network-online.target
Wants=network-online.target

[Service]
WorkingDirectory=/root/ambient-runtime/runtime/cognitive/stackflow
ExecStart=/usr/bin/python3 -u /root/ambient-runtime/runtime/cognitive/stackflow/identity_udp_listener.py
Restart=always
RestartSec=2
```

------------------------------------------------------------------------

# 8. Service Management

Useful commands:

``` bash
systemctl status ambient-cognitive-runtime.service
systemctl start ambient-cognitive-runtime.service
systemctl stop ambient-cognitive-runtime.service
systemctl restart ambient-cognitive-runtime.service
systemctl enable ambient-cognitive-runtime.service
systemctl disable ambient-cognitive-runtime.service
systemctl is-active ambient-cognitive-runtime.service
systemctl is-enabled ambient-cognitive-runtime.service
journalctl -u ambient-cognitive-runtime.service -f
```

------------------------------------------------------------------------

# 9. Deployment

Deployment is performed incrementally using the project deployment
script.

Workflow:

    Developer PC
          │
    deploy_runtime_ax630c.bat
          │
    ADB / SCP
          │
    AX630C
          │
    Updated runtime modules

------------------------------------------------------------------------

# 10. Basic Linux Administration Commands

## File system

``` bash
pwd
ls -la
cd
find .
du -sh .
```

## Files

``` bash
cat file
nano file
cp
mv
rm
chmod
chown
```

## Processes

``` bash
ps -ef
top
grep
kill
```

## Services

``` bash
systemctl status
systemctl list-units
journalctl -b
```

## Network

``` bash
hostnamectl
ip addr
ip route
ping
```

## Python

``` bash
python3 --version
pip3 list
```

------------------------------------------------------------------------

# 11. Validation Checklist

-   Ethernet configured
-   Static IP reachable
-   DNS working
-   Runtime service enabled
-   Runtime active
-   UDP listener running
-   Runtime Console receiving updates

------------------------------------------------------------------------

# 12. Security Recommendation

The collected system used:

    /etc/systemd/system/ambient-cognitive-runtime.service

with permission 666.

Recommended:

``` bash
chmod 644 /etc/systemd/system/ambient-cognitive-runtime.service
systemctl daemon-reload
```

------------------------------------------------------------------------

# 13. Conclusion

This document serves as both an infrastructure reference and an
operational guide for reproducing the validated AX630C Cognitive Runtime
environment used by Ambient Physical AI, including network
personalization, automatic startup, deployment, and routine Linux
administration.
