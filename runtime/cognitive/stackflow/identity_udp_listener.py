#!/usr/bin/env python3
"""
Identity UDP Listener for the StackFlow Cognitive Runtime.

This module is the current entry point for the runtime integration process.

Responsibilities:
    - Receive Identity Packages through UDP.
    - Build and register the current cognitive context.
    - Generate Semantic Events from the registered context.
    - Dispatch Semantic Events to configured output adapters.
    - Publish Cognitive Runtime State to the dedicated state indicator.
    - Start the StackChan MCP transport in the same Python process.

The MCP server intentionally runs in a background thread. Running both
components in the same process allows them to share the process-local
Context Registry implemented by context_registry.py.
"""

import json
import socket
from datetime import datetime
from threading import Thread

from ambient_runtime_notifier import AmbientRuntimeNotifier
from context_builder import build_context, build_human_message
from context_registry import get_current_context, update_context
from rgb_strip_notifier import RGBStripNotifier
from runtime_state_notifier import RuntimeStateNotifier
from semantic_dispatcher import SemanticDispatcher
from semantic_event_generator import generate_semantic_events
from stackchan_mcp_server import run_mcp_server
from stackchan_notifier import StackChanNotifier


UDP_IP = "0.0.0.0"
UDP_PORT = 4444


def start_mcp_server_thread() -> Thread:
    """
    Start the StackChan MCP server in a background daemon thread.

    The MCP transport must execute in this process because the current
    Context Registry is an in-memory, process-local component. Starting the
    MCP server as a separate operating-system process would create an
    independent Registry instance and Semantic Tools would continue returning
    an empty context.

    A daemon thread is appropriate for the current runtime milestone because
    its lifetime is bound to the main Identity UDP Listener process.
    """

    thread = Thread(
        target=run_mcp_server,
        name="stackchan-mcp-server",
        daemon=True,
    )
    thread.start()

    return thread


def notify_runtime_state(state: str) -> bool:
    """
    Publish a Runtime State without interrupting the semantic pipeline.

    Runtime-state indication is observational. A temporary failure in the
    StickC node, its network path, or its notifier must never prevent context
    construction or Semantic Event delivery to the remaining system nodes.

    Returns:
        True when the notifier reports successful UDP transmission.
        False when notification fails or raises an exception.
    """

    try:
        return runtime_state_notifier.notify(state)
    except Exception as exc:
        print(
            f"Runtime State notification failed [{state}]: {exc}"
        )
        return False


# Start the MCP transport before entering the blocking UDP receive loop.
# The STACKCHAN_MCP_URL environment variable must be configured in the shell
# used to start this process.
mcp_thread = start_mcp_server_thread()


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))


notifier = StackChanNotifier()
rgb_strip_notifier = RGBStripNotifier()
ambient_runtime_notifier = AmbientRuntimeNotifier()
runtime_state_notifier = RuntimeStateNotifier()

dispatcher = SemanticDispatcher()
dispatcher.register_adapter("stackchan", notifier.notify)
dispatcher.register_adapter("rgb_strip", rgb_strip_notifier.notify)
dispatcher.register_adapter(
    "ambient_runtime",
    ambient_runtime_notifier.notify,
)


print("====================================")
print("AX630C Identity UDP Listener")
print(f"Listening on {UDP_IP}:{UDP_PORT}")
print("Cognitive Context Builder: ENABLED")
print("Semantic Event Generator: ENABLED")
print("Semantic Dispatcher: ENABLED")
print("StackChan Notifier: ENABLED")
print("RGB Strip Notifier: ENABLED")
print("Ambient Runtime Notifier: ENABLED")
print("Runtime State Notifier: ENABLED")
print("StackChan MCP Server: STARTING")
print("Shared Context Registry: ENABLED")
print("====================================")

notify_runtime_state("idle")


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

        # The current StickC firmware supports "thinking" but does not yet
        # define a separate "processing_context" state. Context construction
        # and Semantic Event generation are therefore represented as thinking.
        notify_runtime_state("thinking")

        # The Context Builder normalizes the Identity Package into the
        # canonical context representation consumed by semantic services.
        context = build_context(payload)

        # update_context() replaces the process-local current context. The MCP
        # background thread reads this same Registry instance when a Tool is
        # invoked by the XiaoZhi broker.
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

            if dispatch_results.get("rgb_strip"):
                print("RGB Strip semantic event sent: PASS")
            else:
                print("RGB Strip semantic event sent: FAIL")

            if dispatch_results.get("ambient_runtime"):
                print("Ambient Runtime semantic event sent: PASS")
            else:
                print("Ambient Runtime semantic event sent: FAIL")

        # Responding represents the completed handoff of the generated
        # Semantic Events to all currently registered output adapters.
        notify_runtime_state("responding")

    except Exception as exc:
        notify_runtime_state("error")
        print("JSON parse error or listener error")
        print("Raw:", raw)
        print("Error:", exc)

    finally:
        # Every packet-processing cycle returns the indicator to idle,
        # regardless of success or failure.
        notify_runtime_state("idle")
