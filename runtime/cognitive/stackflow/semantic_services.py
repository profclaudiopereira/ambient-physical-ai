#!/usr/bin/env python3
"""
Semantic Services for Ambient Physical AI.

Provides a simple semantic interface to the Current Runtime Context.
These services are local to the Cognitive Runtime and will later be
exposed through MCP without changing their internal logic.

This module also centralizes the user-facing voice message selected after
identity resolution. The Cognitive Runtime decides what must be spoken;
Expression Nodes only render the resulting audio.
"""

from __future__ import annotations

from typing import Any, Mapping, Optional

from context_registry import (
    get_current_context as registry_get_current_context,
    has_context,
)


UNKNOWN_BADGE_MESSAGE = (
    "Crachá não identificado. Por favor, procure a administração."
)

DEFAULT_IDENTIFIED_MESSAGE = "Bem-vindo ao laboratório."

# Explicit grammatical forms for known profiles. Keeping this mapping local
# avoids forcing presentation concerns into the NFC/identity transport layer.
PROFILE_GREETING_TEMPLATES = {
    "herminio": "Bem-vindo ao laboratório, {name}.",
    "claudio": "Bem-vindo ao laboratório, {name}.",
    "mariana": "Bem-vinda ao laboratório, {name}.",
}


def get_runtime_status():
    """
    Return basic runtime information.
    """
    return {
        "runtime": "running",
        "context_available": has_context(),
    }


def get_current_context():
    """
    Return the complete Current Runtime Context.
    """
    return registry_get_current_context()


def get_current_identity():
    """
    Return only the WHO section of the current context.
    """
    context = registry_get_current_context()

    if context is None:
        return None

    return context.get("who")


def get_current_environment():
    """
    Return only the WHERE section of the current context.
    """
    context = registry_get_current_context()

    if context is None:
        return None

    return context.get("where")


def get_current_activity():
    """
    Return only the WHAT section of the current context.
    """
    context = registry_get_current_context()

    if context is None:
        return None

    return context.get("what")


def build_identity_voice_message(
    identity: Optional[Mapping[str, Any]] = None,
) -> str:
    """
    Build the canonical spoken message for the current identity result.

    Contract:
    - A recognized identity produces a personalized laboratory greeting.
    - An absent, unauthenticated, unknown, or incomplete identity produces
      the administration guidance message.
    - The caller may pass an explicit WHO/identity mapping. If omitted, the
      function reads the current identity from the Context Registry.

    Supported identity fields are intentionally permissive because older
    integration stages may use slightly different names:
    - profile_id / id
    - display_name / name
    - authenticated / recognized / identified
    """
    resolved_identity = identity if identity is not None else get_current_identity()

    if not isinstance(resolved_identity, Mapping):
        return UNKNOWN_BADGE_MESSAGE

    if not _identity_is_recognized(resolved_identity):
        return UNKNOWN_BADGE_MESSAGE

    profile_id = _first_text(
        resolved_identity.get("profile_id"),
        resolved_identity.get("id"),
    )
    display_name = _first_text(
        resolved_identity.get("display_name"),
        resolved_identity.get("name"),
        profile_id,
    )

    if not profile_id or not display_name:
        return UNKNOWN_BADGE_MESSAGE

    normalized_profile_id = _normalize_identifier(profile_id)
    template = PROFILE_GREETING_TEMPLATES.get(normalized_profile_id)

    if template is None:
        return f"Bem-vindo ao laboratório, {display_name}."

    return template.format(name=display_name)


def get_current_identity_voice_message() -> str:
    """
    Return the voice message corresponding to the current runtime identity.
    """
    return build_identity_voice_message(get_current_identity())


def _identity_is_recognized(identity: Mapping[str, Any]) -> bool:
    """
    Determine whether an identity mapping represents a valid known profile.

    Explicit negative flags always win. When no status flag is supplied,
    the presence of both an identifier and a display name is accepted for
    compatibility with the already validated Identity Package.
    """
    status_values = (
        identity.get("authenticated"),
        identity.get("recognized"),
        identity.get("identified"),
    )

    explicit_values = [value for value in status_values if value is not None]

    if explicit_values and not all(_as_bool(value) for value in explicit_values):
        return False

    profile_id = _first_text(
        identity.get("profile_id"),
        identity.get("id"),
    )
    display_name = _first_text(
        identity.get("display_name"),
        identity.get("name"),
    )

    return bool(profile_id and display_name)


def _as_bool(value: Any) -> bool:
    """
    Convert common transport representations to a boolean.
    """
    if isinstance(value, bool):
        return value

    if isinstance(value, (int, float)):
        return value != 0

    if isinstance(value, str):
        return value.strip().lower() in {
            "1",
            "true",
            "yes",
            "recognized",
            "identified",
            "authenticated",
            "known",
        }

    return False


def _first_text(*values: Any) -> Optional[str]:
    """
    Return the first non-empty textual value.
    """
    for value in values:
        if value is None:
            continue

        text = str(value).strip()
        if text:
            return text

    return None


def _normalize_identifier(value: str) -> str:
    """
    Normalize profile identifiers for stable lookup across transport formats.
    """
    return value.strip().lower().replace("í", "i")
