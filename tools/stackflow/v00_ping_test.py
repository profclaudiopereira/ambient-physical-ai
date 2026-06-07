#!/usr/bin/env python3
# v00_ping_test.py
# Ambient Physical AI - AX630C + StackFlow Lab
#
# Purpose:
# Validate that the StackFlow runtime is reachable through TCP port 10001.
#
# Validated behavior:
# work_id=sys
# action=ping
# Expected result:
# error.code == 0

import socket
import json

HOST = "127.0.0.1"
PORT = 10001

def main():
    msg = {
        "request_id": "safe_ping_001",
        "work_id": "sys",
        "action": "ping"
    }

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5)

    try:
        s.connect((HOST, PORT))
        s.sendall((json.dumps(msg) + "\n").encode("utf-8"))
        data = s.recv(4096)
        print(data.decode("utf-8", errors="replace"))
    finally:
        s.close()

if __name__ == "__main__":
    main()
