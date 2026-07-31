#!/usr/bin/env python3
"""
Identity and Context-Change UDP Listener for the StackFlow Cognitive Runtime.

This module is the current entry point for runtime integration.

Responsibilities:
    - Receive Identity Packages and authorized Context Change Requests by UDP.
    - Build and register the current cognitive context.
    - Preserve the active identity during voice-driven context changes.
    - Generate and dispatch normalized Semantic Event V1 objects.
    - Refresh Ambient Context information on the Tab5.
    - Publish Cognitive Runtime State to the dedicated state indicator.
    - Start the StackChan MCP transport in the same Python process.

The MCP server intentionally runs in a background thread. Running both
components in the same process allows them to share the process-local Context
Registry implemented by context_registry.py.
"""

import json
import socket
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from threading import Thread

from ambient_runtime_notifier import AmbientRuntimeNotifier
from context_builder import build_context, build_human_message
from context_registry import get_current_context, update_context
from echo_pyramid_adapter import EchoPyramidAdapter
from rgb_strip_notifier import RGBStripNotifier
from runtime_state_notifier import RuntimeStateNotifier
from semantic_dispatcher import SemanticDispatcher
from semantic_event_generator import (
    generate_context_changed_event,
    generate_semantic_events,
)
from semantic_services import build_identity_voice_message
from services.ambient_context.ambient_context_service import (
    send_ambient_context,
)
from stackchan_mcp_server import run_mcp_server
from stackchan_notifier import StackChanNotifier


UDP_IP = "0.0.0.0"
UDP_PORT = 4444

# These names are the canonical environment values already used by M5Dial,
# Echo Pyramid MultiNet commands and the Cognitive Runtime.
VALID_CONTEXTS = {
    "Research",
    "Lab",
    "Meeting",
    "Classroom",
    "Demo",
}


def start_mcp_server_thread() -> Thread:
    """Start the StackChan MCP server in a background daemon thread."""
    thread = Thread(
        target=run_mcp_server,
        name="stackchan-mcp-server",
        daemon=True,
    )
    thread.start()
    return thread


def notify_runtime_state(state: str) -> bool:
    """
    Publish Runtime State without interrupting the semantic pipeline.

    Runtime-state indication is observational. A temporary failure in the
    StickC node or network path must not prevent Context or Semantic Event
    processing.
    """
    try:
        return runtime_state_notifier.notify(state)
    except Exception as exc:
        print(f"Runtime State notification failed [{state}]: {exc}")
        return False


def refresh_ambient_context(current_context: dict) -> None:
    """
    Refresh profile-aware external information on the Tab5.

    This service is separate from Semantic Event delivery. A failure here is
    reported but does not block context registration or event dispatch.
    """
    try:
        profile_id = current_context.get("who", {}).get("id", "unknown")

        send_ambient_context(
            profile_id=profile_id,
            tab5_host="192.168.77.203",
        )

        print(f"Ambient Context updated for '{profile_id}'")

    except Exception as exc:
        print(f"Ambient Context update failed: {exc}")


def print_dispatch_results(dispatch_results: dict) -> None:
    """Print one consistent delivery summary for every Semantic Event."""
    print("\nSemantic Event dispatch results:")
    print(json.dumps(dispatch_results, ensure_ascii=False, indent=2))

    if dispatch_results.get("stackchan"):
        print("StackChan semantic event sent/prepared: PASS")
    else:
        print("StackChan semantic event sent/prepared: FAIL")

    if dispatch_results.get("rgb_strip"):
        print("RGB expression semantic event sent: PASS")
    else:
        print("RGB expression semantic event sent: FAIL")

    if dispatch_results.get("ambient_runtime"):
        print("Ambient Runtime semantic event sent: PASS")
    else:
        print("Ambient Runtime semantic event sent: FAIL")

    if dispatch_results.get("echo_pyramid"):
        print("Echo Pyramid semantic event handled: PASS")
    else:
        print("Echo Pyramid semantic event handled: FAIL")


