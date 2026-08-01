# OLED Context Presenter

## Ambient Physical AI

### Semantic Presentation Layer for the Mini OLED

---

# Overview

The `oled_context_presenter` component is responsible for transforming the normalized semantic context produced by the Ambient Runtime into a compact visual representation suitable for the Mini OLED display.

It occupies the presentation layer of the Mini OLED pipeline.

The component does not acquire data, receive network messages or communicate directly with the Cognitive Runtime.

Instead, it receives a fully normalized context snapshot and generates the corresponding presentation using the low-level SH1107 display driver. 

---

# Purpose

The purpose of this component is to isolate presentation logic from communication, data acquisition and hardware control.

Current responsibilities include:

- presenting the latest semantic context;
- formatting information for the Mini OLED;
- selecting the information displayed according to the runtime state;
- avoiding unnecessary display refreshes;
- generating a deterministic presentation from the normalized runtime snapshot.

The component intentionally performs no business logic.

---

# Position within the Ambient Runtime

The OLED Context Presenter occupies the presentation layer of the Ambient Runtime.

```text
Cognitive Runtime
        │
        ▼
semantic_event_receiver
        │
        ▼
ambient_context_snapshot
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
Mini OLED
```

Each component owns a single engineering responsibility.

The presenter is responsible only for deciding **what** should appear on the display.

The SH1107 driver is responsible only for **how** that information reaches the display hardware.

---

# Responsibilities

The `oled_context_presenter` component converts a normalized runtime snapshot into a compact visual representation.

Its current responsibilities include:

## Runtime State Presentation

Present the current operational state of the Ambient Runtime.

Supported presentation states include:

- Waiting for Context;
- Stale Context;
- Global Context;
- Personalized Context.

Each state is rendered using a dedicated presentation layout.

---

## Global Context Presentation

Display global information available to every user.

Current presentation includes:

- location;
- weather summary;
- temperature;
- UV index;
- UV classification.

The presenter assumes that all values have already been validated and normalized.

---

## Personalized Context Presentation

Display information specific to the authenticated user.

Current presentation supports:

- personalized title;
- primary value;
- secondary value;
- authenticated profile identifier.

The presenter performs no profile selection.

It simply renders the information available in the normalized context snapshot.

---

## Text Formatting

Prepare textual information for display on the Mini OLED.

Current formatting responsibilities include:

- ASCII uppercase conversion;
- fixed-width formatting;
- line length limitation;
- consistent field formatting.

Formatting rules are intentionally centralized within this component.

---

## Runtime State Selection

Determine which presentation should be rendered according to the normalized runtime state.

Possible rendering paths include:

- waiting for context;
- stale context;
- global information only;
- authenticated personalized information.

No communication or business rules are evaluated during this process.

---

# Presentation Pipeline

The presentation workflow follows a strictly layered architecture.

```text
Semantic Event
        │
        ▼
semantic_event_receiver
        │
        ▼
ambient_context_snapshot
        │
        ▼
oled_context_presenter
        │
        ▼
oled_sh1107
        │
        ▼
Mini OLED
```

Each layer performs a single responsibility.

This architecture minimizes coupling between semantic communication, presentation logic and hardware drivers.

---

# Rendering States

The presenter renders different layouts according to the runtime snapshot.

## Waiting State

When no semantic context has been received, the display presents a waiting screen indicating that the Ambient Runtime is awaiting information from the Cognitive Runtime. :contentReference[oaicite:1]{index=1}

---

## Stale State

When the available context is marked as stale, the display informs that the previously received information is no longer considered current. :contentReference[oaicite:2]{index=2}

---

## Global Context

When global information is available, the presenter displays environmental information intended for every user.

Typical information includes:

- location;
- weather;
- temperature;
- UV conditions.

---

## Personalized Context

When the runtime contains authenticated personal information, the presenter extends the display with user-specific content while preserving the global context already shown.

This information is obtained exclusively from the normalized runtime snapshot.

---

# Public API

The component intentionally exposes only two public functions.

## Context Rendering

```c
esp_err_t oled_context_presenter_render(
    const ambient_context_snapshot_t *context
);
```

Renders the current normalized context on the Mini OLED.

The caller is responsible for:

- selecting the appropriate PaHub channel;
- providing a valid context snapshot.

The presenter assumes that hardware routing has already been configured before rendering begins. :contentReference[oaicite:3]{index=3}

---

## Render Invalidation

```c
void oled_context_presenter_invalidate(void);
```

Forces the next rendering operation even if the effective presentation has not changed.

This function is useful after display initialization or whenever the current display contents must be refreshed explicitly. :contentReference[oaicite:4]{index=4}

---

# Dependencies

The component depends on:

- `oled_sh1107`;
- `semantic_event_receiver`.

Its ESP-IDF component registration is:

