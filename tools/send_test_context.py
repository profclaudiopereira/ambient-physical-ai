import json
import socket
import sys
import time

PORT = 5555

if len(sys.argv) < 3:
    print("Usage: python send_test_context.py <TAB5_IP> <profile>")
    sys.exit(1)

tab5_ip = sys.argv[1]
profile = sys.argv[2]

payload = {
    "type": "ambient_context",
    "authenticated": True,
    "profile_id": profile,
    "ttl_seconds": 900,
    "sequence": int(time.time()),
    "global": {
        "available": True,
        "location": "Recife",
        "weather_summary": "Cloudy",
        "temperature_c": 28.0,
        "uv_index": 6.2,
        "uv_label": "High"
    },
    "personal": {
        "available": True,
        "title": "USD/BRL",
        "value": "5.42",
        "secondary": "Test"
    }
}

data = json.dumps(payload).encode("utf-8")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# connect() em UDP não cria conexão real.
# Apenas força o Windows a escolher a interface e o IP local.
sock.connect((tab5_ip, PORT))

print(f"Local UDP endpoint: {sock.getsockname()}")
print(f"Remote UDP endpoint: {sock.getpeername()}")

sent = sock.send(data)

print(f"Sent {sent} bytes to {tab5_ip}:{PORT} for profile={profile}")

sock.close()