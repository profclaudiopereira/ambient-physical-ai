# AX630C Deployment Tools

## Ambient Physical AI

This directory contains helper scripts used during development to deploy
the Cognitive Runtime from the development workstation to the AX630C
target device.

These tools are intended only for development and validation.

------------------------------------------------------------------------

# Current Tool

## deploy_runtime_ax630c_full_tts_UPDATED_WITH_CONSOLE_NOTIFIER.bat

Deploys the complete Cognitive Runtime tree from the development
workstation to the AX630C runtime environment.

Current deployment target:

``` text
runtime/cognitive/stackflow/
```

Instead of copying individual Python modules, the script synchronizes
the entire StackFlow runtime directory, including all runtime modules,
services, supporting scripts, laboratories and documentation required by
the current development environment.

The deployment currently includes, among others:

-   Core StackFlow runtime modules
-   Identity processing modules
-   Semantic Dispatcher components
-   Ambient Context services
-   Echo Pyramid voice integration modules
-   Runtime notifier modules
-   Cognitive Runtime Console notifier
-   Runtime Console UART test utility
-   Supporting documentation and laboratory files contained in the
    StackFlow runtime tree

After deployment, the script:

-   creates the destination directory if necessary;
-   removes Python cache artifacts (`__pycache__` and `*.pyc`);
-   validates the deployed Python sources using `python3 -m py_compile`.

------------------------------------------------------------------------

# Requirements

-   Windows
-   OpenSSH Client (`scp`)
-   Network connectivity to the AX630C
-   SSH enabled on the AX630C

------------------------------------------------------------------------

# Usage

From the repository root:

``` bat
tools\deploy_runtime_ax630c_full_tts_UPDATED_WITH_CONSOLE_NOTIFIER.bat
```

The script copies the runtime files to:

``` text
/root/ambient-runtime/runtime/cognitive/stackflow/
```

The runtime can then be validated directly on the AX630C.

------------------------------------------------------------------------

# Notes

This script does not modify the Git repository.

It is intended only to synchronize the complete validated Cognitive
Runtime workspace between the development workstation and the AX630C
during development.