```cmake
idf_component_register(
    SRCS "oled_context_presenter.c"
    INCLUDE_DIRS "include"
    REQUIRES
        oled_sh1107
        semantic_event_receiver
)
```

The component intentionally has no direct dependency on:

- network services;
- JSON processing;
- UDP communication;
- Cognitive Runtime protocols;
- PaHub routing.

These responsibilities remain outside the presentation layer. :contentReference[oaicite:5]{index=5}

---

# Rendering Optimization

The `oled_context_presenter` minimizes unnecessary OLED updates by rendering only when the effective presentation changes.

Before generating a new screen, the component computes a signature representing the complete normalized context.

If the newly computed signature matches the previously rendered one, the rendering operation is skipped.

```text
ambient_context_snapshot
        │
        ▼
Context Signature
        │
        ▼
Compare with Previous Signature
        │
   ┌────┴────┐
   │         │
Same      Different
   │         │
   ▼         ▼
Return   Render Display
```

This mechanism significantly reduces unnecessary I²C traffic and OLED refreshes while preserving deterministic behavior. :contentReference[oaicite:0]{index=0}

---

# Presentation Rules

The presenter follows a deterministic set of presentation rules.

## Snapshot-Based Rendering

Every rendering decision is derived exclusively from the supplied `ambient_context_snapshot_t`.

The presenter never:

- queries other runtime modules;
- waits for external events;
- stores semantic information;
- interprets communication protocols.

Each rendered screen is a direct representation of the current normalized context.

---

## Fixed Display Layout

The Mini OLED uses a predefined layout.

Typical organization:

```text
Location

Weather

Temperature

UV Index

----------

Personal Information

Profile
```

The layout remains constant regardless of the origin of the semantic information.

---

## Text Normalization

Before rendering, textual information is normalized.

Current normalization includes:

- ASCII uppercase conversion;
- fixed-width formatting;
- maximum line length enforcement.

These rules ensure consistent rendering regardless of the received semantic content. :contentReference[oaicite:1]{index=1}

---

## Stateless Presentation

The presenter does not maintain semantic history.

Its internal state is limited to rendering optimization.

Semantic interpretation, historical information and context management remain responsibilities of other runtime components.

---

# Engineering Principles

The implementation follows the engineering principles adopted throughout the Ambient Physical AI project.

## Separation of Responsibilities

The component performs only presentation.

It intentionally avoids implementing:

- semantic communication;
- JSON parsing;
- UDP reception;
- user authentication;
- profile selection;
- weather acquisition;
- display hardware control.

Those responsibilities belong to dedicated runtime components.

---

## Presentation Independence

The presenter depends only on:

```text
ambient_context_snapshot_t
```

As long as this data structure remains stable, the presenter is independent from:

- communication protocols;
- Cognitive Runtime implementation;
- transport mechanisms;
- external services.

This minimizes coupling across the Ambient Runtime.

---

## Hardware Independence

The presenter never manipulates display registers or I²C transactions directly.

All hardware interaction is delegated to the `oled_sh1107` driver.

This separation keeps presentation logic independent from the display controller implementation.

---

## Deterministic Rendering

For a given context snapshot, the generated display is always identical.

No hidden runtime state influences the presentation.

This deterministic behavior simplifies validation, debugging and future maintenance.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| Waiting screen | Validated |
| Stale context screen | Validated |
| Global context presentation | Validated |
| Personalized context presentation | Validated |
| Text normalization | Validated |
| Render optimization | Validated |
| Forced refresh support | Validated |
| Mini OLED integration | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Related Components

The OLED Context Presenter operates together with the following Ambient Runtime components.

| Component | Relationship |
|-----------|--------------|
| `semantic_event_receiver` | Supplies the normalized semantic context snapshot. |
| `oled_sh1107` | Performs the low-level rendering on the Mini OLED. |
| `pahub` | Selects the I²C channel used by the Mini OLED before rendering. |
| `ambient_network` | Provides the network infrastructure required for semantic updates, although it is not accessed directly by the presenter. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/components/oled_sh1107/README.md` | SH1107 display driver. |
| `firmware/nodes/ambient-runtime-node/components/semantic_event_receiver/README.md` | Semantic communication and normalized context generation. |
| `docs/notes/NOTE_002_SH1107_DISPLAY_GEOMETRY_VALIDATION.md` | Engineering investigation that validated the SH1107 initialization sequence. |

---

# Conclusion

The `oled_context_presenter` component implements the presentation layer of the Mini OLED within the Ambient Runtime.

By transforming a normalized semantic context into a deterministic visual representation while remaining independent from communication, hardware control and business logic, the component preserves the modular architecture of the Ambient Physical AI project.

Its rendering optimization strategy, strict separation of responsibilities and dependence on a single normalized context structure provide a maintainable and reusable foundation for contextual information presentation on resource-constrained embedded displays.