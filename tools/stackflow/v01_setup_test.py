#!/usr/bin/env python3
# v01_setup_test.py
# Ambient Physical AI - AX630C + StackFlow Lab
#
# Purpose:
# Create an LLM session and retrieve the dynamic work_id.
#
# Expected result:
# The runtime returns a work_id such as:
# llm.1000

import socket
import json

HOST = "127.0.0.1"
PORT = 10001
MODEL = "qwen2.5-0.5B-prefill-20e"

def send_json(sock, payload):
    sock.sendall((json.dumps(payload) + "\n").encode("utf-8"))

def recv_json(sock):
    buf = ""
    while "\n" not in buf:
        chunk = sock.recv(4096)
        if not chunk:
            return None
        buf += chunk.decode("utf-8", errors="replace")
    line, _ = buf.split("\n", 1)
    return json.loads(line)

def main():
    setup = {
        "request_id": "setup_001",
        "work_id": "llm",
        "action": "setup",
        "object": "llm.setup",
        "data": {
            "model": MODEL,
            "response_format": "llm.utf-8.stream",
            "input": "llm.utf-8.stream",
            "enoutput": True,
            "max_token_len": 1023,
            "prompt": "You are a helpful assistant."
        }
    }

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(15)

    try:
        s.connect((HOST, PORT))
        send_json(s, setup)
        resp = recv_json(s)
        print("SETUP RESPONSE:")
        print(resp)

        if resp and "work_id" in resp:
            print("SESSION WORK_ID:", resp["work_id"])

            # Close the session if setup succeeded.
            exit_msg = {
                "request_id": "exit_setup_001",
                "work_id": resp["work_id"],
                "action": "exit"
            }
            send_json(s, exit_msg)

    finally:
        s.close()

if __name__ == "__main__":
    main()
