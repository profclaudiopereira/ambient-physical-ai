# MASTER_REPORT_ATOM_MATRIX_RGB_NODE.md

# Ambient Physical AI

## Engineering Mission Report

### Atom Matrix RGB Node --- Semantic Integration and Platform Calibration

## Mission Summary

This mission completed the implementation and validation of the **Atom
Matrix RGB Node** as a fully integrated Expression Layer component,
aligned with the architecture already established for the RGB Strip
Node.

The work focused on replicating the semantic behavior---not simply
copying code---while adapting the implementation to the optical
characteristics of the Atom Matrix hardware.

------------------------------------------------------------------------

# Objectives Achieved

-   Replicated the RGB Strip semantic architecture in the Atom Matrix
    node.
-   Extended the Expression Processor to support contextual information
    (`semantic_context_t`).
-   Updated the semantic consumer to extract `payload.user_id`.
-   Implemented profile-aware ambient lighting.
-   Preserved compatibility with the existing Cognitive Runtime.
-   Added platform-specific color calibration.
-   Centralized calibration constants.
-   Produced engineering documentation.

------------------------------------------------------------------------

# Files Modified

    firmware/nodes/expression-node/atom-matrix-rgb-node/main/

    atom_matrix_rgb_node.c
    expression_processor.c
    expression_processor.h
    rgb_effects.c
    rgb_effects.h
    semantic_consumer.c

Documentation added:

    firmware/nodes/expression-node/atom-matrix-rgb-node/notes/

    TECHNICAL_NOTE_ATOM_MATRIX_RGB_NODE_PROFILE_LIGHTING.md

------------------------------------------------------------------------

# Main Engineering Changes

## 1. Semantic Context

Introduced:

``` c
typedef struct
{
    const char *user_id;
} semantic_context_t;
```

allowing semantic events to carry authenticated profile information
without changing the event contract.

------------------------------------------------------------------------

## 2. Semantic Consumer

The consumer now:

-   parses UDP JSON;
-   validates semantic events;
-   reads `payload.user_id`;
-   creates `semantic_context_t`;
-   forwards context to the Expression Processor.

------------------------------------------------------------------------

## 3. Expression Processor

The processor now selects ambient lighting according to the
authenticated profile.

Mappings:

-   Claudio
-   Herminio
-   Mariana / student
-   Visitor / guest

Unknown users safely fall back to Visitor.

------------------------------------------------------------------------

## 4. RGB Effects

The effects layer was reorganized into:

-   Primitive effects
-   Semantic state effects
-   Profile-specific effects

This preserves a clean architectural separation.

------------------------------------------------------------------------

## 5. Platform Calibration

Testing showed that identical RGB values on the RGB Strip and Atom
Matrix do not produce identical perceived colors.

Instead of forcing identical RGB numbers, the project now adopts
**platform-specific calibration with semantic equivalence**.

Current calibration:

  Profile    RGB
  ---------- -----------
  Claudio    (0,2,10)
  Herminio   (10,8,0)
  Mariana    (12,11,9)
  Visitor    (0,10,12)

Calibration values were centralized at the beginning of `rgb_effects.c`.

------------------------------------------------------------------------

# Validation

Validated:

-   Build
-   Flash
-   Wi-Fi
-   Semantic reception
-   user_id extraction
-   Profile selection
-   LED output
-   Visual calibration

The remaining activity is optional visual fine tuning under the final
demonstration environment.

------------------------------------------------------------------------

# Architectural Decisions

During the mission the following decisions were reinforced:

-   Semantic meaning must remain independent of hardware.
-   Hardware-specific calibration is acceptable and desirable.
-   Expression Nodes remain consumers of semantic events only.
-   The Cognitive Runtime continues sending semantic intent rather than
    LED commands.

------------------------------------------------------------------------

# Deliverables

-   Updated firmware.
-   Platform calibration.
-   Technical note.
-   Cleaner RGB organization.
-   Ready for future rhythm/animation implementation.

------------------------------------------------------------------------

# Repository Status

At mission completion:

-   Atom Matrix work committed separately.
-   Identity Node changes intentionally left unstaged.
-   Working tree isolated for subsequent Identity mission.

------------------------------------------------------------------------

# Final Assessment

Mission objectives were achieved.

The Atom Matrix RGB Node now matches the architectural capabilities of
the RGB Strip Node while respecting the physical characteristics of its
own hardware platform.

Status:

    ATOM MATRIX RGB NODE

    Semantic integration ........ COMPLETE
    Profile-aware lighting ...... COMPLETE
    Platform calibration ........ COMPLETE
    Engineering documentation ... COMPLETE

    Ready for integration.
