#!/usr/bin/env python3

import socket
import json
from datetime import datetime

from context_builder import build_context, build_human_message
from context_registry import update_context, get_current_context
from stackchan_notifier import StackChanNotifier


UDP_IP = "0.0.0.0"
UDP_PORT = 4444


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

notifier = StackChanNotifier()

print("====================================")
print("AX630C Identity UDP Listener")
print(f"Listening on {UDP_IP}:{UDP_PORT}")
print("Cognitive Context Builder: ENABLED")
print("StackChan Notifier: ENABLED")
print("====================================")

while True:
    data, addr = sock.recvfrom(4096)

    raw = data.decode(errors="replace")

    print("\n------------------------------------")
    print("Packet received")
    print("Sender:", addr)

    try:
        payload = json.loads(raw)

        print("Identity Package received")
        print("Timestamp:", datetime.now().isoformat())
        print("Contract Version:", payload.get("contract_version", "1.0"))
        print("Profile:", payload.get("profile"))
        print(
            "Current Context:",
            payload.get("current_context", payload.get("context")),
        )
        print("Legacy Context:", payload.get("context"))
        print("UID:", payload.get("nfc", {}).get("uid"))
        print("Source:", payload.get("source"))

        
        context = build_context(payload)
        update_context(context)
        current_context = get_current_context()
        message = build_human_message(current_context)

        print("\nContext object generated:")
        print(json.dumps(current_context, ensure_ascii=False, indent=2))

        print("\nHuman-readable message generated:")
        print(message)

        delivered = notifier.notify(message, current_context)

        if delivered:
            print("StackChan notification sent: PASS")
        else:
            print("StackChan notification prepared: PASS")
            print("StackChan reaction observed: PENDING")

    except Exception as e:
        print("JSON parse error or listener error")
        print("Raw:", raw)
        print("Error:", e)
