# Python Client for StackFlow on AX630C

## `client.py` commented and explained in English

Project: **Ambient Physical AI**  
Module: **AX630C + StackFlow**  
Validated model: **Qwen2.5-0.5B-prefill-20e**  
Validated interface: **TCP 10001 / JSON over TCP**

---

# 1. Purpose of this Client

This Python client demonstrates how to communicate directly with the local AX630C runtime without using the official M5Stack application.

The validated communication flow is:

```text
Python Client
    ↓
TCP 10001
    ↓
llm_sys
    ↓
llm_llm
    ↓
Qwen2.5-0.5B
```

The client performs the following steps:

1. Opens a TCP connection to the StackFlow Runtime.
2. Sends a `setup` message to create an LLM session.
3. Receives a dynamic `work_id`, for example `llm.1001`.
4. Sends a prompt to the LLM.
5. Receives the response as a stream.
6. Sends `exit` to terminate the session.

---

# 2. Fully Commented Source Code

Save this file as:

```text
/root/client.py
```

on the AX630C.

```python
# Import the socket module.
# The socket module allows a direct TCP connection to the StackFlow Runtime.
import socket

# Import the json module.
# The discovered protocol uses JSON messages transmitted over TCP.
import json


# StackFlow server address.
# Since this script runs on the AX630C itself, localhost is used.
HOST = "127.0.0.1"

# TCP port where the llm_sys process is listening.
# This port was identified using the command:
# ss -lntp
PORT = 10001

# Name of the LLM model installed on the AX630C.
# This model was identified at:
# /opt/m5stack/data/qwen2.5-0.5B-prefill-20e/
MODEL = "qwen2.5-0.5B-prefill-20e"


def send_json(sock, payload):
    """
    Sends a JSON message to the StackFlow Runtime.

    Parameters:
    - sock: an already connected TCP socket.
    - payload: a Python dictionary that will be converted to JSON.

    Important note:
    The protocol uses JSON over TCP with a newline terminator.

    Therefore, we send:

        json + "\n"

    Without the "\n", the server may continue waiting for the end of the message.
    """

    # Convert the Python dictionary into a JSON string.
    # ensure_ascii=False allows Unicode characters to be transmitted correctly.
    message = json.dumps(payload, ensure_ascii=False)

    # Append the message terminator required by the protocol.
    message = message + "\n"

    # Convert the string to UTF-8 bytes and send it through the socket.
    sock.sendall(message.encode("utf-8"))
    
def recv_json(sock):
    """
    Receives a JSON message from the StackFlow Runtime.

    The server sends responses terminated by "\n".
    This function reads data from the socket until a newline character is found.

    Returns:
    - A Python dictionary if a valid JSON message is received.
    - None if the connection is closed.
    """

    # Temporary buffer used to accumulate the received data.
    buf = ""

    # Continue receiving data until the message terminator is found.
    while "\n" not in buf:
        # Receive up to 4096 bytes from the socket.
        chunk = sock.recv(4096)

        # If no data is received, the connection has been closed.
        if not chunk:
            return None

        # Decode the received bytes into text.
        # errors="replace" prevents failures if invalid characters are received.
        buf += chunk.decode("utf-8", errors="replace")

    # Split at the first newline character.
    # line contains one complete JSON message.
    line, _ = buf.split("\n", 1)

    # Convert the JSON text into a Python dictionary.
    return json.loads(line)


def main():
    """
    Main client function.

    It performs the complete workflow:

    1. Connect
    2. LLM setup
    3. Send prompt
    4. Receive the streaming response
    5. Terminate the session
    """

    # Create an IPv4 TCP socket.
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Set a timeout to prevent the script from blocking indefinitely.
    # If the server does not respond within 30 seconds, an exception is raised.
    s.settimeout(30)

    # Connect to the local StackFlow Runtime.
    s.connect((HOST, PORT))


    # ------------------------------
    # 1. LLM SESSION SETUP
    # ------------------------------

    # Before performing inference, we must create a session.
    # The initial work_id is "llm".
    # The runtime will return a new dynamic work_id, such as "llm.1001".
    setup = {
        "request_id": "setup_client_001",
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

    # Send the setup message.
    send_json(s, setup)

    # Wait for the runtime response.
    setup_resp = recv_json(s)

    # Display the complete setup response.
    print("SETUP:", setup_resp)

    # Extract the actual work_id returned by the runtime.
    # Example: "llm.1001"
    work_id = setup_resp["work_id"]


    # ------------------------------
    # 2. SENDING THE PROMPT
    # ------------------------------

    # Inference message.
    #
    # The "delta" field contains the text sent to the LLM.
    # The "finish": True field indicates that this is the end of the input.
    inference = {
        "request_id": "infer_client_001",
        "work_id": work_id,
        "action": "inference",
        "object": "llm.utf-8.stream",
        "data": {
            "delta": "Hello",
            "index": 0,
            "finish": True
        }
    }

    # Send the prompt to the LLM.
    send_json(s, inference)


    # ------------------------------
    # 3. RECEIVING THE RESPONSE
    # ------------------------------

    print("RESPONSE:", end=" ", flush=True)

    # The runtime returns the response as a stream.
    # Therefore, multiple JSON packets are received.
    while True:
        # Read one JSON message from the socket.
        resp = recv_json(s)

        # If no response is received, terminate.
        if resp is None:
            break

        # The "data" field contains the response fragment.
        data = resp.get("data", {})

        # "delta" contains the generated text fragment.
        delta = data.get("delta", "")

        # "finish" indicates whether the response has completed.
        finish = data.get("finish", False)

        # Print the fragment without inserting a newline.
        print(delta, end="", flush=True)

        # When finish=True, the response is complete.
        if finish:
            break

    # Print the final newline.
    print()


    # ------------------------------
    # 4. TERMINATING THE SESSION
    # ------------------------------

    # Send the exit message.
    # This informs the runtime that the session can be closed.
    exit_msg = {
        "request_id": "exit_client_001",
        "work_id": work_id,
        "action": "exit"
    }

    # Send the exit command.
    send_json(s, exit_msg)

    # Close the TCP socket.
    s.close()


# Script entry point.
if __name__ == "__main__":
    main()
```

