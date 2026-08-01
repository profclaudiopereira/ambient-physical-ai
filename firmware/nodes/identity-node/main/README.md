# Identity Node V1

## Ambient Physical AI

### Identity Layer Runtime Node

The Identity Node is the Identity Layer implementation of the Ambient Physical AI ecosystem.

Its responsibility is to transform a physical identity presented by a user into a standardized semantic identity that can be consumed by the distributed Cognitive Runtime.

Within the current architecture, the Identity Node represents the boundary between the physical world and the cognitive software infrastructure.

Rather than simply reading NFC cards, the node combines physical authentication, local user interaction, operational context selection and standardized identity generation into a single runtime component.

The resulting Identity Package becomes the authoritative source describing **who** is interacting with the environment and **under which operational context** the interaction is taking place.

This information is then transmitted to the Cognitive Runtime, where it becomes part of the semantic processing pipeline responsible for context construction and ambient decision making.

---

# Mission

The Identity Node performs five primary responsibilities.

```text
NFC Detection
        │
        ▼
Identity Resolution
        │
        ▼
Context Selection
        │
        ▼
Identity Visualization
        │
        ▼
Identity Package Generation
```

More specifically, the node is responsible for:

- detecting NFC cards using the WS1850S reader;
- resolving authenticated users through local UID mapping;
- allowing local operational context selection;
- presenting identity information through the M5Dial interface;
- generating standardized Identity Packages for the remaining Ambient Physical AI ecosystem.

The Identity Node intentionally concentrates only the responsibilities associated with identity acquisition.

Higher-level reasoning, context interpretation and ambient adaptation are performed by the Cognitive Runtime and therefore remain outside the scope of this firmware.

---

# Current Project Status

```text
Identity Node V1

VALIDATED
```

Current validated milestone:

```text
IDENTITY_NODE_V1_MILESTONE_002

Identity visualization with profile images validated.
```

The current implementation validates the complete identity acquisition pipeline, including:

- NFC communication;
- user identification;
- profile visualization;
- operational context selection;
- Identity Package generation;
- UDP communication with the Cognitive Runtime;
- integration with the Presence Layer.

No experimental functionality is required to execute the validated runtime.

---

# Identity Layer Overview

Within Ambient Physical AI, identity acquisition is intentionally separated from environmental perception and cognitive reasoning.

```text
Presence Layer
        │
presence_event
        │
        ▼
Identity Layer
        │
Identity Package
        │
        ▼
Cognitive Runtime
        │
Semantic Context
        │
        ▼
Ambient Intelligence
```

This separation allows each layer to evolve independently while preserving stable communication contracts between distributed components.

The Identity Node therefore acts as a specialized runtime responsible only for identity acquisition and presentation.

---

# Hardware Platform

The current validated hardware platform is based on the M5Dial V1.1.

```text
M5Dial V1.1

ESP32-S3
        │
Round IPS Display
        │
Rotary Encoder
        │
Capacitive Touch
        │
Speaker
        │
WS1850S NFC Reader
```

The integrated hardware allows the Identity Node to provide a complete local interaction experience without requiring external displays or input devices.

Current hardware capabilities include:

- circular graphical interface;
- touch interaction;
- rotary encoder navigation;
- audible feedback through the internal speaker;
- NFC card acquisition through the reusable WS1850S component.

The NFC reader itself is documented separately in:

```text
components/ws1850s/README.md
```

This document focuses exclusively on the Identity Layer runtime and therefore avoids duplicating the reusable component documentation.

---

# Runtime Architecture

The Identity Node occupies the boundary between physical interaction and semantic processing.

```text
                 Presence Node
                       │
             UDP presence_event
                       │
                       ▼
              Identity Node
                       │
          ┌────────────┴────────────┐
          │                         │
 Identity Visualization     Identity Package
          │                         │
          └────────────┬────────────┘
                       │
                 UDP JSON
                       │
                       ▼
           Cognitive Runtime
                (AX630C)
```

The runtime performs all local interaction before transmitting a standardized Identity Package to the Cognitive Runtime.

No semantic reasoning is performed locally.

The Identity Node remains intentionally focused on identity acquisition, local presentation and contract generation.

---

# Identity Acquisition Flow

