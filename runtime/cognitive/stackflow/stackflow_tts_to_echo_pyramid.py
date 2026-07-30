#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
StackFlow TTS -> Echo Pyramid validation client.

Flow:
    Text
      -> StackFlow llm_sys TCP/10001
      -> single_speaker_english_fast
      -> tts.base64.wav
      -> temporary WAV
      -> EchoPyramidAdapter.send_wav()
      -> Echo Pyramid TCP/5006

This is a technical validation client. The installed English model may not
pronounce Portuguese naturally.
"""

from __future__ import annotations

import argparse
import base64
import json
import socket
import tempfile
import time
import uuid
import wave
from pathlib import Path
from typing import Any, Dict, Optional

from echo_pyramid_adapter import EchoPyramidAdapter


class StackFlowClient:
    def __init__(
        self,
        host: str = "127.0.0.1",
        port: int = 10001,
        timeout: float = 60.0,
    ) -> None:
        self.host = host
        self.port = port
        self.timeout = timeout
        self.sock: Optional[socket.socket] = None
        self.rx_buffer = b""

    def connect(self) -> None:
        self.sock = socket.create_connection(
            (self.host, self.port),
            timeout=self.timeout,
        )
        self.sock.settimeout(self.timeout)

    def close(self) -> None:
        if self.sock is not None:
            try:
                self.sock.close()
            finally:
                self.sock = None

    def send_json(self, payload: Dict[str, Any]) -> None:
        if self.sock is None:
            raise RuntimeError("StackFlow socket is not connected")

        encoded = (
            json.dumps(payload, ensure_ascii=False) + "\n"
        ).encode("utf-8")
        self.sock.sendall(encoded)

    def recv_json(self) -> Dict[str, Any]:
        if self.sock is None:
            raise RuntimeError("StackFlow socket is not connected")

        while True:
            if b"\n" in self.rx_buffer:
                line, self.rx_buffer = self.rx_buffer.split(b"\n", 1)
                line = line.strip()
                if not line:
                    continue
                return json.loads(
                    line.decode("utf-8", errors="replace")
                )

            chunk = self.sock.recv(65536)
            if not chunk:
                raise ConnectionError(
                    "StackFlow closed the TCP connection"
                )
            self.rx_buffer += chunk


def request_id() -> str:
    return str(uuid.uuid4())


def assert_success(response: Dict[str, Any]) -> None:
    error = response.get("error") or {}
    code = error.get("code", 0)
    message = error.get("message", "")

    if code != 0:
        raise RuntimeError(
            f"StackFlow error code={code}: {message}"
        )


def setup_tts(client: StackFlowClient, model: str) -> str:
    payload = {
        "request_id": request_id(),
        "work_id": "tts",
        "action": "setup",
        "object": "tts.setup",
        "data": {
            "model": model,
            "response_format": "tts.base64.wav",
            "input": "tts.utf-8",
            "enoutput": True,
            "enkws": False,
        },
    }

    print("Configuring StackFlow TTS...")
    client.send_json(payload)
    response = client.recv_json()
    print(
        "TTS setup response:",
        json.dumps(response, ensure_ascii=False),
    )
    assert_success(response)

    work_id = response.get("work_id")
    if not isinstance(work_id, str) or not work_id:
        raise RuntimeError(
            "StackFlow setup response did not contain work_id"
        )

    return work_id


def start_inference(
    client: StackFlowClient,
    work_id: str,
    text: str,
) -> str:
    inference_request_id = request_id()
    payload = {
        "request_id": inference_request_id,
        "work_id": work_id,
        "action": "inference",
        "object": "tts.utf-8",
        "data": text,
    }

    print(f"Submitting TTS text: {text}")
    client.send_json(payload)
    return inference_request_id


def extract_audio_payload(
    message: Dict[str, Any],
) -> Optional[tuple[bytes, bool]]:
    """
    Extract one base64 audio payload.

    Returns:
        (audio_bytes, is_wav)

    StackFlow deployments may label the response as WAV while returning raw
    signed 16-bit PCM bytes. Both representations are accepted here.
    """
    obj = message.get("object")
    data = message.get("data")

    if obj not in {
        "tts.base64.wav",
        "tts.wav.base64",
        "tts.wav",
        "tts.pcm",
        "sys.pcm",
        "None",
        None,
    }:
        return None

    candidate: Any = data

    if isinstance(data, dict):
        for key in (
            "base64",
            "wav",
            "audio",
            "pcm",
            "data",
            "delta",
        ):
            if data.get(key):
                candidate = data[key]
                break

    if not isinstance(candidate, str) or not candidate.strip():
        return None

    encoded = candidate.strip()

    if encoded.lower() in {"none", "null"}:
        return None

    if encoded.startswith("data:") and "," in encoded:
        encoded = encoded.split(",", 1)[1]

    try:
        audio_bytes = base64.b64decode(encoded, validate=False)
    except Exception as exc:
        raise ValueError(
            f"Could not decode TTS base64 audio: {exc}"
        ) from exc

    if not audio_bytes:
        return None

    return audio_bytes, audio_bytes.startswith(b"RIFF")


def pcm_to_wav_bytes(
    pcm_bytes: bytes,
    sample_rate: int = 16000,
    channels: int = 1,
    sample_width: int = 2,
) -> bytes:
    """
    Wrap raw little-endian signed PCM in a standard WAV container.
    """
    if len(pcm_bytes) % sample_width != 0:
        raise ValueError(
            "Raw PCM byte length is not aligned to the sample width"
        )

    import io

    buffer = io.BytesIO()
    with wave.open(buffer, "wb") as wav_file:
        wav_file.setnchannels(channels)
        wav_file.setsampwidth(sample_width)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(pcm_bytes)

    return buffer.getvalue()


def wait_for_wav(
    client: StackFlowClient,
    timeout_seconds: float,
    idle_timeout_seconds: float = 1.5,
) -> bytes:
    """
    Collect the complete StackFlow TTS audio stream.

    The installed StackFlow TTS service returns raw PCM in multiple base64
    messages even when response_format requests tts.base64.wav. The stream
    does not provide a reliable explicit final marker on every release, so
    completion is detected by a short period without new audio after the
    first chunk has arrived.
    """
    deadline = time.monotonic() + timeout_seconds
    pcm_chunks: list[bytes] = []
    wav_chunks: list[bytes] = []
    received_audio = False

    while time.monotonic() < deadline:
        remaining_total = deadline - time.monotonic()
        receive_timeout = (
            min(idle_timeout_seconds, remaining_total)
            if received_audio
            else remaining_total
        )

        if client.sock is not None:
            client.sock.settimeout(max(0.1, receive_timeout))

        try:
            message = client.recv_json()
        except socket.timeout:
            if received_audio:
                print(
                    "StackFlow TTS stream complete "
                    "(audio inactivity boundary reached)."
                )
                break
            raise TimeoutError(
                "Timed out waiting for the first StackFlow TTS audio block"
            )

        preview = json.dumps(message, ensure_ascii=False)
        print("StackFlow message:", preview[:500])
        assert_success(message)

        extracted = extract_audio_payload(message)
        if extracted is None:
            continue

        audio_bytes, is_wav = extracted

        # Ignore textual protocol placeholders that are not audio.
        if len(audio_bytes) < 16:
            continue

        received_audio = True

        if is_wav:
            wav_chunks.append(audio_bytes)
            print(
                f"StackFlow WAV block received: {len(audio_bytes)} bytes"
            )
        else:
            pcm_chunks.append(audio_bytes)
            print(
                f"StackFlow PCM block received: {len(audio_bytes)} bytes"
            )

    if not received_audio:
        raise TimeoutError(
            "No StackFlow TTS audio was received"
        )

    if wav_chunks and pcm_chunks:
        raise ValueError(
            "StackFlow returned a mixed WAV/PCM stream"
        )

    if wav_chunks:
        if len(wav_chunks) > 1:
            raise ValueError(
                "StackFlow returned multiple independent WAV containers"
            )
        return wav_chunks[0]

    complete_pcm = b"".join(pcm_chunks)

    print(
        "StackFlow raw PCM stream assembled: "
        f"{len(pcm_chunks)} block(s), {len(complete_pcm)} bytes."
    )
    print(
        "Wrapping complete stream as 16 kHz mono 16-bit WAV."
    )

    return pcm_to_wav_bytes(complete_pcm)


def exit_unit(
    client: StackFlowClient,
    work_id: str,
    timeout_seconds: float = 10.0,
) -> None:
    """
    Release the TTS work unit and wait for its acknowledgement.

    Waiting for the release response avoids a race where a subsequent setup
    request arrives while the previous TTS unit is still being deinitialized.
    """
    exit_request_id = request_id()

    try:
        client.send_json(
            {
                "request_id": exit_request_id,
                "work_id": work_id,
                "action": "exit",
                "object": "None",
                "data": "None",
            }
        )

        deadline = time.monotonic() + timeout_seconds

        while time.monotonic() < deadline:
            if client.sock is not None:
                client.sock.settimeout(
                    max(0.1, deadline - time.monotonic())
                )

            response = client.recv_json()
            assert_success(response)

            if response.get("request_id") == exit_request_id:
                print(
                    f"StackFlow TTS unit released: {work_id}"
                )
                return

        print(
            f"Warning: no TTS exit acknowledgement for {work_id}"
        )

    except Exception as exc:
        print(
            f"Warning: could not confirm release of {work_id}: {exc}"
        )


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Generate speech with StackFlow TTS and send it "
            "to the Echo Pyramid."
        )
    )
    parser.add_argument(
        "--text",
        default="Welcome to the laboratory, Herminio.",
        help=(
            "Text to synthesize. End the sentence with a period."
        ),
    )
    parser.add_argument(
        "--stackflow-host",
        default="127.0.0.1",
    )
    parser.add_argument(
        "--stackflow-port",
        type=int,
        default=10001,
    )
    parser.add_argument(
        "--model",
        default="single_speaker_english_fast",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=60.0,
    )
    args = parser.parse_args()

    text = args.text.strip()
    if not text:
        raise SystemExit("Text cannot be empty")

    if not text.endswith((".", "!", "?")):
        text += "."

    client = StackFlowClient(
        host=args.stackflow_host,
        port=args.stackflow_port,
        timeout=args.timeout,
    )
    work_id: Optional[str] = None

    try:
        client.connect()
        work_id = setup_tts(client, args.model)
        start_inference(client, work_id, text)
        wav_bytes = wait_for_wav(client, args.timeout)

        with tempfile.TemporaryDirectory(
            prefix="stackflow_tts_",
        ) as temp_directory:
            wav_path = Path(temp_directory) / "speech.wav"
            wav_path.write_bytes(wav_bytes)

            print(
                f"StackFlow WAV received: "
                f"{len(wav_bytes)} bytes"
            )

            adapter = EchoPyramidAdapter()
            result = adapter.send_wav(wav_path)

            print(
                "Echo Pyramid playback:",
                "PASS" if result else "FAIL",
            )
            return 0 if result else 1

    finally:
        if work_id is not None:
            exit_unit(client, work_id)
        client.close()


if __name__ == "__main__":
    raise SystemExit(main())
