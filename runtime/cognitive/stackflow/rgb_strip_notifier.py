#!/usr/bin/env python3
"""
RGB Strip Notifier for Ambient Physical AI.

Consumes normalized Semantic Event V1 objects and delivers a compact
Expression Layer message to the RGB Strip Node through UDP.

This adapter does not generate semantic meaning. It only translates
the normalized runtime event into the delivery contract currently
supported by the RGB Strip Expression Node.
"""

import json
import os
import socket

from semantic_event import is_semantic_event


class RGBStripNotifier:
    def __init__(self):
        self.mode = os.getenv("RGB_STRIP_NOTIFY_MODE", "udp").lower()
        self.host = os.getenv("RGB_STRIP_HOST", "192.168.77.26")
        self.port = int(os.getenv("RGB_STRIP_PORT", "5555"))

    def notify(self, semantic_event):
        """
        Consume a normalized Semantic Event V1.

        Returns:
            True when the event is sent through UDP.
            False when delivery is disabled or only prepared.
        """
        if not is_semantic_event(semantic_event):
            raise ValueError("invalid Semantic Event V1")

        event_name = semantic_event.get("event_type") or semantic_event.get("event")

        if not isinstance(event_name, str) or not event_name.strip():
            raise ValueError("Semantic Event does not contain a valid event")

        payload = {
            "type": "semantic_event",
            "event": event_name.strip(),
            "target": "rgb_strip",
        }

        if self.mode == "udp":
            return self._notify_udp(payload)

        print("\nRGB Strip semantic message prepared:")
        print(json.dumps(payload, ensure_ascii=False, indent=2))
        print("RGB Strip delivery: DISABLED")
        return False

    def _notify_udp(self, payload):
        if not self.host or self.port <= 0:
            print("RGB Strip UDP mode selected, but host/port not configured.")
            return False

        data = json.dumps(payload, ensure_ascii=False).encode("utf-8")

        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.sendto(data, (self.host, self.port))

        print(
            f"RGB Strip semantic event sent to "
            f"{self.host}:{self.port}"
        )

        return True