The complete validated runtime follows the sequence below.

```text
Presence detected
        │
        ▼
Tap NFC Card
        │
        ▼
Read UID
        │
        ▼
Resolve Profile
        │
        ▼
Associate Context
        │
        ▼
Display Identity
        │
        ▼
Generate Identity Package
        │
        ▼
Cognitive Runtime
```

Each stage is intentionally isolated from the others, improving readability, maintainability and future evolution of the firmware while preserving the current communication contracts.

---

# Software Architecture

The Identity Node is implemented as a distributed FreeRTOS application where independent tasks cooperate through well-defined communication mechanisms instead of directly manipulating shared application state.

This organization improves maintainability, isolates hardware-specific operations and keeps the graphical interface responsive while NFC operations and network communication execute asynchronously.

The current implementation intentionally separates:

- user interface;
- NFC acquisition;
- network communication;
- application events.

This architecture has been validated as the baseline for Identity Node V1.

---

# FreeRTOS Runtime

The firmware is organized around three independent execution contexts.

```text
                FreeRTOS

        ┌──────────────────────┐
        │      UI Task         │
        └──────────┬───────────┘
                   │
      Identity Event Queue
                   │
      ┌────────────┴────────────┐
      │                         │
┌───────────────┐      ┌────────────────┐
│   NFC Task    │      │ UDP Listener   │
└───────────────┘      └────────────────┘
```

Each task owns a specific responsibility and communicates through application-level events.

No task modifies another task's internal state directly.

This ownership model considerably simplifies the runtime while reducing coupling between graphical, networking and hardware-specific operations.

---

# UI Task

The UI Task owns every aspect of the graphical user interface.

Responsibilities include:

- display rendering;
- touch processing;
- rotary encoder handling;
- speaker feedback;
- profile visualization;
- context visualization;
- Identity Package generation;
- application state management.

The UI Task is the only runtime component allowed to modify the display.

This design prevents concurrent rendering operations and keeps the graphical interface deterministic.

The task also owns the application-level identity state, including:

- selected profile;
- selected operational context;
- current visualization state;
- temporary presence prompt;
- identity confirmation screen.

Incoming events generated by other runtime tasks are translated into user interface updates exclusively by the UI Task.

---

# NFC Task

The NFC Task owns the complete lifecycle of the WS1850S reader.

Responsibilities include:

- controller initialization;
- NFC polling;
- card detection;
- UID acquisition;
- UID mapping;
- controller recovery;
- staged error handling.

Rather than interacting directly with the display, the NFC Task publishes application events through the Identity Event Queue.

This separation allows NFC timing requirements to remain independent from the graphical interface refresh cycle.

The current implementation also includes a staged recovery mechanism capable of rebuilding the NFC runtime after repeated communication failures.

The detailed implementation of the reusable NFC component is documented separately in:

```text
components/ws1850s/README.md
```

---

# UDP Listener Task

The UDP Listener Task integrates the Identity Layer with the Presence Layer.

Its responsibility is intentionally limited.

The task:

- waits for Wi-Fi connectivity;
- listens for UDP packets;
- validates the incoming Presence Layer contract;
- converts accepted packets into application events.

When a valid `presence_event` is received, the task notifies the UI through the Identity Event Queue.

The graphical interface then displays the authentication prompt without requiring any direct interaction between networking code and display rendering.

---

# Identity Event Queue

Communication between runtime tasks is performed through a FreeRTOS queue.

```text
NFC Task
        │
        │
UDP Listener
        │
        ▼
Identity Event Queue
        │
        ▼
UI Task
```

Current event categories include:

- NFC card detected;
- NFC card removed;
- UID successfully acquired;
- NFC runtime errors;
- NFC recovery completed;
- Presence Layer notifications.

This event-driven architecture keeps hardware-specific operations isolated from user interface logic while preserving deterministic application behavior.

---

# Shared I²C Bus Protection

The M5Dial internal peripherals and the WS1850S reader share the same I²C bus.

To prevent concurrent access, the firmware implements a dedicated mutex protecting short hardware transactions.

```text
UI Task
        │
        │
        ▼
   I²C Mutex
        ▲
        │
NFC Task
```

The mutex intentionally protects only low-level bus operations.

