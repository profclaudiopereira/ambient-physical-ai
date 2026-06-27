#!/usr/bin/env python3
"""
Context Registry for Ambient Physical AI.

Maintains the current semantic Context Package produced by the
Cognitive Runtime.

The registry is intentionally lightweight. It stores only the latest
validated Context Package in memory, allowing future semantic services
(MCP tools) to query the current runtime state.
"""

from copy import deepcopy

_current_context = None


def update_context(context_package):
    """
    Store the latest validated Context Package.
    """
    global _current_context
    _current_context = deepcopy(context_package)


def get_current_context():
    """
    Return the current Context Package.

    A copy is returned to avoid accidental modifications.
    """
    if _current_context is None:
        return None

    return deepcopy(_current_context)


def has_context():
    """
    Returns True if a Context Package is currently available.
    """
    return _current_context is not None


def clear_context():
    """
    Remove the current Context Package.
    """
    global _current_context
    _current_context = None