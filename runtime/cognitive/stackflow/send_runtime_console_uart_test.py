#!/usr/bin/env python3
"""
UART validation sender for the Ambient Physical AI Cognitive Runtime Console.

This laboratory script sends one complete ``runtime_status`` JSON document
from the AX630C to the CoreS3 Lite through ``/dev/ttyS1``.

Engineering constraints:
- Uses only the Python standard library.
- Does not require PySerial.
- Configures the Linux UART explicitly before transmission.
- Sends exactly one newline-delimited JSON document.
- Does not modify or start any Cognitive Runtime service.
- Intended only for physical-link and console-ingestion validation.
"""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


UART_DEVICE = Path("/dev/ttyS1")
UART_BAUD_RATE = 115200


def configure_uart() -> None:
    """
    Configure the AX630C UART as 115200 bit/s, 8N1, without flow control.
    """
    if not UART_DEVICE.exists():
        raise FileNotFoundError(
            f"UART device does not exist: {UART_DEVICE}"
        )

    subprocess.run(
        [
            "stty",
            "-F",
            str(UART_DEVICE),
            str(UART_BAUD_RATE),
            "cs8",
            "-cstopb",
            "-parenb",
            "-ixon",
            "-ixoff",
            "-crtscts",
        ],
        check=True,
    )


def build_test_payload() -> dict[str, Any]:
    """Build the complete runtime-status document used by this validation."""
    return {
        "type": "runtime_status",
        "services": {
            "linux": True,
            "stackflow": True,
            "mcp": True,
            "semantic": True,
        },
        "identity": {
            "user": "Claudio",
            "context": "Lab",
        },
        "cognitive": {
            "state": "UART TEST",
            "last_event": "physical_link_validation",
            "last_rx": "AX630C UART",
        },
        "health": {
            "cpu_percent": 18,
            "ram_used_mb": 512,
            "ram_total_mb": 2048,
            "temperature_c": 46,
            "uptime": "UART TEST",
        },
        "network": {
            "connected": True,
            "heartbeat": True,
            "rssi_dbm": 0,
            "ip": "192.168.77.15",
            "runtime_link": "AX630C UART",
        },
        "events": [
            "UART physical link validated",
            "JSON document received",
            "Runtime model updated",
            "LVGL views refreshed",
        ],
    }


def send_document(payload: dict[str, Any]) -> int:
    """
    Serialize and transmit one newline-delimited JSON document.

    Returns the number of bytes written, including the final LF delimiter.
    """
    document = json.dumps(
        payload,
        ensure_ascii=False,
        separators=(",", ":"),
    )
    encoded_document = (document + "\n").encode("utf-8")

    with UART_DEVICE.open("wb", buffering=0) as uart:
        written = uart.write(encoded_document)

    if written != len(encoded_document):
        raise OSError(
            "Incomplete UART transmission: "
            f"{written}/{len(encoded_document)} bytes"
        )

    return written


def main() -> int:
    """Configure the UART and execute one physical-link validation."""
    try:
        configure_uart()
        payload = build_test_payload()
        written = send_document(payload)

    except FileNotFoundError as error:
        print(f"UART validation failed: {error}", file=sys.stderr)
        return 1

    except subprocess.CalledProcessError as error:
        print(
            "UART validation failed while configuring /dev/ttyS1: "
            f"stty exited with status {error.returncode}",
            file=sys.stderr,
        )
        return 2

    except PermissionError:
        print(
            f"UART validation failed: permission denied for {UART_DEVICE}",
            file=sys.stderr,
        )
        return 3

    except OSError as error:
        print(f"UART validation failed: {error}", file=sys.stderr)
        return 4

    print("Runtime Console UART test packet transmitted.")
    print(f"UART device: {UART_DEVICE}")
    print(f"UART configuration: {UART_BAUD_RATE} bit/s, 8N1")
    print(f"Bytes written: {written}")
    print("Expected CoreS3 state: UART TEST")
    print("Expected CoreS3 user: Claudio")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
