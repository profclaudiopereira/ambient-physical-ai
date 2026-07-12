#!/usr/bin/env python3

import socket
import json
from datetime import datetime

UDP_IP = "0.0.0.0"
UDP_PORT = 4444

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print("====================================")
print("AX630C Identity UDP Listener")
print(f"Listening on {UDP_IP}:{UDP_PORT}")
print("====================================")

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
        print("Profile:", payload.get("profile"))
        print("Context:", payload.get("context"))
        print("UID:", payload.get("nfc", {}).get("uid"))
        print("Source:", payload.get("source"))

    except Exception as e:
        print("JSON parse error")
        print("Raw:", raw)
        print("Error:", e)