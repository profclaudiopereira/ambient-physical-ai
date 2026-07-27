#!/usr/bin/env python3
"""
Runtime State Notifier for Ambient Physical AI.

Delivers Cognitive Runtime State messages to the dedicated Runtime State
Indicator (StickC Plus2).

Runtime State is intentionally independent from Semantic Events.

Its purpose is to expose the internal execution state of the Cognitive
Runtime without affecting the semantic decision pipeline.
"""

import json
import os
import socket


class RuntimeStateNotifier:
    def __init__(self):
        self.mode = os.getenv(
            "RUNTIME_STATE_NOTIFY_MODE",
            "udp",
        ).lower()

        self.host = os.getenv(
            "RUNTIME_STATE_HOST",
            "192.168.77.27",
        )

        self.port = int(
            os.getenv(
                "RUNTIME_STATE_PORT",
                "5555",
            )
        )

    def notify(self, runtime_state: str):
        """Send one Runtime State message."""

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

    def _notify_udp(self, payload):
        if not self.host or self.port <= 0:
            print(
                "Runtime State UDP mode selected, "
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
            f"Runtime State sent to "
            f"{self.host}:{self.port}"
        )

        return True
