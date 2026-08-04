#!/usr/bin/env python3
"""
Cognitive Runtime Console UART notifier for Ambient Physical AI.

Publishes the AX630C Cognitive Runtime observability snapshot to the dedicated
CoreS3 Lite console using newline-delimited JSON over ``/dev/ttyS1``.

The module follows the existing StackFlow notifier pattern while preserving a
clear responsibility boundary:

- it does not generate semantic meaning;
- it does not alter the Context Registry;
- it does not dispatch actions to runtime nodes;
- it only collects, normalizes and transports observability data.

Transport contract:
- UART device: /dev/ttyS1 by default;
- line format: one compact UTF-8 JSON document followed by LF;
- serial configuration: 115200 bit/s, 8N1, no flow control;
- document type: ``runtime_status``;
- no third-party Python dependencies.

The CoreS3 Lite parser accepts partial updates, but this notifier deliberately
sends a complete bounded snapshot. Complete snapshots simplify recovery after
console reset, malformed input or temporary link interruption.
"""

from __future__ import annotations

import json
import os
import socket
import subprocess
import threading
from collections import deque
from pathlib import Path
from typing import Any


DEFAULT_UART_DEVICE = "/dev/ttyS1"
DEFAULT_UART_BAUD_RATE = 115200
DEFAULT_EVENT_LIMIT = 5

# Mirror the fixed-size limits implemented by the CoreS3 Lite data model.
USER_MAX_LENGTH = 31
CONTEXT_MAX_LENGTH = 31
STATE_MAX_LENGTH = 31
EVENT_MAX_LENGTH = 63
LAST_RX_MAX_LENGTH = 31
IP_MAX_LENGTH = 23
LINK_MAX_LENGTH = 23
UPTIME_MAX_LENGTH = 23


def _bounded_text(value: Any, maximum_length: int, fallback: str = "") -> str:
    """Convert one value to bounded text compatible with the console model."""
    if value is None:
        return fallback[:maximum_length]

    text = str(value).strip()
    if not text:
        return fallback[:maximum_length]

    return text[:maximum_length]