def handle_context_change_request(payload: dict) -> None:
    """
    Validate and apply one voice-originated environment-context change.

    The active identity, NFC UID, role and authorization remain unchanged.
    Only the environment and context-change metadata are updated. After the
    Registry update, a dedicated context_changed Semantic Event is dispatched
    so downstream nodes can react without replaying identity authentication.
    """
    requested_context = payload.get("requested_context")
    request_source = payload.get("source", "unknown")

    print("Context Change Request received")
    print("Timestamp:", datetime.now().isoformat())
    print("Requested Context:", requested_context)
    print("Source:", request_source)

    if requested_context not in VALID_CONTEXTS:
        raise ValueError(
            "invalid requested_context; expected one of "
            + ", ".join(sorted(VALID_CONTEXTS))
        )

    current_context = get_current_context()
    if current_context is None:
        raise ValueError(
            "context change rejected: no authenticated identity is active"
        )

    previous_context = (
        current_context.get("where", {}).get("environment", "Unknown")
    )

    current_context.setdefault("where", {})
    current_context["where"]["environment"] = requested_context
    current_context["where"]["location_source"] = request_source

    current_context.setdefault("what", {})
    current_context["what"]["activity"] = "context_change"
    current_context["what"]["state"] = "validated"
    current_context["what"]["event_type"] = "context_change_request"

    current_context.setdefault("why", {})
    current_context["why"]["intent"] = "change_environment_context"
    current_context["why"]["reasoning"] = (
        f"Context changed from {previous_context} to "
        f"{requested_context} by {request_source}"
    )
    current_context["why"]["confidence"] = 1.0

    current_context.setdefault("source", {})
    current_context["source"]["context_change_source"] = request_source

    update_context(current_context)
    updated_context = get_current_context()

    print(f"Context updated: {previous_context} -> {requested_context}")
    print("Updated Runtime Context:")
    print(json.dumps(updated_context, ensure_ascii=False, indent=2))

    # Preserve the already validated profile-aware Tab5 refresh path.
    refresh_ambient_context(updated_context)

    # Generate a dedicated event instead of reusing identity_authenticated.
    # This prevents a second welcome message and keeps identity semantics clean.
    context_event = generate_context_changed_event(
        context_package=updated_context,
        previous_context=previous_context,
        requested_context=requested_context,
        request_source=request_source,
    )

    print("\nContext Changed Semantic Event generated:")
    print(json.dumps(context_event, ensure_ascii=False, indent=2))

    dispatch_results = dispatcher.dispatch(context_event)
    print_dispatch_results(dispatch_results)


# The MCP transport must share the same in-memory Registry as this listener.
mcp_thread = start_mcp_server_thread()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

notifier = StackChanNotifier()
rgb_strip_notifier = RGBStripNotifier()
ambient_runtime_notifier = AmbientRuntimeNotifier()
echo_pyramid_adapter = EchoPyramidAdapter()
runtime_state_notifier = RuntimeStateNotifier()

dispatcher = SemanticDispatcher()
dispatcher.register_adapter("stackchan", notifier.notify)
dispatcher.register_adapter("rgb_strip", rgb_strip_notifier.notify)
dispatcher.register_adapter(
    "ambient_runtime",
    ambient_runtime_notifier.notify,
)
dispatcher.register_adapter(
    "echo_pyramid",
    echo_pyramid_adapter.notify,
)

print("====================================")
print("AX630C Identity and Context UDP Listener")
print(f"Listening on {UDP_IP}:{UDP_PORT}")
print("Identity Package ingress: ENABLED")
print("Context Change Request ingress: ENABLED")
print("Cognitive Context Builder: ENABLED")
print("Semantic Event Generator: ENABLED")
print("Semantic Dispatcher: ENABLED")
print("StackChan Notifier: ENABLED")
print("RGB Expression Notifier: ENABLED")
print("Ambient Runtime Notifier: ENABLED")
print("Echo Pyramid Adapter: ENABLED")
print(
    "Echo Pyramid UDP Target: "
    f"{echo_pyramid_adapter.host}:{echo_pyramid_adapter.port}"
)
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
        packet_type = payload.get("type")

        if packet_type == "context_change_request":
            notify_runtime_state("thinking")
            handle_context_change_request(payload)
            notify_runtime_state("responding")
            continue

        if packet_type != "identity_package":
            raise ValueError(
                f"unsupported UDP packet type: {packet_type!r}"
            )

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

        notify_runtime_state("thinking")

        # Normalize the Identity Package and replace the active Registry state.
        context = build_context(payload)
        update_context(context)
        current_context = get_current_context()

        refresh_ambient_context(current_context)

        # Identity voice runs only for Identity Packages. Context changes never
        # execute this block and therefore never replay the welcome greeting.
        try:
            identity = current_context.get("who")
            voice_message = build_identity_voice_message(identity)

            print("\nIdentity voice message selected:")
            print(voice_message)

            tts_client_path = (
                Path(__file__).resolve().parent
                / "stackflow_tts_to_echo_pyramid.py"
            )

            if not tts_client_path.is_file():
                raise FileNotFoundError(
                    f"StackFlow TTS client not found: {tts_client_path}"
                )

            completed = subprocess.run(
                [
                    sys.executable,
                    str(tts_client_path),
                    "--text",
                    voice_message,
                ],
                check=False,
            )

            if completed.returncode == 0:
                print("Echo Pyramid personalized voice: PASS")
            else:
                print(
                    "Echo Pyramid personalized voice: FAIL "
                    f"(TTS client exit code {completed.returncode})"
                )

        except Exception as exc:
            print(f"Echo Pyramid personalized voice failed: {exc}")

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
            print_dispatch_results(dispatch_results)

        notify_runtime_state("responding")

    except Exception as exc:
        notify_runtime_state("error")
        print("JSON parse error or listener error")
        print("Raw:", raw)
        print("Error:", exc)

    finally:
        # Every processing cycle returns the explicit cognitive indicator to
        # idle, including the context-change branch that uses continue above.
        notify_runtime_state("idle")
