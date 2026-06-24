import os
import json
import websocket
from urllib.parse import urlsplit, urlunsplit, parse_qsl, urlencode

def redact_url(url: str) -> str:
    parts = urlsplit(url)
    query = parse_qsl(parts.query, keep_blank_values=True)
    redacted = [(k, "<REDACTED>" if k.lower() == "token" else v) for k, v in query]
    return urlunsplit((parts.scheme, parts.netloc, parts.path, urlencode(redacted), parts.fragment))

url = os.getenv("STACKCHAN_MCP_URL")
if not url:
    raise SystemExit("ERROR: STACKCHAN_MCP_URL not set")

print("Connecting to:")
print(redact_url(url))

try:
    ws = websocket.create_connection(url, timeout=10)
    ws.settimeout(60)
    print("CONNECTED")

    while True:
        msg = ws.recv()
        print("RECEIVED:")
        print(msg[:3000])

        data = json.loads(msg)
        method = data.get("method")
        msg_id = data.get("id")

        if method == "initialize":
            ws.send(json.dumps({
                "jsonrpc": "2.0",
                "id": msg_id,
                "result": {
                    "protocolVersion": data.get("params", {}).get("protocolVersion", "2024-11-05"),
                    "capabilities": {"tools": {}},
                    "serverInfo": {
                        "name": "ambient-physical-ai-mcp-dummy-tool-ping-probe",
                        "version": "0.0.1"
                    }
                }
            }))
            print("SENT initialize response")

        elif method == "tools/list":
            ws.send(json.dumps({
                "jsonrpc": "2.0",
                "id": msg_id,
                "result": {
                    "tools": [
                        {
                            "name": "AmbientPing",
                            "description": "Safe dummy probe tool for Ambient Physical AI MCP discovery. Returns pong only.",
                            "inputSchema": {
                                "type": "object",
                                "properties": {},
                                "required": []
                            }
                        }
                    ]
                }
            }))
            print("SENT tools/list response with AmbientPing")

        elif method == "ping":
            ws.send(json.dumps({
                "jsonrpc": "2.0",
                "id": msg_id,
                "result": {}
            }))
            print("SENT ping response")

        elif method == "tools/call":
            params = data.get("params", {})
            tool_name = params.get("name")
            print("TOOL CALL REQUESTED:", tool_name)

            if tool_name == "AmbientPing":
                ws.send(json.dumps({
                    "jsonrpc": "2.0",
                    "id": msg_id,
                    "result": {
                        "content": [{"type": "text", "text": "pong"}],
                        "isError": False
                    }
                }))
                print("SENT AmbientPing result")
            else:
                ws.send(json.dumps({
                    "jsonrpc": "2.0",
                    "id": msg_id,
                    "error": {"code": -32601, "message": "Unknown tool"}
                }))

        elif method == "notifications/initialized":
            print("INITIALIZED notification received")

        else:
            print("UNHANDLED MESSAGE")

except KeyboardInterrupt:
    print("STOPPED BY USER")
except Exception as e:
    print("STOPPED:")
    print(type(e).__name__, str(e))

try:
    ws.close()
    print("CLOSED")
except Exception:
    pass
