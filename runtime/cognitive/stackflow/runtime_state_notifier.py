#!/usr/bin/env python3
"""
Runtime State Notifier for Ambient Physical AI.

Delivers Cognitive Runtime State messages to the dedicated Runtime State
Indicators of the Ambient Physical AI Expression Layer.

The same normalized Runtime State message is delivered to:

- Runtime State RGB Node;
- Wearable Haptic Node.

Runtime State delivery is intentionally independent from Semantic Events.

Its purpose is to expose the internal execution state of the Cognitive
Runtime without affecting the semantic decision pipeline.
"""

import json
import os
import socket
from typing import Any


DEFAULT_RUNTIME_STATE_HOSTS = (
    "192.168.1.207",
    "192.168.1.208",
)

DEFAULT_RUNTIME_STATE_PORT = 5555


class RuntimeStateNotifier:
    """Publishes normalized Runtime State messages to Expression Layer nodes."""

    def __init__(self) -> None:
        self.mode = os.getenv(
            "RUNTIME_STATE_NOTIFY_MODE",
            "udp",
        ).strip().lower()

        self.hosts = self._load_hosts()

        self.port = int(
            os.getenv(
                "RUNTIME_STATE_PORT",
                str(DEFAULT_RUNTIME_STATE_PORT),
            )
        )

    @staticmethod
    def _load_hosts() -> tuple[str, ...]:
        """
        Load Runtime State destination hosts.

        Preferred configuration:

            RUNTIME_STATE_HOSTS=192.168.1.207,192.168.1.208

        Legacy single-host configuration remains supported through:

            RUNTIME_STATE_HOST=192.168.1.207
        """

        configured_hosts = os.getenv(
            "RUNTIME_STATE_HOSTS",
            "",
        ).strip()

        if configured_hosts:
            hosts = tuple(
                host.strip()
                for host in configured_hosts.split(",")
                if host.strip()
            )

            if hosts:
                return hosts

        legacy_host = os.getenv(
            "RUNTIME_STATE_HOST",
            "",
        ).strip()

        if legacy_host:
            return (legacy_host,)

        return DEFAULT_RUNTIME_STATE_HOSTS

    def notify(self, runtime_state: str) -> bool:
        """Send one normalized Runtime State message."""

        if not isinstance(runtime_state, str):
            raise ValueError("runtime_state must be a string")

        runtime_state = runtime_state.strip()

        if not runtime_state:
            raise ValueError("runtime_state cannot be empty")

        payload = {
            "type": "runtime_state",
            "target": "runtime_state_indicator",
            "state": runtime_state,
        }

        if self.mode == "udp":
            return self._notify_udp(payload)

        print("\nRuntime State message prepared:")
        print(json.dumps(payload, ensure_ascii=False, indent=2))
        print("Runtime State delivery: DISABLED")

        return False

    def _notify_udp(self, payload: dict[str, Any]) -> bool:
        """Deliver the same Runtime State payload to all configured nodes."""

        if not self.hosts or self.port <= 0:
            print(
                "Runtime State UDP mode selected, "
                "but destinations or port are not configured."
            )
            return False

        data = json.dumps(
            payload,
            ensure_ascii=False,
        ).encode("utf-8")

        successful_deliveries = 0

        with socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM,
        ) as sock:
            for host in self.hosts:
                try:
                    sock.sendto(
                        data,
                        (host, self.port),
                    )

                    successful_deliveries += 1

                    print(
                        f"Runtime State sent to "
                        f"{host}:{self.port}"
                    )

                except OSError as error:
                    print(
                        f"Runtime State delivery failed for "
                        f"{host}:{self.port}: {error}"
                    )

        if successful_deliveries == 0:
            print("Runtime State delivery failed for all destinations.")
            return False

        if successful_deliveries < len(self.hosts):
            print(
                "Runtime State delivery completed with partial success: "
                f"{successful_deliveries}/{len(self.hosts)} destinations."
            )
        else:
            print(
                "Runtime State delivered to all configured destinations: "
                f"{successful_deliveries}/{len(self.hosts)}."
            )

        return True