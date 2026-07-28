# TECHNICAL NOTE

## Ambient Physical AI

### Echo Pyramid Personalized Welcome Integration

------------------------------------------------------------------------

# Purpose

This technical note documents the engineering work performed to extend
the Expression Layer with personalized welcome support.

The implementation preserves full backward compatibility while allowing
the Cognitive Runtime to transmit the authenticated user's name to the
Echo Pyramid.

This document records the modifications performed both on the AX630C
Cognitive Runtime and on the Echo Pyramid firmware.

------------------------------------------------------------------------

# Engineering Goal

Previous protocol:

``` text
WELCOME
```

New protocol:

``` text
WELCOME|<User Name>
```

Examples:

``` text
WELCOME|Mariana
WELCOME|Hermínio
WELCOME|Claudio
```

Legacy compatibility is preserved:

``` text
WELCOME
```

------------------------------------------------------------------------

# Cognitive Runtime Modifications

**Updated file**

``` text
runtime/cognitive/stackflow/echo_pyramid_adapter.py
```

## Improvements

-   Added protocol documentation.
-   Added command builder abstraction.
-   Added authenticated user extraction.
-   Added parameter sanitization.
-   Preserved legacy protocol compatibility.
-   Improved engineering comments.

## Semantic Event Update

Originally the adapter searched nested structures:

``` text
payload.profile
payload.user
payload.identity
```

The validated Semantic Event actually provides:

``` text
payload.user_name
payload.user_id

context.active_user
```

The adapter was updated to support both formats.

Supported lookup now includes:

``` text
profile.name
profile.display_name
profile.id

payload.user_name
payload.user_id

context.active_user
context.user_name
context.user_id
```

------------------------------------------------------------------------

# Communication Contract

Previous:

``` text
WELCOME
```

Current:

``` text
WELCOME|<Authenticated User>
```

Examples:

``` text
WELCOME|Mariana
WELCOME|Hermínio
WELCOME|Professor
```

------------------------------------------------------------------------

# Echo Pyramid Firmware

Updated firmware:

``` text
firmware/nodes/expression-node/echo-pyramid/
```

## Improvements

-   UDP parser accepts optional parameters.
-   Personalized welcome command parsing.
-   User parameter extraction.
-   Backward compatibility preserved.
-   Documentation improved.

Supported commands:

``` text
WELCOME
WELCOME|Mariana
WELCOME|Hermínio
WELCOME|Professor
```

------------------------------------------------------------------------

# Validation

Validated pipeline:

``` text
Identity Node
    ↓
Identity Package
    ↓
Context Builder
    ↓
Semantic Event Generator
    ↓
Semantic Dispatcher
    ↓
Echo Pyramid Adapter
    ↓
UDP
    ↓
Echo Pyramid
    ↓
Command Parser
    ↓
Expression Processor
    ↓
RGB Welcome
```

Observed firmware output:

``` text
UDP message received: WELCOME|Mariana

Personalized welcome event received via UDP

parameter: Mariana

Personalized welcome requested for authenticated user: Mariana

Expression action: RGB_WELCOME
```

Result:

**PASS**

------------------------------------------------------------------------

# Current Status

Validated:

-   Personalized transport
-   User parameter extraction
-   UTF-8 names
-   Backward compatibility
-   RGB reaction
-   Runtime stability

Pending:

-   Speech synthesis with authenticated user name
-   AtomS3R display integration
-   Wake Word interaction
-   Conversational runtime

------------------------------------------------------------------------

# Engineering Conclusion

The personalized welcome protocol has been successfully integrated
between the Cognitive Runtime and the Expression Layer.

The complete transport path has been validated end-to-end while
preserving compatibility with previous firmware revisions.

------------------------------------------------------------------------

# Validation Milestone

``` text
PERSONALIZED_WELCOME_TRANSPORT_001

Status:
VALIDATED
```
