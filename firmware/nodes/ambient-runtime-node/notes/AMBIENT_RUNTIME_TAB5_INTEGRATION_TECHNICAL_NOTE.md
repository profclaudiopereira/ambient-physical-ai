# Ambient Runtime (Tab5) — Integration Technical Note

## Ambient Physical AI

**Temporary Engineering Note**

---

## Purpose

This note records the engineering work performed during the integration between the AX630C Cognitive Runtime and the Ambient Runtime (Tab5). It is intended to preserve the implementation history before the information is consolidated into the official README.

---

## Summary of the Work

This milestone involved changes on **both sides of the integration**:

1. **Ambient Runtime (ESP32-P4 / Tab5)**
   - Modified the network component to migrate from DHCP to a fixed IPv4 configuration.
   - Configured the firmware to stop the DHCP client and apply a static address during initialization.
   - Target address:
     - IP: **192.168.77.25**
     - Gateway: **192.168.77.1**
     - Mask: **255.255.255.0**

   The implementation performs `esp_netif_dhcpc_stop()` followed by `esp_netif_set_ip_info()` before starting the Wi-Fi station. fileciteturn36file0

2. **AX630C Cognitive Runtime**
   - Added a dedicated **Ambient Runtime notifier** (`ambient_runtime_notifier.py`).
   - Registered this notifier in the `Identity UDP Listener` so that Semantic Events are dispatched to the Tab5 in addition to the existing destinations. fileciteturn36file2 fileciteturn36file1

---

## Root Cause Investigation

After assigning a fixed IP to the Tab5, the interface still remained in a waiting state.

The investigation showed that **network connectivity was not the remaining problem**.

The missing piece was that the AX630C had no delivery adapter dedicated to the Ambient Runtime. Although semantic events were generated, there was no component responsible for forwarding them to the Tab5.

To resolve this:

- `ambient_runtime_notifier.py` was created as a dedicated StackFlow adapter.
- `identity_udp_listener.py` was updated to instantiate and register this notifier with the `SemanticDispatcher`.
- Successful and failed deliveries are now reported separately for the Ambient Runtime path. fileciteturn36file2 fileciteturn36file1

---

## Integration Flow

```text
Presence Node
      ↓
Identity Node
      ↓
Identity UDP Listener (AX630C)
      ↓
Semantic Dispatcher
      ├── StackChan Notifier
      ├── RGB Strip Notifier
      └── Ambient Runtime Notifier
                    ↓ UDP
             Ambient Runtime (Tab5)
```

---

## Engineering Impact

### Tab5

- Static IPv4.
- Deterministic deployment.
- Stable network identity.

### AX630C

- New delivery adapter.
- Ambient Runtime becomes a first-class semantic destination.
- Dispatcher architecture preserved.

---

## Files Affected

### Ambient Runtime

```text
ambient_network.cpp
```

### Cognitive Runtime

```text
ambient_runtime_notifier.py
identity_udp_listener.py
```

---

## Repository Note

This document is temporary and should later be merged into the official documentation for the Ambient Runtime node after the complete integration milestone is closed.

