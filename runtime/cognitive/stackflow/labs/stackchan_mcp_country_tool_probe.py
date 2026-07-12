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
    ws.settimeout(120)
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
                        "name": "ambient-physical-ai-country-tool-probe",
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
                            "name": "GetCurrentCountry",
                            "description": "Returns the country where the Ambient Physical AI runtime is currently deployed. Use this tool when the user asks which country the current Ambient Physical AI environment is located in.",
                            "inputSchema": {
                                "type": "object",
                                "properties": {},
                                "required": []
                            }
                        }
                    ]
                }
            }))
            print("SENT tools/list response with GetCurrentCountry")

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

            if tool_name == "GetCurrentCountry":
                ws.send(json.dumps({
                    "jsonrpc": "2.0",
                    "id": msg_id,
                    "result": {
                        "content": [
                            {
                                "type": "text",
                                "text": "Brazil"
                            }
                        ],
                        "isError": False
                    }
                }))
                print("SENT GetCurrentCountry result: Brazil")
            else:
                ws.send(json.dumps({
                    "jsonrpc": "2.0",
                    "id": msg_id,
                    "error": {
                        "code": -32601,
                        "message": "Unknown tool"
                    }
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
