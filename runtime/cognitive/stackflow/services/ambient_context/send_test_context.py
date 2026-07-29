#!/usr/bin/env python3
"""Sends a deterministic ambient_context payload without calling any API."""

import argparse
import json
import socket


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("host", help="Tab5 IPv4 address")
    parser.add_argument("--port", type=int, default=5555)
    parser.add_argument(
        "--profile",
        choices=["claudio", "herminio", "mariana", "student"],
        default="claudio",
    )
    args = parser.parse_args()

    personal = {
        "claudio": {
            "available": True,
            "title": "USD BRL",
            "value": "R$ 5.42",
            "secondary": "+0.31%",
        },
        "herminio": {
            "available": True,
            "title": "SPORT",
            "value": "SPORT x CEARA",
            "secondary": "2026-07-30 20:30",
        },
        "mariana": {
            "available": True,
            "title": "CINEMA BR",
            "value": "FILME EM CARTAZ",
            "secondary": "TMDB 8.1",
        },
        "student": {
            "available": True,
            "title": "STEAM DEAL",
            "value": "GAME TITLE",
            "secondary": "-60% R$ 29.90",
        },
    }[args.profile]

    payload = {
        "type": "ambient_context",
        "sequence": 1,
        "ttl_seconds": 900,
        "authenticated": True,
        "profile_id": args.profile,
        "global": {
            "available": True,
            "location": "RECIFE",
            "weather_summary": "PARTLY CLOUDY",
            "temperature_c": 28.4,
            "uv_index": 7.1,
            "uv_label": "HIGH",
        },
        "personal": personal,
    }

    encoded = json.dumps(payload, separators=(",", ":")).encode("utf-8")
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sent = sock.sendto(encoded, (args.host, args.port))

    print(json.dumps(payload, indent=2))
    print(f"\nSent {sent} bytes to {args.host}:{args.port}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
