#!/usr/bin/env python3

import json
import os
import socket


class StackChanNotifier:
    def __init__(self):
        self.mode = os.getenv("STACKCHAN_NOTIFY_MODE", "dry-run").lower()
        self.host = os.getenv("STACKCHAN_HOST", "")
        self.port = int(os.getenv("STACKCHAN_PORT", "0"))

    def notify(self, message, context):
        payload = {
            "type": "stackchan_notification",
            "message": message,
            "source": "ax630c_cognitive_runtime",
            "context": {
                "active_user": context.get("active_user"),
                "role": context.get("role"),
                "environment_context": context.get("environment_context"),
            },
        }

        if self.mode == "udp":
            return self._notify_udp(payload)

        print("\nStackChan notification prepared:")
        print(json.dumps(payload, ensure_ascii=False, indent=2))
        print("StackChan delivery: PENDING")
        return False

    def _notify_udp(self, payload):
        if not self.host or self.port <= 0:
            print("StackChan UDP mode selected, but host/port not configured.")
            return False

        data = json.dumps(payload, ensure_ascii=False).encode("utf-8")

        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.sendto(data, (self.host, self.port))

        print(f"StackChan UDP notification sent to {self.host}:{self.port}")
        return True