Higher-level graphical rendering is performed outside the critical section, minimizing contention and improving overall responsiveness.

Additionally, temporary NFC quiet windows are introduced while the user interacts with the touch interface or rotary encoder.

This strategy reduces unnecessary contention between the graphical interface and NFC polling without affecting the validated runtime behavior.

---

# Runtime State Ownership

The current architecture clearly defines ownership of runtime data.

The UI Task owns:

- active profile;
- selected context;
- current display state;
- temporary visualization screens.

The NFC Task owns:

- NFC controller lifecycle;
- polling state machine;
- UID acquisition;
- recovery procedures.

The UDP Listener owns:

- network reception;
- Presence Layer validation;
- event publication.

This ownership model eliminates the need for complex synchronization mechanisms and significantly simplifies future maintenance.

---

# Operational Contexts

The Identity Node allows local selection of the operational context through the M5Dial rotary encoder.

Current validated contexts are:

- Lab
- Research
- Classroom
- Demo
- Meeting

The selected context becomes part of every generated Identity Package and represents the user's intended operating environment rather than the physical location.

Context selection remains available independently of NFC authentication and is immediately reflected by the graphical interface.

---

# User Profiles

The current implementation provides five validated identity profiles.

```text
Unknown
Claudio
Student
Mariana
Hermínio
```

Each profile includes:

- profile identifier;
- display name;
- role;
- profile image.

Profile resolution currently uses local NFC UID mapping implemented by the Identity Node runtime.

The abstraction responsible for graphical profile rendering is documented separately in:

```text
main/README.md
```

This separation avoids duplication between the runtime documentation and the documentation dedicated to profile image management.

---

# User Interface

The Identity Node provides a self-contained user interface implemented on the M5Dial platform.

The interface was designed to guide the user through the complete identity acquisition process while presenting the current runtime state in a simple and deterministic manner.

Rather than exposing implementation details, the graphical interface focuses on the operational workflow experienced by the user.

Current interface responsibilities include:

- presenting the Identity Console;
- requesting user authentication after presence detection;
- displaying authenticated profile information;
- indicating the active operational context;
- providing immediate visual feedback during the authentication process.

The user interface is entirely managed by the UI Task, ensuring that all display operations remain centralized within a single runtime component.

---

# User Interaction Flow

The validated user experience follows the sequence below.

```text
Identity Console
        │
        ▼
Presence detected
        │
        ▼
Tap NFC Card
        │
        ▼
Card authentication
        │
        ▼
Identity visualization
        │
        ▼
Return to console
```

This sequence intentionally minimizes user interaction while providing clear visual feedback throughout the authentication process.

---

# Identity Console

The Identity Console represents the default operating screen of the Identity Node.

It remains active whenever no temporary interaction screen is being displayed.

The console presents the current operational state of the node, including:

- active user profile;
- profile avatar;
- user role;
- selected operational context;
- runtime readiness.

When no authenticated identity is available, the interface displays the default placeholder profile and indicates that the system is waiting for identification.

The console is automatically refreshed whenever:

- the active context changes;
- the selected profile changes;
- NFC availability changes;
- temporary interaction screens expire.

---

# Presence Prompt

The Identity Node integrates with the Presence Layer through UDP presence notifications.

When a validated presence event is received, the graphical interface temporarily replaces the default console with a dedicated authentication prompt.

```text
Presence detected

Tap NFC Card
```

The purpose of this screen is to guide the user naturally toward the authentication step without exposing networking or runtime details.

The prompt remains visible only for a limited period before the interface automatically returns to the default console if no authentication occurs.

---

# Identity Visualization

After successful authentication, the Identity Node presents a temporary identity confirmation screen.

The visualization contains:

- authenticated profile image;
- user name;
- user role;
- selected operational context;
- NFC UID.

This screen provides immediate confirmation that the physical identity has been successfully resolved before the generated Identity Package is transmitted to the Cognitive Runtime.

The visualization is intentionally temporary.

After a predefined timeout, the interface automatically returns to the Identity Console, allowing the node to continue operating without user intervention.

---

# Profile Images

Identity visualization is based on embedded profile images stored locally within the firmware.

Current validated profiles include:

- Claudio
- Hermínio
- Mariana
- Student
- Unknown

