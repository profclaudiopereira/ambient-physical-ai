# AX630C Deployment Tools

## Ambient Physical AI

This directory contains helper scripts used during development to deploy the Cognitive Runtime from the development workstation to the AX630C target device.

These tools are intended only for development and validation.

---

# Current Tool

## deploy_runtime_ax630c.bat

Deploys the Cognitive Runtime Python modules from the development workstation to the AX630C runtime environment.

Current deployment target:

```text
runtime/cognitive/stackflow/
```

Files copied:

- context_builder.py
- context_registry.py
- identity_udp_listener.py
- semantic_services.py
- semantic_mcp_tools.py
- stackchan_notifier.py

---

# Requirements

- Windows
- OpenSSH Client (`scp`)
- Network connectivity to the AX630C
- SSH enabled on the AX630C

---

# Usage

From the repository root:

```bat
tools\deploy_runtime_ax630c.bat
```

The script copies the runtime files to:

```text
/root/ambient-runtime/runtime/cognitive/stackflow/
```

The runtime can then be validated directly on the AX630C.

---

# Notes

This script does not modify the Git repository.

It is intended only to synchronize the validated runtime between the development workstation and the AX630C during development.