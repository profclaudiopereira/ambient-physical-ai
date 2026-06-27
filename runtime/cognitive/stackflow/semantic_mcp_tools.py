#!/usr/bin/env python3
"""
Semantic MCP Tool Layer for Ambient Physical AI.

Maps local semantic services to MCP-style tool definitions and calls.
This module does not implement WebSocket transport yet.
"""

import json

from semantic_services import (
    get_runtime_status,
    get_current_identity,
    get_current_environment,
    get_current_activity,
    get_current_context,
)


def list_tools():
    return [
        {
            "name": "get_runtime_status",
            "description": "Return Cognitive Runtime status.",
            "inputSchema": {"type": "object", "properties": {}},
        },
        {
            "name": "get_current_identity",
            "description": "Return the current identified user.",
            "inputSchema": {"type": "object", "properties": {}},
        },
        {
            "name": "get_current_environment",
            "description": "Return the current environment context.",
            "inputSchema": {"type": "object", "properties": {}},
        },
        {
            "name": "get_current_activity",
            "description": "Return the current activity state.",
            "inputSchema": {"type": "object", "properties": {}},
        },
        {
            "name": "get_current_context",
            "description": "Return the full current 5W Context Package.",
            "inputSchema": {"type": "object", "properties": {}},
        },
    ]


def call_tool(name, arguments=None):
    if name == "get_runtime_status":
        result = get_runtime_status()
    elif name == "get_current_identity":
        result = get_current_identity()
    elif name == "get_current_environment":
        result = get_current_environment()
    elif name == "get_current_activity":
        result = get_current_activity()
    elif name == "get_current_context":
        result = get_current_context()
    else:
        raise ValueError(f"Unknown semantic MCP tool: {name}")

    return {
        "content": [
            {
                "type": "text",
                "text": json.dumps(result, ensure_ascii=False, indent=2),
            }
        ]
    }