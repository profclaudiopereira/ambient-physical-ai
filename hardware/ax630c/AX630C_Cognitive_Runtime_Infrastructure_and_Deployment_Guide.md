# AX630C Cognitive Runtime Infrastructure and Deployment Guide

**Project:** Ambient Physical AI\
**Platform:** AX630C + LLM Mate\
**Operating System:** Ubuntu 22.04 LTS (Jammy Jellyfish)\
**Kernel:** Linux 4.19.125 (ARM64)\
**Python:** 3.10.12

------------------------------------------------------------------------

# 1. Purpose

This document describes the infrastructure required to transform a
factory-installed AX630C/LLM Mate into the Cognitive Runtime node used
by the Ambient Physical AI project.

It documents the actual production configuration validated during
development rather than a generic Ubuntu installation.

------------------------------------------------------------------------

# 2. Platform Baseline

-   Hardware: AX630C + LLM Mate
-   OS: Ubuntu 22.04 LTS
-   Kernel: Linux 4.19.125
-   Architecture: ARM64
-   Hostname: `m5stack-LLM`

------------------------------------------------------------------------

# 3. Runtime Architecture

    Power On
        │
    Linux Kernel
        │
    systemd
        │
        ├── networking.service
        │        │
        │        └── /etc/network/interfaces
        │
        ├── rc-local.service
        │        ├── M5Stack hardware initialization
        │        ├── Ethernet tuning
        │        ├── NPU configuration
        │        └── Platform services
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
                 ├── StackChan MCP Server
                 └── Shared Context Registry

------------------------------------------------------------------------

# 4. Runtime Installation Directory

    /root/ambient-runtime/runtime/cognitive/stackflow

Main entry point:

    identity_udp_listener.py

------------------------------------------------------------------------

# 5. Network Infrastructure

The platform uses the traditional **ifupdown** stack.

It does **not** use:

-   Netplan
-   NetworkManager
-   systemd-networkd

Persistent configuration:

    /etc/network/interfaces

Validated configuration:

``` text
auto eth0
allow-hotplug eth0
iface eth0 inet static
    address 192.168.77.15
    netmask 255.255.255.0
    gateway 192.168.77.1
    dns-nameservers 192.168.77.1 8.8.8.8
```

------------------------------------------------------------------------

# 6. Automatic Startup

The Cognitive Runtime is managed by **systemd**.

Service:

    /etc/systemd/system/ambient-cognitive-runtime.service

Configuration:

``` ini
[Unit]
Description=Ambient Physical AI Cognitive Runtime
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=root
Group=root
WorkingDirectory=/root/ambient-runtime/runtime/cognitive/stackflow
ExecStart=/usr/bin/python3 -u /root/ambient-runtime/runtime/cognitive/stackflow/identity_udp_listener.py
Restart=always
RestartSec=2

StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

Engineering decisions:

-   Wait for network before starting.
-   Automatic restart on failure.
-   Journald logging.
-   Dedicated service independent of `rc.local`.

------------------------------------------------------------------------

# 7. Runtime Startup Sequence

Validated boot log:

1.  Ambient Physical AI Cognitive Runtime
2.  Identity UDP Listener
3.  UDP Port 4444
4.  Context Builder
5.  Semantic Event Generator
6.  Semantic Dispatcher
7.  StackChan Notifier
8.  RGB Strip Notifier
9.  Ambient Runtime Notifier
10. Runtime State Notifier
11. StackChan MCP Server
12. Shared Context Registry

------------------------------------------------------------------------

# 8. Deployment Strategy

Runtime updates are performed incrementally.

Workflow:

    Developer PC
          │
    deploy_runtime_ax630c.bat
          │
          ▼
    ADB / SCP
          │
          ▼
    AX630C
          │
          ▼
    Only updated runtime modules are replaced

This minimizes deployment time and preserves the remaining runtime
environment.

------------------------------------------------------------------------

# 9. Platform-Specific Initialization

`rc.local` is reserved for native platform initialization.

It performs:

-   Driver loading
-   NPU initialization
-   Ethernet tuning
-   USB ADB initialization
-   Native M5Stack services

The Ambient Physical AI runtime is **not** started from `rc.local`.

------------------------------------------------------------------------

# 10. Validation Checklist

-   Ubuntu boot completed
-   Ethernet configured
-   Static IP reachable
-   DNS operational
-   `ambient-cognitive-runtime.service` active
-   UDP port 4444 listening
-   Runtime Console receiving status
-   Semantic dispatcher initialized

------------------------------------------------------------------------

# 11. Operational Recommendations

-   Keep `/etc/network/interfaces` under version control
    (documentation).
-   Backup service files before updates.
-   Prefer incremental deployment.
-   Use `journalctl -u ambient-cognitive-runtime.service` for
    diagnostics.

## Security Note

The collected system showed:

    /etc/systemd/system/ambient-cognitive-runtime.service

with permissions `666` (world writable).

Recommended permissions:

``` bash
chmod 644 /etc/systemd/system/ambient-cognitive-runtime.service
systemctl daemon-reload
```

------------------------------------------------------------------------

# 12. Conclusion

The Ambient Physical AI Cognitive Runtime is deployed as a dedicated
Linux service managed by `systemd`, running independently from the
native M5Stack services while integrating with them. The resulting
architecture provides deterministic startup, automatic recovery after
reboot or power loss, and a reproducible infrastructure suitable for
engineering validation and competition demonstrations.
