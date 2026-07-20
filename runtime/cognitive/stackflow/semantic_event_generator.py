#!/usr/bin/env python3
"""
Semantic Event Generator for Ambient Physical AI.

Transforms the current validated Context Package into normalized
Semantic Event V1 objects.

This module does not deliver events and does not know device protocols.
Its only responsibility is semantic event generation.
"""

from semantic_event import build_semantic_event


ROLE_PROFILES = {
    "owner": {
        "normalized_role": "researcher",
        "access_level": "full_research",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
            "research_equipment",
            "cognitive_runtime",
            "administration",
        ],
    },
    "researcher": {
        "normalized_role": "researcher",
        "access_level": "full_research",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
            "research_equipment",
            "cognitive_runtime",
            "administration",
        ],
    },
    "technician": {
        "normalized_role": "technician",
        "access_level": "technical_operations",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
            "research_equipment",
            "maintenance",
        ],
    },
    "learner": {
        "normalized_role": "student",
        "access_level": "limited_research",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
            "research_equipment_limited",
            "cognitive_runtime_limited",
        ],
    },
    "student": {
        "normalized_role": "student",
        "access_level": "limited_research",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
            "research_equipment_limited",
            "cognitive_runtime_limited",
        ],
    },
    "visitor": {
        "normalized_role": "visitor",
        "access_level": "ambient_only",
        "capabilities": [
            "ambient_runtime",
            "expression_layer",
        ],
    },
}


DEFAULT_ROLE_PROFILE = {
    "normalized_role": "visitor",
    "access_level": "ambient_only",
    "capabilities": [
        "ambient_runtime",
        "expression_layer",
    ],
}


def _resolve_authorization(role):
    """
    Resolve a legacy or normalized role into the V1 authorization model.
    """
    normalized_key = str(role or "unknown").strip().lower()
    profile = ROLE_PROFILES.get(normalized_key, DEFAULT_ROLE_PROFILE)

    return {
        "role": profile["normalized_role"],
        "access_level": profile["access_level"],
        "capabilities": list(profile["capabilities"]),
    }


def _compact_context(context_package, authorization):
    """
    Extract only the minimum context required by downstream consumers.
    """
    who = context_package.get("who", {})
    where = context_package.get("where", {})
    what = context_package.get("what", {})

    return {
        "active_user": who.get("name", "Unknown"),
        "user_id": who.get("id", "unknown"),
        "role": authorization["role"],
        "access_level": authorization["access_level"],
        "capabilities": authorization["capabilities"],
        "environment": where.get("environment", "Unknown"),
        "activity": what.get("activity", "unknown"),
        "state": what.get("state", "unknown"),
    }


def generate_semantic_events(context_package):
    """
    Generate the initial Semantic Event set from a Context Package.

    Returns:
        A list containing normalized Semantic Event V1 dictionaries.
    """
    if not isinstance(context_package, dict):
        raise ValueError("context_package must be a dictionary")

    who = context_package.get("who", {})
    where = context_package.get("where", {})
    what = context_package.get("what", {})

    user_name = who.get("name", "Unknown")
    user_id = who.get("id", "unknown")
    source_role = who.get("role", "unknown")
    environment = where.get("environment", "Unknown")
    activity = what.get("activity", "unknown")
    state = what.get("state", "unknown")

    authorization = _resolve_authorization(source_role)
    compact_context = _compact_context(context_package, authorization)

    identity_event = build_semantic_event(
        event_type="identity_authenticated",
        target="cognitive_runtime",
        priority="high",
        payload={
            "user_id": user_id,
            "user_name": user_name,
            "source_role": source_role,
            "role": authorization["role"],
            "access_level": authorization["access_level"],
            "capabilities": authorization["capabilities"],
            "environment": environment,
            "activity": activity,
            "state": state,
        },
        context=compact_context,
        source="semantic_event_generator_v1",
    )

    return [
        identity_event,
    ]