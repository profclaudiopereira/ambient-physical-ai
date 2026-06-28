import os
import websocket
from urllib.parse import urlsplit, urlunsplit, parse_qsl, urlencode

def redact_url(url: str) -> str:
    parts = urlsplit(url)
    query = parse_qsl(parts.query, keep_blank_values=True)
    redacted = []
    for k, v in query:
        redacted.append((k, "<REDACTED>" if k.lower() == "token" else v))
    return urlunsplit((
        parts.scheme,
        parts.netloc,
        parts.path,
        urlencode(redacted),
        parts.fragment
    ))

url = os.getenv("STACKCHAN_MCP_URL")

if not url:
    raise SystemExit("ERROR: STACKCHAN_MCP_URL not set")

print("Connecting to:")
print(redact_url(url))

try:
    ws = websocket.create_connection(url, timeout=10)
    print("CONNECTED")

    ws.settimeout(5)

    try:
        msg = ws.recv()
        print("RECEIVED:")
        print(msg[:1000])
    except Exception as e:
        print("NO INITIAL MESSAGE OR TIMEOUT:")
        print(type(e).__name__, str(e))

    ws.close()
    print("CLOSED")

except Exception as e:
    print("CONNECTION FAILED:")
    print(type(e).__name__, str(e))