Each authenticated profile is associated with a dedicated avatar rendered directly on the M5Dial display.

Image management is intentionally separated from the remaining application logic through the ProfileImageManager abstraction.

Complete implementation details are documented separately in:

```text
main/README.md
```

This README focuses on runtime behavior and intentionally avoids duplicating documentation specific to graphical asset management.

---

# ProfileImageManager

The Identity Node never accesses image resources directly.

Instead, profile rendering is centralized through the following public interface:

```cpp
ProfileImageManager::drawProfile(
    profile.id,
    center_x,
    center_y
);
```

This abstraction isolates image storage from application logic and provides a stable interface for the graphical subsystem.

Current implementation characteristics include:

- embedded RGB565 images;
- automatic profile resolution;
- centralized rendering;
- fallback to the default placeholder profile.

Because the graphical interface depends only on this abstraction, future changes to image storage can be implemented without modifying the UI runtime.

---

# Operational Context Selection

The M5Dial rotary encoder allows the operator to select the current operational context.

Validated contexts include:

- Lab
- Research
- Classroom
- Demo
- Meeting

Whenever the encoder position changes, the Identity Console immediately reflects the newly selected context.

The selected context also becomes part of every generated Identity Package.

This design allows identity information and operational intent to be transmitted together to the Cognitive Runtime.

---

# Touch Interaction

The capacitive touch interface provides simple local interaction for runtime validation and manual profile navigation.

Touch events are processed exclusively by the UI Task.

To minimize contention with the NFC reader, temporary NFC quiet windows are introduced while the user interacts with the display.

This strategy improves responsiveness without affecting the validated authentication flow.

---

# Audible Feedback

The Identity Node complements graphical interaction with short audible notifications generated by the M5Dial speaker.

Audio feedback is used to indicate events such as:

- interface initialization;
- profile changes;
- successful authentication;
- interaction requests.

These sounds are intentionally brief and serve only as interaction feedback.

No voice synthesis or speech recognition is performed by the Identity Node.

---

# Display Ownership

The graphical interface follows a strict ownership model.

Only the UI Task is allowed to perform display rendering.

This eliminates concurrent display updates and keeps the runtime deterministic.

Temporary screens follow the priority below.

```text
Presence Prompt
        │
        ▼
Identity Confirmation
        │
        ▼
Identity Console
```

This priority order guarantees that the most relevant information is always presented to the user while preserving a consistent interaction model throughout the authentication process.

---

# NFC Integration

The Identity Node acquires physical identities through the reusable WS1850S NFC reader component.

Within the Identity Layer architecture, NFC communication is intentionally isolated from the remaining application logic.

The reusable component is responsible for low-level communication with the controller, while the Identity Node is responsible for transforming the acquired UID into an application-level identity.

This separation preserves a clear distinction between hardware access and identity processing.

Complete documentation of the reusable NFC component is available in:

```text
components/ws1850s/README.md
```

The present document describes only how the Identity Layer uses that component.

---

# NFC Runtime

The NFC runtime executes independently from the graphical interface.

Its responsibilities include:

- controller initialization;
- card polling;
- card presence detection;
- UID acquisition;
- communication error detection;
- staged recovery;
- event publication.

The runtime never modifies the user interface directly.

Instead, it publishes application events that are later processed by the UI Task.

This architecture keeps NFC timing independent from graphical rendering while improving system responsiveness.

---

# Card Detection

The Identity Node continuously monitors the NFC reader waiting for nearby cards.

The polling cycle follows the sequence below.

```text
Initialize Reader
        │
        ▼
Poll Card
        │
        ▼
Card Present?
        │
 ┌──────┴──────┐
 │             │
 No           Yes
 │             │
 ▼             ▼
Continue     Read UID
Polling
```

The polling frequency is intentionally controlled to reduce unnecessary bus activity while maintaining a responsive user experience.

---

# UID Acquisition

When a card is detected, the runtime performs UID acquisition using the WS1850S driver.

The acquisition process includes:

- card presence confirmation;
- anti-collision procedure;
- UID retrieval;
- bounded retry attempts;
- error classification.

Expected temporary conditions, such as card removal during acquisition, are treated as normal operational events rather than controller failures.

