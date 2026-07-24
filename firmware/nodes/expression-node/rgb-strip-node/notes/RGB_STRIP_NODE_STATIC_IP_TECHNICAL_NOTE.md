# RGB Strip Node — Static IP Migration

## Ambient Physical AI

**Document type:** Temporary Engineering Note

---

## Purpose

This document records the firmware modification that migrated the RGB Strip Node from DHCP-based addressing to a fixed IPv4 configuration. It is intended as a temporary engineering record before the information is incorporated into the node README.

---

## Modified Source File

```text
wifi_station.c
```

The network initialization component was modified to replace automatic DHCP address assignment with a deterministic static network configuration. fileciteturn35file0

---

## Engineering Change

### Previous behavior

The firmware initialized the Wi-Fi station and obtained its IPv4 address from the DHCP server.

```text
DHCP
    ↓
Dynamic IPv4
```

This allowed the address to change after reboots or network reconfiguration.

### New behavior

The firmware now:

1. Stops the DHCP client.
2. Creates a fixed IPv4 configuration.
3. Applies the configuration before connecting to Wi-Fi.

The implementation performs:

```c
esp_netif_dhcpc_stop(sta_netif);
esp_netif_set_ip_info(sta_netif, &ip_info);
```

before starting the Wi-Fi connection. fileciteturn35file0

---

## Current Network Configuration

```text
RGB Strip Node

IPv4    : 192.168.77.26
Gateway : 192.168.77.1
Mask    : 255.255.255.0
```

---

## Motivation

Using fixed addresses provides:

- deterministic deployment;
- easier integration;
- simpler packet tracing;
- reproducible demonstrations;
- stable destination addressing for StackFlow components.

---

## Functional Impact

Only the network initialization changed.

No modifications were made to:

- RGB rendering logic;
- semantic event processing;
- UDP protocol;
- expression behavior.

---

## Repository Status

This note is intentionally temporary.

During the next documentation revision its contents should be incorporated into:

```text
firmware/nodes/expression-node/.../README.md
```

and this file can then be removed.

---

## Status

**Completed**

The RGB Strip Node now boots with a deterministic static IPv4 configuration instead of relying on DHCP.
