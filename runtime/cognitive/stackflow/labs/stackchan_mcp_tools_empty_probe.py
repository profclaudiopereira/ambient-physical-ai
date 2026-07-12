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
    ws.settimeout(10)
    print("CONNECTED")

    while True:
        msg = ws.recv()
        print("RECEIVED:")
        print(msg[:2000])

        data = json.loads(msg)
        method = data.get("method")
        msg_id = data.get("id")

        if method == "initialize":
            response = {
                "jsonrpc": "2.0",
                "id": msg_id,
                "result": {
                    "protocolVersion": data.get("params", {}).get("protocolVersion", "2024-11-05"),
                    "capabilities": {
                        "tools": {}
                    },
                    "serverInfo": {
                        "name": "ambient-physical-ai-mcp-empty-tools-probe",
                        "version": "0.0.1"
                    }
                }
            }
            print("SENDING initialize response")
            ws.send(json.dumps(response))

        elif method == "tools/list":
            response = {
                "jsonrpc": "2.0",
                "id": msg_id,
                "result": {
                    "tools": []
                }
            }
            print("SENDING empty tools/list response")
            ws.send(json.dumps(response))

        else:
            print("UNHANDLED MESSAGE")

except Exception as e:
    print("STOPPED:")
    print(type(e).__name__, str(e))

try:
    ws.close()
    print("CLOSED")
except Exception:
    pass
