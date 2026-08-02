# NOTE_001_STATIC_IP_CONFIGURATION

## Ambient Physical AI

### RGB Strip Node

---

# Purpose

This document records the engineering modification that migrated the RGB Strip Node from DHCP-based network addressing to a deterministic static IPv4 configuration.

The objective of this change was to provide a stable network configuration for deployment, integration and reproducible demonstrations within the Ambient Physical AI distributed ecosystem.

---

# Modified Source File

```text
main/
└── wifi_station.c
```

The network initialization component was modified to replace automatic DHCP address assignment with a fixed IPv4 configuration.

---

# Engineering Change

## Previous Behavior

The firmware initialized the Wi-Fi Station and obtained its IPv4 address dynamically from the DHCP server.

```text
DHCP
    │
    ▼
Dynamic IPv4 Address
```

Although functional, the assigned address could change after reboots or network reconfiguration.

---

## New Behavior

The firmware now performs the following sequence:

1. Stops the DHCP client.
2. Creates a fixed IPv4 configuration.
3. Applies the configuration before initiating the Wi-Fi connection.

The implementation performs:

```c
esp_netif_dhcpc_stop(sta_netif);

esp_netif_set_ip_info(sta_netif, &ip_info);
```

before starting the Wi-Fi connection process.

---

# Current Network Configuration

```text
RGB Strip Node

IPv4    : 192.168.77.26
Gateway : 192.168.77.1
Mask    : 255.255.255.0
```

---

# Motivation

Using a deterministic IPv4 configuration provides several engineering advantages:

- deterministic deployment;
- easier distributed system integration;
- simpler packet tracing during debugging;
- reproducible demonstrations;
- stable destination addressing for StackFlow components.

These characteristics simplify development, validation and long-term maintenance of the distributed system.

---

# Functional Impact

Only the network initialization process was modified.

No changes were made to:

- RGB rendering logic;
- semantic event processing;
- UDP communication;
- expression behavior;
- software architecture.

The modification is isolated to the Wi-Fi initialization stage.

---

# Validation

The implementation was successfully validated.

The firmware now:

- initializes the Wi-Fi Station normally;
- applies the configured static IPv4 address;
- connects successfully to the wireless network;
- receives Semantic Events over UDP;
- preserves normal operation of the Expression Layer.

No functional regressions were observed after the migration.

---

# Status

```text
STATUS

Completed

Validated
```

The RGB Strip Node now boots using a deterministic static IPv4 configuration instead of relying on DHCP address assignment.