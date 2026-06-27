#!/usr/bin/env python3
"""
Semantic Services for Ambient Physical AI.

Provides a simple semantic interface to the Current Runtime Context.
These services are local to the Cognitive Runtime and will later be
exposed through MCP without changing their internal logic.
"""

from context_registry import (
    get_current_context as registry_get_current_context,
    has_context,
)


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