Persistent communication failures activate the staged recovery mechanism.

---

# Local Identity Resolution

The current validated implementation resolves identities through local NFC UID mapping.

The process follows the sequence below.

```text
UID
 │
 ▼
Local Mapping
 │
 ▼
Profile
 │
 ▼
Identity Package
```

Known UIDs are associated with predefined user profiles stored locally by the Identity Node.

Unknown cards automatically fall back to the default visitor profile.

This behavior guarantees deterministic execution without requiring external services.

Current implementation intentionally performs no remote identity lookup.

---

# Identity Profiles

Each resolved profile provides the information required by both the local interface and the generated Identity Package.

Current profile information includes:

- profile identifier;
- display name;
- role;
- profile image.

The graphical representation of each profile is handled by the ProfileImageManager module documented separately.

---

# Presence Layer Integration

The Identity Node integrates directly with the Presence Layer.

Presence notifications are received through UDP before identity acquisition begins.

The validated interaction sequence is:

```text
Presence Node
        │
presence_event
        │
        ▼
Identity Node
        │
Display Prompt
        │
        ▼
Tap NFC Card
```

The Identity Node validates incoming presence notifications before initiating the authentication workflow.

Only validated events are converted into application-level runtime events.

This prevents unrelated network traffic from affecting the user interface.

---

# Identity Package Generation

After successful authentication, the Identity Node generates a standardized Identity Package.

The package contains the information required by the Cognitive Runtime to identify the authenticated user and understand the current operational context.

The generated package includes:

- contract version;
- profile identifier;
- user name;
- role;
- operational context;
- NFC status;
- card presence state;
- UID;
- source identifier.

The package represents the official identity contract produced by the Identity Layer.

---

# Communication with the Cognitive Runtime

Identity Packages are transmitted directly to the Cognitive Runtime using UDP.

```text
Identity Node
        │
Identity Package
        │
UDP
        │
        ▼
Cognitive Runtime
```

The transport layer intentionally remains lightweight.

Identity acquisition is event-driven, making persistent communication channels unnecessary for the current validated implementation.

The Cognitive Runtime receives each Identity Package independently and continues semantic processing without requiring additional interaction from the Identity Node.

---

# Runtime State Machine

From the perspective of the Identity Layer, NFC operation follows a controlled state machine.

```text
Initialize
      │
      ▼
Idle
      │
      ▼
Polling
      │
      ▼
Card Detected
      │
      ▼
Read UID
      │
      ▼
Generate Identity
      │
      ▼
Waiting Removal
      │
      ▼
Idle
```

Communication failures activate additional recovery states without affecting the remaining runtime components.

This isolation improves robustness while keeping the user interface responsive.

---

# Current Implementation Boundaries

The validated Identity Node intentionally limits its responsibilities to local identity acquisition.

Current implementation:

- uses local UID mapping;
- generates Identity Package V1.1;
- communicates directly with the Cognitive Runtime through UDP;
- provides local identity visualization.

The current implementation does **not** include:

- centralized profile synchronization;
- NDEF profile decoding;
- cloud-based identity resolution;
- remote profile management.

These capabilities belong to future project evolution and are not part of the validated Identity Node V1 baseline.

---

# Repository Organization

The Identity Node follows the repository organization adopted throughout the Ambient Physical AI project.

The objective of this structure is to separate reusable components, application-specific runtime code and supporting documentation, making the project easier to understand, maintain and evolve.

Current directory organization:

```text
identity-node/

README.md

components/
    ws1850s/
        README.md

main/
    README.md

    main.cpp
    profile_image_manager.cpp
    profile_image_manager.h

    profile_images/
        claudio.h
        herminio.h
        mariana.h
        student.h
        unknown.h
```

Each directory has a well-defined responsibility.

---

# Documentation Hierarchy

Documentation is intentionally distributed across multiple README files.

Instead of concentrating every explanation into a single document, each directory documents only the functionality that belongs to its own scope.

```text
README.md
        │
        ├──────────────► Identity Layer overview
        │
        ├──────────────► Runtime architecture
        │
        ├──────────────► Identity workflow
        │
        ▼
components/ws1850s/README.md
        │
        └──────────────► Reusable NFC reader component

main/README.md
        │
        └──────────────► Profile image subsystem
```

