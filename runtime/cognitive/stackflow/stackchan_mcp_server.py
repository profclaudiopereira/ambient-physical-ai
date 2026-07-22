#!/usr/bin/env python3
"""
StackChan MCP WebSocket Server for Ambient Physical AI.

This module implements the MCP transport adapter between the XiaoZhi MCP
Broker and the semantic tools exposed by the StackFlow Cognitive Runtime.

Responsibilities:
    - Establish the authenticated WebSocket connection.
    - Handle the MCP initialization handshake.
    - Respond to broker keepalive requests.
    - Publish the available Semantic MCP Tools.
    - Forward Tool calls to semantic_mcp_tools.

Semantic business logic must not be implemented in this module. The server
only translates MCP messages to calls handled by the semantic tool layer.

Security:
    The authenticated MCP URL must be provided through the
    STACKCHAN_MCP_URL environment variable. The credential is never stored
    directly in source code and is redacted before being written to logs.
"""

import json
import os
from typing import Any
from urllib.parse import parse_qsl, urlencode, urlsplit, urlunsplit

import websocket

from semantic_mcp_tools import call_tool, list_tools


DEFAULT_PROTOCOL_VERSION = "2024-11-05"
DEFAULT_CONNECTION_TIMEOUT_SECONDS = 10
DEFAULT_RECEIVE_TIMEOUT_SECONDS = 120


def redact_url(url: str) -> str:
    """
    Return a log-safe MCP URL.

    Authentication tokens contained in the query string are replaced before
    the URL is printed. The original URL remains unchanged for the connection.
    """
    parts = urlsplit(url)
    query = parse_qsl(parts.query, keep_blank_values=True)

    redacted_query = [
        (key, "<REDACTED>" if key.lower() == "token" else value)
        for key, value in query
    ]

    return urlunsplit(
        (
            parts.scheme,
            parts.netloc,
            parts.path,
            urlencode(redacted_query),
            parts.fragment,
        )
    )


def build_success_response(message_id: Any, result: dict) -> dict:
    """Build a successful JSON-RPC 2.0 response."""

    return {
        "jsonrpc": "2.0",
        "id": message_id,
        "result": result,
    }


def build_error_response(
    message_id: Any,
    code: int,
    message: str,
) -> dict:
    """Build a JSON-RPC 2.0 error response."""

    return {
        "jsonrpc": "2.0",
        "id": message_id,
        "error": {
            "code": code,
            "message": message,
        },
    }


class StackChanMCPServer:
    """
    Maintain the MCP WebSocket session with the XiaoZhi broker.

    The server is intentionally transport-focused. Tool definitions and Tool
    execution are delegated to semantic_mcp_tools.
    """

    def __init__(self, url: str):
        self.url = url
        self.ws = None

    def connect(self) -> None:
        """Open the authenticated WebSocket connection."""

        print("Connecting to StackChan MCP broker:")
        print(redact_url(self.url))

        self.ws = websocket.create_connection(
            self.url,
            timeout=DEFAULT_CONNECTION_TIMEOUT_SECONDS,
        )
        self.ws.settimeout(DEFAULT_RECEIVE_TIMEOUT_SECONDS)

        print("StackChan MCP connection established.")

    def send(self, payload: dict) -> None:
        """Serialize and send one JSON-RPC message."""

        if self.ws is None:
            raise RuntimeError("MCP WebSocket connection is not available")

        self.ws.send(json.dumps(payload, ensure_ascii=False))

    def handle_initialize(self, data: dict) -> None:
        """Respond to the MCP initialization request."""

        message_id = data.get("id")
        requested_version = data.get("params", {}).get(
            "protocolVersion",
            DEFAULT_PROTOCOL_VERSION,
        )

        response = build_success_response(
            message_id,
            {
                "protocolVersion": requested_version,
                "capabilities": {
                    "tools": {},
                },
                "serverInfo": {
                    "name": "ambient-physical-ai-stackflow-mcp-server",
                    "version": "0.1.0",
                },
            },
        )

        self.send(response)
        print("MCP initialize response sent.")

    def handle_tools_list(self, data: dict) -> None:
        """Publish the Semantic MCP Tools exposed by the runtime."""

        message_id = data.get("id")
        tools = list_tools()

        response = build_success_response(
            message_id,
            {
                "tools": tools,
            },
        )

        self.send(response)
        print(f"MCP tools/list response sent with {len(tools)} Tool(s).")

    def handle_tools_call(self, data: dict) -> None:
        """Forward an MCP Tool invocation to the semantic tool layer."""

        message_id = data.get("id")
        params = data.get("params", {})

        tool_name = params.get("name")
        arguments = params.get("arguments", {})

        if not tool_name:
            self.send(
                build_error_response(
                    message_id,
                    -32602,
                    "Tool name is required",
                )
            )
            return

        print(f"MCP Tool requested: {tool_name}")

        try:
            result = call_tool(tool_name, arguments)

            # Preserve the response format already validated in the MCP
            # laboratory against the XiaoZhi broker.
            if isinstance(result, dict):
                result.setdefault("isError", False)

            self.send(build_success_response(message_id, result))
            print(f"MCP Tool result sent: {tool_name}")

        except KeyError:
            self.send(
                build_error_response(
                    message_id,
                    -32601,
                    f"Unknown Tool: {tool_name}",
                )
            )

        except Exception as exc:
            print(f"MCP Tool execution failed: {type(exc).__name__}: {exc}")

            self.send(
                build_error_response(
                    message_id,
                    -32603,
                    "Internal Tool execution error",
                )
            )

    def handle_ping(self, data: dict) -> None:
        """Respond to an MCP keepalive request."""

        self.send(build_success_response(data.get("id"), {}))
        print("MCP ping response sent.")

    def handle_message(self, raw_message: str) -> None:
        """Parse and route one message received from the MCP broker."""

        data = json.loads(raw_message)
        method = data.get("method")

        print(f"MCP message received: {method or 'response/unknown'}")

        if method == "initialize":
            self.handle_initialize(data)

        elif method == "notifications/initialized":
            print("MCP initialized notification received.")

        elif method == "ping":
            self.handle_ping(data)

        elif method == "tools/list":
            self.handle_tools_list(data)

        elif method == "tools/call":
            self.handle_tools_call(data)

        else:
            print(f"MCP message not handled: {method}")

    def run(self) -> None:
        """Run the blocking MCP receive loop."""

        self.connect()

        try:
            while True:
                raw_message = self.ws.recv()

                if not raw_message:
                    raise ConnectionError(
                        "MCP broker closed the WebSocket connection"
                    )

                self.handle_message(raw_message)

        except KeyboardInterrupt:
            print("StackChan MCP server stopped by user.")

        finally:
            self.close()

    def close(self) -> None:
        """Close the WebSocket session when available."""

        if self.ws is not None:
            try:
                self.ws.close()
            finally:
                self.ws = None

        print("StackChan MCP connection closed.")


def run_mcp_server() -> None:
    """
    Start the StackChan MCP server.

    This function will later be reused by the Cognitive Runtime when the server
    is started in a background thread alongside the Identity UDP listener.
    """

    url = os.getenv("STACKCHAN_MCP_URL")

    if not url:
        raise SystemExit(
            "ERROR: STACKCHAN_MCP_URL environment variable is not configured"
        )

    server = StackChanMCPServer(url)
    server.run()


if __name__ == "__main__":
    run_mcp_server()