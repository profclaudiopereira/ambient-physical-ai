#!/usr/bin/env python3
# v04_context_injection.py
# Ambient Physical AI - AX630C + StackFlow Lab
#
# Purpose:
# Validate one-shot context injection.
#
# This is the recommended safe pattern discovered:
# setup -> one prompt with full context -> streaming response -> exit
#
# Lab result:
# Prompt:
# Context: My name is Claudio. Question: What is my name?
#
# Observed response:
# My name is Claudio.

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
    prompt = "Context: My name is Claudio. Question: What is my name?"

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(60)
    s.connect((HOST, PORT))

    try:
        setup = {
            "request_id": "setup_one",
            "work_id": "llm",
            "action": "setup",
            "object": "llm.setup",
            "data": {
                "model": MODEL,
                "response_format": "llm.utf-8.stream",
                "input": "llm.utf-8.stream",
                "enoutput": True,
                "max_token_len": 1023,
                "prompt": "You are a helpful assistant. Answer briefly."
            }
        }

        send_json(s, setup)
        setup_resp = recv_json(s)
        print("SETUP:", setup_resp)

        work_id = setup_resp["work_id"]

        inference = {
            "request_id": "infer_one",
            "work_id": work_id,
            "action": "inference",
            "object": "llm.utf-8.stream",
            "data": {
                "delta": prompt,
                "index": 0,
                "finish": True
            }
        }

        send_json(s, inference)

        print("LLM:", end=" ", flush=True)

        while True:
            resp = recv_json(s)
            if resp is None:
                break

            data = resp.get("data", {})
            print(data.get("delta", ""), end="", flush=True)

            if data.get("finish", False):
                break

        print()

        exit_msg = {
            "request_id": "exit_one",
            "work_id": work_id,
            "action": "exit"
        }

        send_json(s, exit_msg)

    finally:
        s.close()

if __name__ == "__main__":
    main()
