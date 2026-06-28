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
    print("CONNECTED")
    ws.settimeout(8)

    msg = ws.recv()
    print("RECEIVED INITIAL:")
    print(msg[:1000])

    data = json.loads(msg)

    if data.get("method") == "initialize":
        init_id = data.get("id", 0)

        response = {
            "jsonrpc": "2.0",
            "id": init_id,
            "result": {
                "protocolVersion": data.get("params", {}).get("protocolVersion", "2024-11-05"),
                "capabilities": {
                    "tools": {}
                },
                "serverInfo": {
                    "name": "ambient-physical-ai-mcp-probe",
                    "version": "0.0.1"
                }
            }
        }

        print("SENDING INITIALIZE RESPONSE:")
        print(json.dumps(response, ensure_ascii=False))

        ws.send(json.dumps(response))
        print("INITIALIZE RESPONSE SENT")

        try:
            while True:
                follow = ws.recv()
                print("RECEIVED FOLLOW-UP:")
                print(follow[:2000])
        except Exception as e:
            print("NO MORE MESSAGES OR TIMEOUT:")
            print(type(e).__name__, str(e))

    ws.close()
    print("CLOSED")

except Exception as e:
    print("FAILED:")
    print(type(e).__name__, str(e))
