# Identity Profile Images V1

## Ambient Physical AI

### Engineering Discovery

---

# Purpose

This document records the engineering decisions adopted during the implementation of profile photographs in the Identity Node V1.

Its purpose is to explain:

- why the current implementation was adopted;
- how it works;
- its limitations;
- and the planned migration path to the definitive architecture.

This document is intentionally separated from the main architecture documentation because it describes an engineering milestone rather than a permanent architectural definition.

---

# Background

One of the objectives defined during the project evolution was to make the Identity Node capable of presenting a visual representation of the identified person.

Originally, the display presented only textual information:

```text
Name
Role
Context
```

Although functionally correct, this solution reduced the visual impact of the demonstration and did not fully represent the concept of physical identity proposed by Ambient Physical AI.

The project therefore evolved to include profile photographs associated with each identity.

---

# Engineering Challenge

At the current project stage, the Cognitive Runtime (AX630C) is already capable of processing identity information.

However, there is not yet a complete infrastructure responsible for:

- centralized profile storage;
- image synchronization;
- local caching;
- profile updates;
- remote profile management.

Waiting for that infrastructure would delay validation of the Identity Layer.

Therefore, an intermediate solution was adopted.

---

# Adopted Solution

A dedicated component named:

```text
ProfileImageManager
```

was introduced.

This component became the single abstraction responsible for providing profile images to the graphical interface.

The UI never accesses image data directly.

Instead, it simply requests:

```cpp
ProfileImageManager::drawProfile(profile.id, x, y);
```

This abstraction completely isolates the display layer from the image storage mechanism.

---

# Current Image Storage

For Version 1, images are embedded directly into firmware.

Each profile photograph is converted to:

```text
96 x 96 pixels
RGB565
uint16_t array
```

and stored as:

```text
main/profile_images/

claudio.h
herminio.h
mariana.h
student.h
unknown.h
```

Advantages:

- extremely fast rendering;
- zero filesystem dependency;
- deterministic execution;
- no network dependency;
- ideal for laboratory validation.

---

# Technical Challenges Found

During implementation several engineering challenges were identified.

## Application Size

Embedding five RGB565 photographs increased the firmware size beyond the default 1 MB application partition.

The project was migrated from:

```text
Single Factory App
```

to

```text
Single Factory App (Large)
```

allowing the firmware to grow without requiring architectural modifications.

---

## RGB565 Rendering

Initial rendering presented corrupted colors due to RGB565 interpretation.

After validation, the rendering pipeline was corrected and stable profile visualization was achieved.

---

## Avatar Quality

The rendering pipeline proved correct.

However, portrait quality depends strongly on the original photograph.

Images containing:

- large backgrounds;
- distant faces;
- excessive visual detail;

produce poor results after reduction to 96 × 96 pixels.

This is not a software limitation but a consequence of image composition.

Future versions should use portraits specifically prepared for avatar rendering.

---

# Current Status

Validated features:

- ProfileImageManager abstraction
- Embedded profile images
- Five independent user profiles
- Five NFC cards mapped
- Automatic avatar selection
- Stable rendering
- Unknown profile fallback

The complete identity visualization flow is now operational.

---

# Why This Is Considered Temporary

Although fully functional, embedded profile images are **not** the intended final architecture.

They were intentionally adopted to validate:

- identity flow;
- user interface;
- profile abstraction;
- rendering pipeline;
- engineering integration.

before introducing the complete profile infrastructure.

---

# Planned Evolution

The future architecture will preserve the same application interface.

Only the image source will change.

Current implementation:

```text
NFC Card
        │
        ▼
UID Mapping
        │
        ▼
Profile Manager
        │
        ▼
ProfileImageManager
        │
        ▼
Embedded RGB565 Images
```

Future implementation:

```text
NFC Card
        │
        ▼
NDEF / UID
        │
        ▼
Identity Service
        │
        ▼
Cognitive Runtime (AX630C)
        │
        ▼
Profile Synchronization
        │
        ▼
Local Cache
        │
        ▼
ProfileImageManager
```

Notice that the graphical interface remains unchanged.

Only the image provider changes.

This validates the architectural decision of introducing the ProfileImageManager abstraction from the beginning.

---

# Conclusion

The implementation successfully achieved the objectives established for Identity Node V1.

The project now supports:

- visual identity;
- profile abstraction;
- avatar rendering;
- profile selection;
- future migration to synchronized profile storage.

The current implementation should therefore be considered an engineering milestone that validates the architectural direction while keeping the project simple, deterministic and fully reproducible during the current development stage.