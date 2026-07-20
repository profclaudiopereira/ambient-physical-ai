#!/usr/bin/env python3
"""
Semantic Dispatcher for Ambient Physical AI.

Routes normalized Semantic Event V1 objects to registered adapters.

This module does not generate semantic meaning and does not implement
device protocols. Its only responsibility is event routing.
"""

from semantic_event import is_semantic_event


class SemanticDispatcher:
    def __init__(self):
        self._adapters = {}

    def register_adapter(self, target, adapter):
        """
        Register an adapter for a logical target.
        """
        if not isinstance(target, str) or not target.strip():
            raise ValueError("target must be a non-empty string")

        if not callable(adapter):
            raise ValueError("adapter must be callable")

        self._adapters[target.strip()] = adapter

    def unregister_adapter(self, target):
        """
        Remove an adapter registration.
        """
        self._adapters.pop(target, None)

    def dispatch(self, semantic_event):
        """
        Route a Semantic Event to every registered adapter.

        Each adapter receives the same normalized Semantic Event.
        """
        if not is_semantic_event(semantic_event):
            raise ValueError("invalid Semantic Event V1")

        results = {}

        for target, adapter in self._adapters.items():
            try:
                results[target] = bool(adapter(semantic_event))
            except Exception as exc:
                print(f"Semantic Dispatcher adapter error [{target}]: {exc}")
                results[target] = False

        return results