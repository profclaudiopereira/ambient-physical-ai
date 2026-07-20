#!/usr/bin/env python3
"""
Semantic Event Contract for Ambient Physical AI.

Defines the normalized Semantic Event V1 structure used by the
Cognitive Runtime and its downstream adapters.
"""

from copy import deepcopy
from datetime import datetime, timezone
from uuid import uuid4


SEMANTIC_EVENT_SCHEMA_VERSION = "1.0"


def _utc_now_iso():
    return datetime.now(timezone.utc).isoformat()


def build_semantic_event(
    event_type,
    target,
    payload,
    *,
    source="ax630c_cognitive_runtime",
    priority="normal",
    context=None,
):
    """
    Build a normalized Semantic Event V1.

    Args:
        event_type: Semantic meaning of the event.
        target: Intended logical consumer.
        payload: Event-specific normalized data.
        source: Runtime component that produced the event.
        priority: Event priority.
        context: Optional compact semantic context.

    Returns:
        A new Semantic Event dictionary.
    """
    if not isinstance(event_type, str) or not event_type.strip():
        raise ValueError("event_type must be a non-empty string")

    if not isinstance(target, str) or not target.strip():
        raise ValueError("target must be a non-empty string")

    if not isinstance(payload, dict):
        raise ValueError("payload must be a dictionary")

    if context is not None and not isinstance(context, dict):
        raise ValueError("context must be a dictionary or None")

    semantic_event = {
        "schema_version": SEMANTIC_EVENT_SCHEMA_VERSION,
        "type": "semantic_event",
        "event_id": str(uuid4()),
        "event_type": event_type.strip(),
        "target": target.strip(),
        "priority": priority,
        "timestamp": _utc_now_iso(),
        "source": source,
        "payload": deepcopy(payload),
    }

    if context is not None:
        semantic_event["context"] = deepcopy(context)

    return semantic_event


def is_semantic_event(candidate):
    """
    Return True when the object matches the minimum Semantic Event V1 contract.
    """
    if not isinstance(candidate, dict):
        return False

    required_fields = {
        "schema_version",
        "type",
        "event_id",
        "event_type",
        "target",
        "priority",
        "timestamp",
        "source",
        "payload",
    }

    if not required_fields.issubset(candidate.keys()):
        return False

    if candidate.get("type") != "semantic_event":
        return False

    if candidate.get("schema_version") != SEMANTIC_EVENT_SCHEMA_VERSION:
        return False

    if not isinstance(candidate.get("payload"), dict):
        return False

    return True