---

# 3. How to Run

From the AX630C shell:

```bash
python3 /root/client.py
```

Expected output:

```text
SETUP: {'created': ..., 'error': {'code': 0, 'message': ''}, 'work_id': 'llm.1001'}
RESPONSE: Hello! How can I assist you today?
```

---

# 4. Protocol Explanation

## Port

```text
10001/TCP
```

## Responsible Process

```text
llm_sys
```

## Protocol

```text
JSON over TCP
```

## Message Terminator

```text
\n
```

Every message sent to the runtime must end with a newline character.

---

# 5. Logical Flow

```text
1. connect()
2. setup
3. receive work_id
4. inference
5. receive streaming response
6. exit
7. close()
```

---

# 6. What is `work_id`?

The `work_id` identifies a session within the runtime.

The initial setup uses:

```json
"work_id": "llm"
```

The runtime responds with something similar to:

```json
"work_id": "llm.1001"
```

After that, all inference commands must use the new `work_id`.

---

# 7. What is Streaming?

The response does not arrive all at once.

Instead, it is delivered in multiple fragments:

```json
{"delta":"Hello! How","finish":false}
{"delta":" can I assist","finish":false}
{"delta":" you today?","finish":false}
{"delta":"","finish":true}
```

The client must concatenate the `delta` fields until it receives:

```json
"finish": true
```

---

# 8. How to Change the Prompt

In the code, change:

```python
"delta": "Hello"
```

for example, to:

```python
"delta": "Explain what MQTT is in one sentence."
```

---

# 9. Important Considerations

## Use a Timeout

The socket uses:

```python
s.settimeout(30)
```

This prevents the script from blocking indefinitely.

## Always Send `\n`

Without a newline character, the runtime may not process the message.

## Always Perform Setup Before Inference

Direct inference without setup returned the following error:

```text
inference data push false
```

---

# 10. Conclusion

This client demonstrates that the AX630C can operate as a local AI server.

Using this client as a foundation, future Ambient Physical AI nodes will be able to communicate with the cognitive runtime over TCP, including:

- CoreS3 Lite
- StackChan
- PoE-P4
- Voice Node
- Ambient Node

This is the first validated functional client for direct communication with the StackFlow Runtime.