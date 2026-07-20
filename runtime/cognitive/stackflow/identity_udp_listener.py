#!/usr/bin/env python3

import json
import socket
from datetime import datetime

from context_builder import build_context, build_human_message
from context_registry import get_current_context, update_context
from semantic_dispatcher import SemanticDispatcher
from semantic_event_generator import generate_semantic_events
from stackchan_notifier import StackChanNotifier


UDP_IP = "0.0.0.0"
UDP_PORT = 4444


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

notifier = StackChanNotifier()

dispatcher = SemanticDispatcher()
dispatcher.register_adapter("stackchan", notifier.notify)

print("====================================")
print("AX630C Identity UDP Listener")
print(f"Listening on {UDP_IP}:{UDP_PORT}")
print("Cognitive Context Builder: ENABLED")
print("Semantic Event Generator: ENABLED")
print("Semantic Dispatcher: ENABLED")
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
        semantic_events = generate_semantic_events(current_context)

        print("\nContext object generated:")
        print(json.dumps(current_context, ensure_ascii=False, indent=2))

        print("\nHuman-readable message generated:")
        print(message)

        print("\nSemantic Events generated:")
        print(json.dumps(semantic_events, ensure_ascii=False, indent=2))

        for semantic_event in semantic_events:
            dispatch_results = dispatcher.dispatch(semantic_event)

            print("\nSemantic Event dispatch results:")
            print(json.dumps(dispatch_results, ensure_ascii=False, indent=2))

            if dispatch_results.get("stackchan"):
                print("StackChan notification sent: PASS")
            else:
                print("StackChan notification prepared: PASS")
                print("StackChan reaction observed: PENDING")

    except Exception as exc:
        print("JSON parse error or listener error")
        print("Raw:", raw)
        print("Error:", exc)