class CognitiveRuntimeConsoleNotifier:
    """
    Publish complete Cognitive Runtime status snapshots through AX630C UART.

    The instance keeps the last valid logical state in memory. Callers update
    only the information they own, then ``publish()`` sends one coherent
    snapshot to the CoreS3 Lite.
    """

    def __init__(self) -> None:
        self.device = Path(
            os.getenv(
                "COGNITIVE_RUNTIME_CONSOLE_UART_DEVICE",
                DEFAULT_UART_DEVICE,
            )
        )

        self.baud_rate = int(
            os.getenv(
                "COGNITIVE_RUNTIME_CONSOLE_UART_BAUD",
                str(DEFAULT_UART_BAUD_RATE),
            )
        )

        self.enabled = (
            os.getenv(
                "COGNITIVE_RUNTIME_CONSOLE_ENABLED",
                "1",
            ).strip().lower()
            not in {"0", "false", "no", "off"}
        )

        self._lock = threading.Lock()
        self._configured = False
        self._events: deque[str] = deque(maxlen=DEFAULT_EVENT_LIMIT)

        self._services = {
            "linux": True,
            "stackflow": True,
            "mcp": True,
            "semantic": True,
        }

        self._identity = {
            "user": "Unknown",
            "context": "Waiting",
        }

        self._cognitive = {
            "state": "IDLE",
            "last_event": "console_notifier_initialized",
            "last_rx": "AX630C UART",
        }

        self._network = {
            "connected": False,
            "heartbeat": True,
            "rssi_dbm": 0,
            "ip": "0.0.0.0",
            "runtime_link": "AX630C UART",
        }

        self.record_event("Console notifier initialized")

    def configure_uart(self) -> None:
        """
        Configure the Linux UART as 115200 8N1 without flow control.

        Configuration is performed once per notifier instance. A later write
        failure clears the flag so the next publication attempts recovery.
        """
        if not self.enabled:
            return

        if not self.device.exists():
            raise FileNotFoundError(
                f"Runtime Console UART device does not exist: {self.device}"
            )

        subprocess.run(
            [
                "stty",
                "-F",
                str(self.device),
                str(self.baud_rate),
                "cs8",
                "-cstopb",
                "-parenb",
                "-ixon",
                "-ixoff",
                "-crtscts",
            ],
            check=True,
        )

        self._configured = True

    def update_services(
        self,
        *,
        linux: bool | None = None,
        stackflow: bool | None = None,
        mcp: bool | None = None,
        semantic: bool | None = None,
    ) -> None:
        """Update service-availability fields without publishing automatically."""
        with self._lock:
            if linux is not None:
                self._services["linux"] = bool(linux)
            if stackflow is not None:
                self._services["stackflow"] = bool(stackflow)
            if mcp is not None:
                self._services["mcp"] = bool(mcp)
            if semantic is not None:
                self._services["semantic"] = bool(semantic)

    def update_identity(self, *, user: Any, context: Any) -> None:
        """Update the authenticated user and active environment context."""
        with self._lock:
            self._identity["user"] = _bounded_text(
                user,
                USER_MAX_LENGTH,
                "Unknown",
            )
            self._identity["context"] = _bounded_text(
                context,
                CONTEXT_MAX_LENGTH,
                "Waiting",
            )

    def update_cognitive(
        self,
        *,
        state: Any,
        last_event: Any | None = None,
        last_rx: Any = "AX630C UART",
    ) -> None:
        """Update cognitive state and optional semantic-event metadata."""
        with self._lock:
            self._cognitive["state"] = _bounded_text(
                state,
                STATE_MAX_LENGTH,
                "IDLE",
            )

            if last_event is not None:
                self._cognitive["last_event"] = _bounded_text(
                    last_event,
                    EVENT_MAX_LENGTH,
                    "none",
                )

            self._cognitive["last_rx"] = _bounded_text(
                last_rx,
                LAST_RX_MAX_LENGTH,
                "AX630C UART",
            )

    def record_event(self, event: Any) -> None:
        """Append one bounded human-readable entry to the five-item event log."""
        text = _bounded_text(event, EVENT_MAX_LENGTH)
        if not text:
            return

        with self._lock:
            self._events.appendleft(text)

    def update_from_context(self, current_context: dict[str, Any] | None) -> None:
        """
        Extract the Console identity fields from the active Context Registry.

        Expected current project structure:
        - current_context["who"]["id"]
        - current_context["where"]["environment"]
        """
        if not isinstance(current_context, dict):
            return

        who = current_context.get("who", {})
        where = current_context.get("where", {})

        if not isinstance(who, dict):
            who = {}
        if not isinstance(where, dict):
            where = {}

        user = (
            who.get("name")
            or who.get("display_name")
            or who.get("id")
            or "Unknown"
        )

        context = where.get("environment") or "Waiting"

        self.update_identity(
            user=user,
            context=context,
        )

    def update_from_semantic_event(
        self,
        semantic_event: dict[str, Any] | None,
    ) -> None:
        """Extract and record the latest normalized Semantic Event name."""
        if not isinstance(semantic_event, dict):
            return

        event_name = (
            semantic_event.get("event_type")
            or semantic_event.get("event")
        )

        if not isinstance(event_name, str) or not event_name.strip():
            return

        event_name = event_name.strip()

        with self._lock:
            self._cognitive["last_event"] = _bounded_text(
                event_name,
                EVENT_MAX_LENGTH,
                "none",
            )

        self.record_event(event_name)

    @staticmethod
    def _read_cpu_percent() -> int:
        """
        Return a lightweight CPU utilization sample.

        ``os.getloadavg()`` is normalized by logical CPU count. This provides a
        deterministic observability estimate without adding psutil.
        """
        try:
            one_minute_load = os.getloadavg()[0]
            cpu_count = os.cpu_count() or 1
            percent = round((one_minute_load / cpu_count) * 100)
            return max(0, min(100, percent))
        except (AttributeError, OSError):
            return 0

    @staticmethod
    def _read_memory_mb() -> tuple[int, int]:
        """Read used and total RAM from /proc/meminfo."""
        try:
            values: dict[str, int] = {}

            with open(
                "/proc/meminfo",
                "r",
                encoding="utf-8",
            ) as meminfo:
                for line in meminfo:
                    key, raw_value = line.split(":", 1)
                    value_kb = int(raw_value.strip().split()[0])
                    values[key] = value_kb

            total_kb = values.get("MemTotal", 0)
            available_kb = values.get(
                "MemAvailable",
                values.get("MemFree", 0),
            )
            used_kb = max(0, total_kb - available_kb)

            return used_kb // 1024, total_kb // 1024

        except (OSError, ValueError):
            return 0, 0

    @staticmethod
    def _read_temperature_c() -> int:
        """Read the first valid Linux thermal-zone temperature."""
        thermal_root = Path("/sys/class/thermal")

        try:
            for temperature_file in thermal_root.glob(
                "thermal_zone*/temp"
            ):
                raw_value = temperature_file.read_text(
                    encoding="utf-8"
                ).strip()

                if not raw_value:
                    continue

                temperature = int(raw_value)

                # Linux thermal zones commonly expose millidegrees Celsius.
                if abs(temperature) >= 1000:
                    temperature = round(temperature / 1000)

                if -100 <= temperature <= 200:
                    return temperature

        except (OSError, ValueError):
            pass

        return 0

    @staticmethod
    def _read_uptime() -> str:
        """Return compact Linux uptime text bounded for the console."""
        try:
            raw_seconds = float(
                Path("/proc/uptime")
                .read_text(encoding="utf-8")
                .split()[0]
            )
        except (OSError, ValueError, IndexError):
            return "--"

        total_minutes = int(raw_seconds // 60)
        days, remaining_minutes = divmod(total_minutes, 24 * 60)
        hours, minutes = divmod(remaining_minutes, 60)

        if days > 0:
            text = f"{days}d {hours:02d}h {minutes:02d}m"
        else:
            text = f"{hours:02d}h {minutes:02d}m"

        return _bounded_text(text, UPTIME_MAX_LENGTH, "--")

    @staticmethod
    def _read_primary_ip() -> str:
        """
        Resolve the primary outbound IPv4 address without sending application data.
        """
        try:
            with socket.socket(
                socket.AF_INET,
                socket.SOCK_DGRAM,
            ) as sock:
                sock.connect(("192.0.2.1", 9))
                address = sock.getsockname()[0]

            return _bounded_text(
                address,
                IP_MAX_LENGTH,
                "0.0.0.0",
            )

        except OSError:
            return "0.0.0.0"

    def _build_health(self) -> dict[str, Any]:
        """Collect the Linux telemetry fields expected by the console."""
        ram_used_mb, ram_total_mb = self._read_memory_mb()

        return {
            "cpu_percent": self._read_cpu_percent(),
            "ram_used_mb": min(65535, ram_used_mb),
            "ram_total_mb": min(65535, ram_total_mb),
            "temperature_c": self._read_temperature_c(),
            "uptime": self._read_uptime(),
        }

    def _refresh_network(self) -> None:
        """Refresh IP and link availability while preserving heartbeat state."""
        address = self._read_primary_ip()

        self._network["ip"] = address
        self._network["connected"] = address != "0.0.0.0"
        self._network["runtime_link"] = _bounded_text(
            "AX630C UART",
            LINK_MAX_LENGTH,
            "AX630C UART",
        )

    def build_snapshot(self) -> dict[str, Any]:
        """Build one complete Console-compatible runtime_status document."""
        with self._lock:
            self._refresh_network()

            return {
                "type": "runtime_status",
                "services": dict(self._services),
                "identity": dict(self._identity),
                "cognitive": dict(self._cognitive),
                "health": self._build_health(),
                "network": dict(self._network),
                "events": list(self._events),
            }

    def publish(self) -> bool:
        """
        Send one complete newline-delimited runtime snapshot.

        Returns ``False`` when explicitly disabled. Transport failures are
        raised so the caller can log them without hiding integration defects.
        """
        if not self.enabled:
            print("Cognitive Runtime Console UART delivery: DISABLED")
            return False

        with self._lock:
            if not self._configured:
                self.configure_uart()

        snapshot = self.build_snapshot()

        document = json.dumps(
            snapshot,
            ensure_ascii=False,
            separators=(",", ":"),
        )
        encoded_document = (document + "\n").encode("utf-8")

        try:
            with self.device.open("wb", buffering=0) as uart:
                written = uart.write(encoded_document)

        except OSError:
            self._configured = False
            raise

        if written != len(encoded_document):
            self._configured = False
            raise OSError(
                "Incomplete Runtime Console UART transmission: "
                f"{written}/{len(encoded_document)} bytes"
            )

        print(
            "Cognitive Runtime Console snapshot sent: "
            f"state={snapshot['cognitive']['state']} "
            f"user={snapshot['identity']['user']} "
            f"context={snapshot['identity']['context']} "
            f"bytes={written}"
        )

        return True
