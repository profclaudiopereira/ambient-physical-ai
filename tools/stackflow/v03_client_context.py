#!/usr/bin/env python3
# v03_client_context.py
# Ambient Physical AI - AX630C + StackFlow Lab
#
# Purpose:
# Investigate session behavior, multi-turn behavior, context injection and event prompts.
#
# Important:
# During the lab, repeated/multi-turn tests caused timeouts.
# Use carefully.
#
# Recommended usage:
# Run one test at a time if the runtime becomes unstable.
#
# Recovery if runtime stops responding:
# systemctl restart llm-llm
# systemctl restart llm-sys

import socket
import json
import time

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

def setup_session(sock, name):
    setup = {
        "request_id": "setup_" + name,
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

    send_json(sock, setup)
    resp = recv_json(sock)
    print("")
    print("SETUP RESPONSE")
    print(resp)
    return resp["work_id"]

def infer(sock, work_id, request_id, prompt):
    msg = {
        "request_id": request_id,
        "work_id": work_id,
        "action": "inference",
        "object": "llm.utf-8.stream",
        "data": {
            "delta": prompt,
            "index": 0,
            "finish": True
        }
    }

    print("")
    print("USER:")
    print(prompt)
    print("")
    print("LLM:")
    print("--------------------")

    send_json(sock, msg)

    full_response = ""

    while True:
        resp = recv_json(sock)
        if resp is None:
            break

        data = resp.get("data", {})
        delta = data.get("delta", "")
        finish = data.get("finish", False)

        print(delta, end="", flush=True)
        full_response += delta

        if finish:
            break

    print("")
    print("--------------------")
    return full_response

def close_session(sock, work_id):
    msg = {
        "request_id": "exit_" + work_id,
        "work_id": work_id,
        "action": "exit"
    }
    send_json(sock, msg)

def run_test(title, prompts):
    print("")
    print("====================================================")
    print(title)
    print("====================================================")

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(30)
    s.connect((HOST, PORT))

    try:
        work_id = setup_session(s, title)

        for i, prompt in enumerate(prompts):
            infer(s, work_id, title + "_" + str(i), prompt)
            time.sleep(1)

        close_session(s, work_id)

    finally:
        s.close()

def main():
    run_test(
        "TEST_01_SAME_SESSION_MEMORY",
        [
            "Hello.",
            "My name is Claudio.",
            "What is my name?"
        ]
    )

    run_test(
        "TEST_02_NEW_SESSION_MEMORY",
        [
            "What is my name?"
        ]
    )

    run_test(
        "TEST_03_CONTEXT_INJECTION",
        [
            "Context: user=Claudio role=professor. Who is the user?"
        ]
    )

    run_test(
        "TEST_04_EVENT_PRESENCE",
        [
            "Event: presence_detected. What should the system do?"
        ]
    )

    run_test(
        "TEST_05_EVENT_USER_IDENTIFIED",
        [
            "Event: user_identified. User: Claudio. Role: professor. What should the assistant say?"
        ]
    )

    run_test(
        "TEST_06_PRESENTATION_MODE",
        [
            "Environment: lab. User: Claudio. Role: professor. Event: presentation_mode. Describe the ideal behavior."
        ]
    )

if __name__ == "__main__":
    main()
