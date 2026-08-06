#!/usr/bin/env python3
"""
Ambient Runtime Notifier for Ambient Physical AI.

Consumes normalized Semantic Event V1 objects and delivers a compact Ambient
Runtime message to the Tab5 through UDP. Semantic meaning remains centralized
in the AX630C; this adapter only translates and transports the event.
"""

import json
import os
import socket

from semantic_event import is_semantic_event


class AmbientRuntimeNotifier:
    def __init__(self):
        self.mode = os.getenv(
            "AMBIENT_RUNTIME_NOTIFY_MODE",
            "udp",
        ).lower()

        self.host = os.getenv(
            "AMBIENT_RUNTIME_HOST",
            "192.168.1.203",
        )

        self.port = int(
            os.getenv(
                "AMBIENT_RUNTIME_PORT",
                "5555",
            )
        )

    def notify(self, semantic_event):
        """
        Consume and deliver one normalized Semantic Event V1.

        The payload is forwarded because context_changed consumers need the
        previous/current environment and request source. Existing receivers
        remain compatible because the original envelope fields are unchanged.
        """
        if not is_semantic_event(semantic_event):
            raise ValueError("invalid Semantic Event V1")

        event_name = (
            semantic_event.get("event_type")
            or semantic_event.get("event")
        )

        if not isinstance(event_name, str) or not event_name.strip():
            raise ValueError(
                "Semantic Event does not contain a valid event"
            )

        event_payload = semantic_event.get("payload", {})
        if not isinstance(event_payload, dict):
            event_payload = {}

        event_context = semantic_event.get("context", {})
        if not isinstance(event_context, dict):
            event_context = {}

        payload = {
            "type": "semantic_event",
            "event_type": event_name.strip(),
            "target": "ambient_runtime",
            "payload": event_payload,
            "context": event_context,
            "source": semantic_event.get(
                "source",
                "ax630c_cognitive_runtime",
            ),
        }

        if self.mode == "udp":
            return self._notify_udp(payload)

        print("\nAmbient Runtime semantic message prepared:")
        print(json.dumps(payload, ensure_ascii=False, indent=2))
        print("Ambient Runtime delivery: DISABLED")
        return False

    def _notify_udp(self, payload):
        if not self.host or self.port <= 0:
            print(
                "Ambient Runtime UDP mode selected, "
                "but host/port not configured."
            )
            return False

        data = json.dumps(
            payload,
            ensure_ascii=False,
        ).encode("utf-8")

        with socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM,
        ) as sock:
            sock.sendto(
                data,
                (self.host, self.port),
            )

        print(
            f"Ambient Runtime semantic event sent to "
            f"{self.host}:{self.port}"
        )

        return True