This organization avoids duplicated documentation while improving long-term maintainability.

Whenever possible, subordinate documentation is referenced instead of being repeated.

---

# Main Application

The `main` directory contains the Identity Node runtime implementation.

Its responsibilities include:

- application startup;
- runtime initialization;
- graphical interface;
- FreeRTOS task creation;
- Identity Package generation;
- integration with reusable components.

Hardware-independent functionality is delegated to reusable modules whenever appropriate.

---

# Reusable Components

The Identity Node uses reusable software components whenever possible.

The primary reusable module in the current implementation is:

```text
components/
    ws1850s/
```

This component encapsulates the complete communication layer for the WS1850S NFC controller and is intentionally documented independently because it may be reused by future firmware nodes.

The Identity Node interacts with this component through its public interface without duplicating low-level implementation details.

---

# Profile Image Resources

Graphical resources are intentionally isolated from the remaining application.

```text
main/

profile_image_manager.cpp
profile_image_manager.h

profile_images/
```

This organization keeps embedded image assets separate from runtime logic while allowing the user interface to remain independent from image storage details.

The complete profile image subsystem is documented separately in:

```text
main/README.md
```

---

# Build

The Identity Node is developed using ESP-IDF.

Typical build command:

```bash
idf.py build
```

The current CMake configuration includes the main runtime together with the profile image manager.

```cmake
SRCS
    "main.cpp"
    "profile_image_manager.cpp"
```

The reusable WS1850S component is linked through the component dependency mechanism provided by ESP-IDF.

---

# Flash

After a successful build, the firmware can be programmed using the standard ESP-IDF workflow.

```bash
idf.py flash monitor
```

The serial monitor is particularly useful during runtime validation because it provides:

- initialization logs;
- NFC diagnostics;
- Presence Layer notifications;
- Identity Package generation;
- communication status.

These logs were extensively used during validation of Identity Node V1.

---

# Configuration

The current firmware is intentionally self-contained.

Runtime configuration includes:

- Wi-Fi credentials;
- UDP communication parameters;
- AX630C destination;
- local profile mapping;
- operational contexts.

These parameters are currently compiled into the firmware as part of the validated V1 implementation.

---

# Integration Boundaries

The Identity Node intentionally exposes a limited public interface to the remaining Ambient Physical AI ecosystem.

Incoming information:

```text
Presence Layer
        │
presence_event
```

Outgoing information:

```text
Identity Package
```

Everything between these two communication contracts is considered an internal implementation detail of the Identity Layer.

This separation preserves architectural independence between distributed runtime components.

---

# Engineering Philosophy

The current implementation follows a small number of engineering principles adopted throughout Ambient Physical AI.

These include:

- clear separation of responsibilities;
- reusable software components;
- event-driven communication;
- deterministic runtime behavior;
- modular documentation;
- maintainable source organization.

The objective is to keep each firmware node focused on its own responsibility while exposing stable communication contracts to the distributed Cognitive Runtime.

---

# Navigation

The following documents complement this README.

| Document | Purpose |
|----------|---------|
| `README.md` | Identity Layer overview |
| `components/ws1850s/README.md` | WS1850S reusable NFC component |
| `main/README.md` | Profile images and ProfileImageManager |

Together, these documents describe the complete validated implementation of the Identity Layer without unnecessary duplication of technical content.

---

# Validation Status

The Identity Node V1 has completed the validation activities planned for the first implementation milestone of the Identity Layer.

Validation focused on the complete end-to-end identity acquisition pipeline rather than on isolated software modules.

The objective was to demonstrate that the node could reliably acquire a physical identity, present it locally and deliver a standardized Identity Package to the Cognitive Runtime.

---

# Validated Features

The following capabilities have been validated in the current implementation.

## Runtime

- FreeRTOS multi-task architecture
- Independent UI Task
- Independent NFC Task
- Independent UDP Listener Task
- Identity Event Queue
- Shared I²C protection
- Event-driven runtime

## User Interface

- Identity Console
- Presence Prompt
- Identity Visualization
- Profile rendering
- Context selection
- Touch interaction
- Rotary encoder navigation
- Audible feedback

## NFC

