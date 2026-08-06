#!/usr/bin/env python3
"""
Echo Pyramid communication adapter for Ambient Physical AI.

This module provides two independent delivery paths:

1. Semantic command path (UDP/5005)
   Preserves the validated compact protocol used by display and RGB actions.

2. Voice audio path (TCP/5006)
   Sends generated or prerecorded PCM audio using the APAI Voice Protocol V1.

Semantic decisions and speech text remain centralized in the AX630C. The Echo
Pyramid only renders commands and audio received from the Cognitive Runtime.
"""

from __future__ import annotations

import os
import shlex
import socket
import struct
import subprocess
import tempfile
import wave
from pathlib import Path
from typing import Any, Dict, Mapping, Optional

from semantic_event import is_semantic_event


class EchoPyramidAdapter:
    """
    Deliver Semantic Events and speech audio to the Echo Pyramid Voice Node.

    UDP is used for compact semantic commands already validated by the project.
    TCP is used for PCM voice payloads because the complete audio stream must
    arrive in order.
    """

    DEFAULT_HOST = "192.168.1.204"
    DEFAULT_PORT = 5005
    DEFAULT_VOICE_PORT = 5006

    VOICE_MAGIC = b"APAI"
    VOICE_PROTOCOL_VERSION = 1
    VOICE_CHANNELS = 1
    VOICE_BITS_PER_SAMPLE = 16
    VOICE_SAMPLE_RATE = 16000
    VOICE_SEND_BLOCK_BYTES = 4096

    EVENT_COMMAND_MAP: Dict[str, str] = {
        "identity_authenticated": "WELCOME",
        "welcome_researcher": "WELCOME",
        "welcome_student": "WELCOME",
        "welcome_visitor": "WELCOME",
        "rgb_red": "RED",
        "rgb_green": "GREEN",
        "rgb_blue": "BLUE",
        "rgb_off": "OFF",
    }

    def __init__(self) -> None:
        """
        Initialize deployment configuration from environment variables.

        Optional TTS integration:
            ECHO_PYRAMID_TTS_COMMAND

        The command is a template containing:
            {text}   spoken text
            {output} temporary WAV output path

        Example:
            python3 /opt/tts/generate.py --text {text} --output {output}

        The generated WAV must be uncompressed PCM, mono, 16 kHz, 16-bit.
        """

        self.mode = os.getenv(
            "ECHO_PYRAMID_NOTIFY_MODE",
            "udp",
        ).strip().lower()

        self.host = os.getenv(
            "ECHO_PYRAMID_HOST",
            self.DEFAULT_HOST,
        ).strip()

        self.port = self._read_port(
            "ECHO_PYRAMID_PORT",
            self.DEFAULT_PORT,
        )

        self.voice_port = self._read_port(
            "ECHO_PYRAMID_VOICE_PORT",
            self.DEFAULT_VOICE_PORT,
        )

        self.tts_command = os.getenv(
            "ECHO_PYRAMID_TTS_COMMAND",
            "",
        ).strip()

    @staticmethod
    def _read_port(variable_name: str, default_port: int) -> int:
        value = os.getenv(variable_name, str(default_port)).strip()

        try:
            return int(value)
        except ValueError:
            print(
                "Echo Pyramid configuration warning: "
                f"invalid {variable_name} [{value}]; using {default_port}."
            )
            return default_port

    def notify(self, semantic_event: Mapping[str, Any]) -> bool:
        """
        Translate and deliver one normalized Semantic Event V1 through UDP.
        """

        if not is_semantic_event(semantic_event):
            raise ValueError("invalid Semantic Event V1")

        event_name = self._extract_event_name(semantic_event)

        if event_name is None:
            raise ValueError(
                "Semantic Event does not contain a valid event name"
            )

        print(f"[Echo Pyramid] Semantic event received: {event_name}")

        command = self._build_command(
            event_name=event_name,
            semantic_event=semantic_event,
        )

        if command is None:
            print(
                "Echo Pyramid event ignored: "
                f"unsupported semantic event [{event_name}]"
            )
            return True

        if self.mode != "udp":
            print(f"Echo Pyramid command prepared: {command}")
            print("Echo Pyramid delivery: DISABLED")
            return False

        return self._notify_udp(command)

    def speak(self, text: str) -> bool:
        """
        Generate speech through the configured AX630C TTS command and play it.

        This method intentionally does not embed a specific TTS engine. The
        AX630C deployment selects its installed engine through
        ECHO_PYRAMID_TTS_COMMAND, while this adapter owns validation and network
        delivery of the resulting WAV file.
        """

        normalized_text = " ".join(str(text).strip().split())

        if not normalized_text:
            print("Echo Pyramid speech rejected: empty text.")
            return False

        if not self.tts_command:
            print(
                "Echo Pyramid speech unavailable: "
                "ECHO_PYRAMID_TTS_COMMAND is not configured."
            )
            print(f"Speech text prepared: {normalized_text}")
            return False

        with tempfile.TemporaryDirectory(
            prefix="ambient_voice_",
        ) as temporary_directory:
            output_path = (
                Path(temporary_directory) / "speech_16k_mono_s16.wav"
            )

            command = self._build_tts_command(
                text=normalized_text,
                output_path=output_path,
            )

            print(f"Generating Echo Pyramid speech: {normalized_text}")

            try:
                completed = subprocess.run(
                    command,
                    check=False,
                    capture_output=True,
                    text=True,
                )
            except OSError as exc:
                print(f"Echo Pyramid TTS execution failed: {exc}")
                return False

            if completed.returncode != 0:
                print(
                    "Echo Pyramid TTS command failed "
                    f"with exit code {completed.returncode}."
                )
                if completed.stdout.strip():
                    print("TTS stdout:", completed.stdout.strip())
                if completed.stderr.strip():
                    print("TTS stderr:", completed.stderr.strip())
                return False

            if not output_path.is_file():
                print(
                    "Echo Pyramid TTS failed: expected WAV was not created: "
                    f"{output_path}"
                )
                return False

            return self.send_wav(output_path)

    def send_wav(self, wav_path: str | Path) -> bool:
        """
        Validate and transmit one compatible PCM WAV file through TCP/5006.
        """

        path = Path(wav_path)

        try:
            pcm_payload, sample_count = self._read_pcm_wav(path)
        except (OSError, ValueError, wave.Error) as exc:
            print(f"Echo Pyramid WAV validation failed: {exc}")
            return False

        return self.send_pcm(
            pcm_payload=pcm_payload,
            sample_count=sample_count,
        )

    def send_pcm(
        self,
        pcm_payload: bytes,
        sample_count: Optional[int] = None,
    ) -> bool:
        """
        Send signed 16-bit, little-endian, mono, 16 kHz PCM through TCP.

        ``sample_count`` is the number of mono samples, not the byte length.
        """

        if not isinstance(pcm_payload, (bytes, bytearray)):
            raise TypeError("pcm_payload must be bytes-like")

        if not pcm_payload:
            print("Echo Pyramid PCM delivery rejected: empty payload.")
            return False

        if len(pcm_payload) % 2 != 0:
            print(
                "Echo Pyramid PCM delivery rejected: "
                "16-bit payload has an odd byte length."
            )
            return False

        resolved_sample_count = (
            sample_count
            if sample_count is not None
            else len(pcm_payload) // 2
        )

        if resolved_sample_count <= 0:
            print("Echo Pyramid PCM delivery rejected: invalid sample count.")
            return False

        expected_bytes = resolved_sample_count * 2

        if expected_bytes != len(pcm_payload):
            print(
                "Echo Pyramid PCM delivery rejected: sample count does not "
                "match the payload length."
            )
            return False

        if not self._voice_target_is_valid():
            return False

        header = struct.pack(
            "!4sBBBBII",
            self.VOICE_MAGIC,
            self.VOICE_PROTOCOL_VERSION,
            self.VOICE_CHANNELS,
            self.VOICE_BITS_PER_SAMPLE,
            0,
            self.VOICE_SAMPLE_RATE,
            resolved_sample_count,
        )

        duration_seconds = (
            resolved_sample_count / self.VOICE_SAMPLE_RATE
        )

        try:
            with socket.create_connection(
                (self.host, self.voice_port),
                timeout=10,
            ) as sock:
                sock.sendall(header)

                for offset in range(
                    0,
                    len(pcm_payload),
                    self.VOICE_SEND_BLOCK_BYTES,
                ):
                    sock.sendall(
                        pcm_payload[
                            offset:offset + self.VOICE_SEND_BLOCK_BYTES
                        ]
                    )

                try:
                    sock.shutdown(socket.SHUT_WR)
                except OSError:
                    pass

            print(
                "Echo Pyramid voice sent: "
                f"{duration_seconds:.2f}s -> "
                f"{self.host}:{self.voice_port}"
            )
            return True

        except OSError as exc:
            print(
                "Echo Pyramid voice delivery failed for "
                f"{self.host}:{self.voice_port}: {exc}"
            )
            return False

    def _build_tts_command(
        self,
        text: str,
        output_path: Path,
    ) -> list[str]:
        """
        Resolve the configured TTS command template without invoking a shell.
        """

        formatted = self.tts_command.format(
            text=text,
            output=str(output_path),
        )
        return shlex.split(formatted)

    @classmethod
    def _read_pcm_wav(
        cls,
        path: Path,
    ) -> tuple[bytes, int]:
        """
        Read a WAV matching the embedded Voice Protocol V1 format.
        """

        if not path.is_file():
            raise ValueError(f"WAV file not found: {path}")

        with wave.open(str(path), "rb") as wav_file:
            channels = wav_file.getnchannels()
            sample_width = wav_file.getsampwidth()
            sample_rate = wav_file.getframerate()
            sample_count = wav_file.getnframes()
            compression = wav_file.getcomptype()

            if compression != "NONE":
                raise ValueError(
                    f"unsupported WAV compression: {compression}"
                )

            if channels != cls.VOICE_CHANNELS:
                raise ValueError(
                    f"invalid channels: {channels}; expected mono"
                )

            if sample_width * 8 != cls.VOICE_BITS_PER_SAMPLE:
                raise ValueError(
                    f"invalid sample width: {sample_width * 8}; "
                    "expected 16 bits"
                )

            if sample_rate != cls.VOICE_SAMPLE_RATE:
                raise ValueError(
                    f"invalid sample rate: {sample_rate}; expected 16000"
                )

            pcm_payload = wav_file.readframes(sample_count)

        expected_bytes = sample_count * 2

        if len(pcm_payload) != expected_bytes:
            raise ValueError(
                "incomplete WAV PCM payload: "
                f"{len(pcm_payload)} bytes; expected {expected_bytes}"
            )

        return pcm_payload, sample_count

    def _voice_target_is_valid(self) -> bool:
        if not self.host:
            print("Echo Pyramid voice host is not configured.")
            return False

        if self.voice_port <= 0 or self.voice_port > 65535:
            print("Echo Pyramid voice port is invalid.")
            return False

        return True

    @staticmethod
    def _extract_event_name(
        semantic_event: Mapping[str, Any],
    ) -> Optional[str]:
        event_type = semantic_event.get("event_type")

        if isinstance(event_type, str) and event_type.strip():
            normalized = event_type.strip().lower()

            if normalized != "semantic_event":
                return normalized

        legacy_event = semantic_event.get("event")

        if isinstance(legacy_event, str) and legacy_event.strip():
            return legacy_event.strip().lower()

        return None

    def _build_command(
        self,
        event_name: str,
        semantic_event: Mapping[str, Any],
    ) -> Optional[str]:
        base_command = self.EVENT_COMMAND_MAP.get(event_name)

        if base_command is None:
            return None

        if base_command != "WELCOME":
            return base_command

        profile_name = self._extract_profile_name(semantic_event)

        if profile_name is None:
            return base_command

        return f"{base_command}|{profile_name}"

    @classmethod
    def _extract_profile_name(
        cls,
        semantic_event: Mapping[str, Any],
    ) -> Optional[str]:
        candidate_containers = [
            semantic_event.get("profile"),
            semantic_event.get("user"),
            semantic_event.get("identity"),
        ]

        for envelope_key in ("payload", "data", "context"):
            envelope = semantic_event.get(envelope_key)

            if isinstance(envelope, Mapping):
                candidate_containers.extend(
                    [
                        envelope.get("profile"),
                        envelope.get("user"),
                        envelope.get("identity"),
                    ]
                )

        payload = semantic_event.get("payload")
        if isinstance(payload, Mapping):
            for field_name in ("user_name", "user_id"):
                sanitized = cls._sanitize_parameter(
                    payload.get(field_name)
                )
                if sanitized is not None:
                    return sanitized

        context = semantic_event.get("context")
        if isinstance(context, Mapping):
            for field_name in (
                "active_user",
                "user_name",
                "user_id",
            ):
                sanitized = cls._sanitize_parameter(
                    context.get(field_name)
                )
                if sanitized is not None:
                    return sanitized

        for container in candidate_containers:
            if not isinstance(container, Mapping):
                continue

            for field_name in (
                "display_name",
                "name",
                "id",
                "role",
            ):
                sanitized = cls._sanitize_parameter(
                    container.get(field_name)
                )

                if sanitized is not None:
                    return sanitized

        for direct_field in (
            "display_name",
            "user_name",
            "profile_name",
            "profile_id",
        ):
            sanitized = cls._sanitize_parameter(
                semantic_event.get(direct_field)
            )

            if sanitized is not None:
                return sanitized

        return None

    @staticmethod
    def _sanitize_parameter(value: Any) -> Optional[str]:
        if not isinstance(value, str):
            return None

        normalized = " ".join(value.strip().split())

        if not normalized:
            return None

        normalized = normalized.replace("|", " ")
        normalized = normalized[:64].strip()

        return normalized or None

    def _notify_udp(self, command: str) -> bool:
        if not self.host:
            print(
                "Echo Pyramid UDP mode selected, "
                "but no host is configured."
            )
            return False

        if self.port <= 0 or self.port > 65535:
            print(
                "Echo Pyramid UDP mode selected, "
                "but the port is invalid."
            )
            return False

        try:
            with socket.socket(
                socket.AF_INET,
                socket.SOCK_DGRAM,
            ) as sock:
                sock.sendto(
                    command.encode("utf-8"),
                    (self.host, self.port),
                )

            print(
                "Echo Pyramid command sent: "
                f"{command} -> {self.host}:{self.port}"
            )
            return True

        except OSError as exc:
            print(
                "Echo Pyramid delivery failed for "
                f"{self.host}:{self.port}: {exc}"
            )
            return False
