#!/usr/bin/env python3
"""
Context Builder for Ambient Physical AI.

This module converts validated physical and cognitive events into a semantic
5W Context Package.

The Context Package is the central semantic object of the Cognitive Runtime.
It is designed to be consumed later by MCP services and expressive agents.
"""

from datetime import datetime, timezone


def _utc_now_iso():
    return datetime.now(timezone.utc).isoformat()


def build_context_package(identity_package, presence_event=None, environment=None):
    profile = identity_package.get("profile", {})
    nfc = identity_package.get("nfc", {})

    environment_context = identity_package.get(
        "current_context",
        identity_package.get("context", "Unknown"),
    )
    now = _utc_now_iso()

    context_package = {
        "schema_version": "1.0",
        "package_type": "context_package",

        "who": {
            "id": profile.get("id", "unknown"),
            "name": profile.get("name", "Unknown"),
            "role": profile.get("role", "unknown"),
            "identity_source": "nfc",
            "uid": nfc.get("uid", "unknown"),
            "identified": bool(nfc.get("card_present", False)),
        },

        "where": {
            "environment": environment_context,
            "presence_zone": None,
            "location_source": "identity_context",
        },

        "when": {
            "timestamp": now,
            "presence_detected_at": None,
            "identity_confirmed_at": now,
        },

        "what": {
            "activity": "identity_authentication",
            "state": "validated",
            "event_type": identity_package.get("type", "identity_package"),
        },

        "why": {
            "intent": None,
            "reasoning": "pending semantic reasoning",
            "confidence": 0.0,
        },

        "source": {
            "identity_package_source": identity_package.get("source", "unknown"),
            "context_builder": "context_builder_v1",
        },
    }

    if presence_event:
        context_package["where"]["presence_zone"] = presence_event.get("zone")
        context_package["when"]["presence_detected_at"] = presence_event.get("timestamp")
        context_package["source"]["presence_event_source"] = presence_event.get(
            "source",
            "unknown",
        )

    if environment:
        context_package["where"]["environment"] = environment.get(
            "environment",
            context_package["where"]["environment"],
        )
        context_package["source"]["environment_source"] = environment.get(
            "source",
            "unknown",
        )

    return context_package


def build_context(identity_package):
    """
    Backward-compatible wrapper.

    Older callers used build_context().
    It now returns the 5W Context Package.
    """
    return build_context_package(identity_package)


def build_human_message(context_package):
    """
    Compatibility helper for terminal validation.

    Human-readable messages are no longer the primary output of the Context Builder.
    They remain available only for debugging and comparison with previous labs.
    """
    who = context_package.get("who", {})
    where = context_package.get("where", {})

    user = who.get("name", "Unknown")
    role = who.get("role", "unknown")
    env = where.get("environment", "Unknown")

    if role == "owner" or user.lower() == "claudio":
        greeting = f"Olá {user}, bem-vindo ao laboratório."
    elif role == "learner" or user.lower() == "student":
        greeting = f"Olá {user}, bem-vindo à demonstração."
    else:
        greeting = f"Olá {user}, bem-vindo ao ambiente."

    return f"{greeting}\nContexto: {env}."
