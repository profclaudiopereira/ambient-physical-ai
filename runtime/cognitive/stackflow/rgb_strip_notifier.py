#!/usr/bin/env python3
"""
RGB Expression Notifier for Ambient Physical AI.

Delivers compact Semantic Event messages to every configured RGB expression
node. The Cognitive Runtime sends semantic meaning; each node remains
responsible for mapping supported events to its own physical behavior.
"""

import json
import os
import socket
from typing import List, Tuple

from semantic_event import is_semantic_event


class RGBStripNotifier:
    DEFAULT_HOSTS = [
        "192.168.1.205",
        "192.168.1.206",
    ]

    def __init__(self):
        self.mode = os.getenv(
            "RGB_STRIP_NOTIFY_MODE",
            "udp",
        ).strip().lower()

        self.port = int(
            os.getenv(
                "RGB_STRIP_PORT",
                "5555",
            )
        )

        self.hosts = self._load_hosts()

    def _load_hosts(self) -> List[str]:
        """
        Resolve configured RGB destinations while preserving legacy settings.
        """
        configured_hosts = os.getenv("RGB_EXPRESSION_HOSTS")

        if configured_hosts:
            raw_hosts = configured_hosts.split(",")
        else:
            legacy_host = os.getenv("RGB_STRIP_HOST")
            raw_hosts = [legacy_host] if legacy_host else list(self.DEFAULT_HOSTS)

        hosts = []
        seen = set()

        for raw_host in raw_hosts:
            host = raw_host.strip()
            if not host or host in seen:
                continue
            hosts.append(host)
            seen.add(host)

        return hosts

    def notify(self, semantic_event) -> bool:
        """
        Deliver one Semantic Event to every configured RGB expression node.

        Context-change fields are forwarded without changing the existing
        identity_authenticated contract. Nodes that do not yet implement
        context_changed may ignore it safely at their semantic-consumer layer.
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

        payload = {
            "type": "semantic_event",
            "event": event_name.strip(),
            "target": "rgb_strip",
            "payload": {
                "user_id": str(
                    event_payload.get("user_id", "unknown")
                ).strip().lower(),
                "user_name": event_payload.get("user_name", "Unknown"),
                "previous_context": event_payload.get("previous_context"),
                "environment": event_payload.get("environment"),
                "request_source": event_payload.get("request_source"),
            },
        }

        if self.mode == "udp":
            return self._notify_udp(payload)

        print("\nRGB expression semantic message prepared:")
        print(json.dumps(payload, ensure_ascii=False, indent=2))
        print("RGB expression delivery: DISABLED")
        return False

    def _notify_udp(self, payload) -> bool:
        if not self.hosts:
            print(
                "RGB expression UDP mode selected, "
                "but no hosts are configured."
            )
            return False

        if self.port <= 0 or self.port > 65535:
            print(
                "RGB expression UDP mode selected, "
                "but the port is invalid."
            )
            return False

        data = json.dumps(
            payload,
            ensure_ascii=False,
        ).encode("utf-8")

        delivery_results: List[Tuple[str, bool]] = []

        for host in self.hosts:
            delivered = self._send_datagram(host, data)
            delivery_results.append((host, delivered))

        successful_deliveries = sum(
            1 for _, delivered in delivery_results if delivered
        )

        print(
            "RGB expression delivery summary: "
            f"{successful_deliveries}/{len(delivery_results)} sent"
        )

        return all(delivered for _, delivered in delivery_results)

    def _send_datagram(self, host: str, data: bytes) -> bool:
        try:
            with socket.socket(
                socket.AF_INET,
                socket.SOCK_DGRAM,
            ) as sock:
                sock.sendto(data, (host, self.port))

            print(
                "RGB expression semantic event sent to "
                f"{host}:{self.port}"
            )
            return True

        except OSError as exc:
            print(
                "RGB expression semantic event delivery failed for "
                f"{host}:{self.port}: {exc}"
            )
            return False