- WS1850S initialization
- Card polling
- Card detection
- UID acquisition
- UID mapping
- Card removal detection
- Controlled recovery
- Runtime stabilization

## Networking

- Wi-Fi Station mode
- UDP listener
- Presence event reception
- Identity Package transmission
- AX630C integration

## Identity

- Local profile resolution
- Five validated profiles
- Operational context selection
- Identity Package generation
- Identity Package V1.1
- Profile visualization

---

# Validation Summary

The validated runtime currently supports the following operational sequence.

```text
Presence detected
        │
        ▼
Authentication request
        │
        ▼
NFC card acquisition
        │
        ▼
UID resolution
        │
        ▼
Profile mapping
        │
        ▼
Identity visualization
        │
        ▼
Identity Package generation
        │
        ▼
UDP transmission
        │
        ▼
Cognitive Runtime
```

All stages above have been validated within the current project baseline.

---

# Current Engineering Status

```text
Identity Layer

VALIDATED
```

Current implementation status:

| Component | Status |
|-----------|--------|
| Runtime Architecture | VALIDATED |
| User Interface | VALIDATED |
| WS1850S Integration | VALIDATED |
| UID Mapping | VALIDATED |
| Profile Images | VALIDATED |
| Context Selection | VALIDATED |
| Presence Integration | VALIDATED |
| Identity Package | VALIDATED |
| UDP Communication | VALIDATED |
| AX630C Interface | VALIDATED |

The Identity Layer is considered operational and suitable for integration with the remaining Ambient Physical AI ecosystem.

---

# Current Implementation Boundaries

The current firmware intentionally focuses on the validated Identity Layer responsibilities.

Current implementation includes:

- local identity acquisition;
- local UID mapping;
- embedded profile images;
- local graphical interface;
- Identity Package generation;
- UDP communication with the Cognitive Runtime.

The following capabilities are intentionally outside the scope of Identity Node V1:

- semantic reasoning;
- profile synchronization;
- distributed profile storage;
- environmental decision making;
- cloud services.

These responsibilities belong to higher architectural layers.

---

# Future Work

The current implementation establishes a stable foundation for future evolution while intentionally preserving the existing public interfaces.

Planned future work includes:

## Identity Resolution

- NDEF-based identity resolution;
- dual UID/NDEF authentication strategy;
- extended identity metadata.

## Profile Management

- centralized profile synchronization;
- runtime-managed profile assets;
- dynamic profile updates.

## User Experience

- richer graphical interface;
- additional profile information;
- expanded visualization capabilities.

These items represent planned evolution only.

They are **not** part of the validated implementation described by this document.

---

# Identity Package V1.1

The current Identity Layer produces **Identity Package Specification V1.1**.

This version extends the original contract while preserving backward compatibility.

Current additions include:

- `contract_version`
- `current_context`

The legacy `context` field remains available to maintain compatibility with previous consumers.

The validated processing pipeline is:

```text
Identity Node
        │
Identity Package V1.1
        │
        ▼
Identity UDP Listener
        │
        ▼
Context Builder
        │
        ▼
Semantic Context
        │
        ▼
Cognitive Runtime
```

The complete communication chain has been validated using the current project baseline.

---

# Related Documentation

The Identity Layer documentation is organized hierarchically.

```text
identity-node/

README.md

components/
    ws1850s/
        README.md

main/
    README.md
```

Each document addresses a specific responsibility.

- **README.md** provides the architectural and operational overview of the Identity Layer.
- **components/ws1850s/README.md** documents the reusable NFC reader component.
- **main/README.md** documents the profile image subsystem and the `ProfileImageManager`.

This organization minimizes duplicated information while improving documentation maintainability.

---

# Conclusion

Identity Node V1 establishes the validated implementation of the Identity Layer for the Ambient Physical AI ecosystem.

The current firmware successfully integrates user interaction, NFC-based identity acquisition, operational context selection and standardized Identity Package generation into a single runtime component.

The implementation intentionally emphasizes modularity, deterministic runtime behavior and clear separation of responsibilities.

At the conclusion of the current milestone, the Identity Layer provides a stable interface between the physical environment and the Cognitive Runtime, serving as the authoritative source of authenticated user identity for the Ambient Physical AI distributed architecture.

