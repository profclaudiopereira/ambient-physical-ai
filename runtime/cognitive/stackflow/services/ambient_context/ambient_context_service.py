#!/usr/bin/env python3
"""
Ambient Physical AI — Ambient Context Service

Runs on the Cognitive Runtime (AX630C), gathers one global weather context and
one profile-specific context, normalizes both into the Ambient Runtime contract,
and sends the resulting JSON to the Tab5 over UDP port 5555.

This service is intentionally independent from the Tab5 firmware. API selection,
credentials, profile relevance and normalization remain in the Cognitive Runtime.
"""

from __future__ import annotations

import argparse
import json
import logging
import os
import socket
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any, Dict, Optional, Tuple


LOG = logging.getLogger("ambient-context")

DEFAULT_TIMEOUT_SECONDS = 10
DEFAULT_TTL_SECONDS = 900
DEFAULT_TAB5_PORT = 5555

# Recife / PE. Override through environment variables when required.
DEFAULT_LATITUDE = -8.0476
DEFAULT_LONGITUDE = -34.8770
DEFAULT_LOCATION_LABEL = "RECIFE"


class ContextSourceError(RuntimeError):
    """Raised when an external context source cannot provide usable data."""


@dataclass(frozen=True)
class RuntimeConfig:
    tab5_host: str
    tab5_port: int
    profile_id: str
    latitude: float
    longitude: float
    location_label: str
    ttl_seconds: int
    interval_seconds: int
    once: bool
    dry_run: bool


def _http_get_json(
    url: str,
    *,
    headers: Optional[Dict[str, str]] = None,
    timeout: int = DEFAULT_TIMEOUT_SECONDS,
) -> Dict[str, Any]:
    request_headers = {
        "Accept": "application/json",
        "User-Agent": "AmbientPhysicalAI/1.0",
    }
    if headers:
        request_headers.update(headers)

    request = urllib.request.Request(url, headers=request_headers, method="GET")

    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            payload = response.read().decode("utf-8")
    except (urllib.error.URLError, TimeoutError, OSError) as exc:
        raise ContextSourceError(f"HTTP request failed: {exc}") from exc

    try:
        parsed = json.loads(payload)
    except json.JSONDecodeError as exc:
        raise ContextSourceError("Source returned invalid JSON") from exc

    if not isinstance(parsed, dict):
        raise ContextSourceError("Source returned an unexpected JSON root")

    return parsed


def _compact_ascii(value: Any, limit: int) -> str:
    """
    Produces a compact OLED-safe string.

    The present firmware uses a 5x7 ASCII font. Accents are normalized only for
    the small set needed by this demo, avoiding corrupted glyphs on the OLED.
    """
    text = str(value or "").strip()
    replacements = str.maketrans(
        {
            "á": "a", "à": "a", "â": "a", "ã": "a",
            "Á": "A", "À": "A", "Â": "A", "Ã": "A",
            "é": "e", "ê": "e", "É": "E", "Ê": "E",
            "í": "i", "Í": "I",
            "ó": "o", "ô": "o", "õ": "o",
            "Ó": "O", "Ô": "O", "Õ": "O",
            "ú": "u", "Ú": "U",
            "ç": "c", "Ç": "C",
            "–": "-", "—": "-", "º": "o",
        }
    )
    text = text.translate(replacements)
    text = "".join(ch for ch in text if 32 <= ord(ch) <= 126)
    text = " ".join(text.split())
    return text[:limit]


def _weather_label(code: int) -> str:
    labels = {
        0: "CLEAR",
        1: "MOSTLY CLEAR",
        2: "PARTLY CLOUDY",
        3: "CLOUDY",
        45: "FOG",
        48: "RIME FOG",
        51: "LIGHT DRIZZLE",
        53: "DRIZZLE",
        55: "HEAVY DRIZZLE",
        56: "FREEZING DRIZZLE",
        57: "FREEZING DRIZZLE",
        61: "LIGHT RAIN",
        63: "RAIN",
        65: "HEAVY RAIN",
        66: "FREEZING RAIN",
        67: "FREEZING RAIN",
        71: "LIGHT SNOW",
        73: "SNOW",
        75: "HEAVY SNOW",
        77: "SNOW GRAINS",
        80: "RAIN SHOWERS",
        81: "RAIN SHOWERS",
        82: "HEAVY SHOWERS",
        85: "SNOW SHOWERS",
        86: "HEAVY SNOW",
        95: "THUNDERSTORM",
        96: "STORM HAIL",
        99: "STORM HAIL",
    }
    return labels.get(int(code), "WEATHER")


def _uv_label(uv_index: float) -> str:
    if uv_index < 3.0:
        return "LOW"
    if uv_index < 6.0:
        return "MODERATE"
    if uv_index < 8.0:
        return "HIGH"
    if uv_index < 11.0:
        return "VERY HIGH"
    return "EXTREME"


def fetch_global_weather(config: RuntimeConfig) -> Dict[str, Any]:
    """
    Fetches current weather and today's UV maximum from Open-Meteo in one call.
    """
    params = {
        "latitude": f"{config.latitude:.6f}",
        "longitude": f"{config.longitude:.6f}",
        "current": "temperature_2m,weather_code",
        "daily": "uv_index_max,weather_code,temperature_2m_max,temperature_2m_min",
        "timezone": "America/Recife",
        "forecast_days": "2",
    }
    url = "https://api.open-meteo.com/v1/forecast?" + urllib.parse.urlencode(params)
    data = _http_get_json(url)

    current = data.get("current")
    daily = data.get("daily")

    if not isinstance(current, dict) or not isinstance(daily, dict):
        raise ContextSourceError("Open-Meteo response lacks current/daily data")

    temperature = float(current.get("temperature_2m", 0.0))
    weather_code = int(current.get("weather_code", 0))

    uv_values = daily.get("uv_index_max")
    uv_index = (
        float(uv_values[0])
        if isinstance(uv_values, list) and uv_values
        else 0.0
    )

    return {
        "available": True,
        "location": _compact_ascii(config.location_label.upper(), 23),
        "weather_summary": _compact_ascii(_weather_label(weather_code), 31),
        "temperature_c": round(temperature, 1),
        "uv_index": round(uv_index, 1),
        "uv_label": _compact_ascii(_uv_label(uv_index), 15),
    }


def fetch_currency_context() -> Dict[str, Any]:
    """Returns USD/BRL quote data for Claudio."""
    url = "https://economia.awesomeapi.com.br/json/last/USD-BRL"
    data = _http_get_json(url)
    quote = data.get("USDBRL")

    if not isinstance(quote, dict):
        raise ContextSourceError("USD-BRL quotation not found")

    bid = float(quote.get("bid", 0.0))
    pct_change = float(quote.get("pctChange", 0.0))
    sign = "+" if pct_change > 0 else ""

    return {
        "available": True,
        "title": "USD BRL",
        "value": f"R$ {bid:.2f}",
        "secondary": f"{sign}{pct_change:.2f}%",
    }


def _sportsdb_key() -> str:
    # TheSportsDB documents 123 as the V1 development/test key. A personal key
    # can replace it through the environment without changing the source.
    return os.getenv("THESPORTSDB_API_KEY", "123").strip() or "123"


def _find_sport_team_id(team_name: str) -> str:
    encoded = urllib.parse.quote(team_name)
    url = (
        f"https://www.thesportsdb.com/api/v1/json/{_sportsdb_key()}"
        f"/searchteams.php?t={encoded}"
    )
    data = _http_get_json(url)
    teams = data.get("teams")

    if not isinstance(teams, list) or not teams:
        raise ContextSourceError(f"Sports team not found: {team_name}")

    normalized_target = team_name.casefold()
    selected = next(
        (
            team for team in teams
            if isinstance(team, dict)
            and str(team.get("strTeam", "")).casefold() == normalized_target
        ),
        teams[0],
    )

    team_id = selected.get("idTeam") if isinstance(selected, dict) else None
    if not team_id:
        raise ContextSourceError("Sports team response lacks idTeam")

    return str(team_id)


def fetch_sport_context() -> Dict[str, Any]:
    """Returns the next scheduled Sport Club do Recife match for Herminio."""
    team_name = os.getenv("SPORT_TEAM_NAME", "Sport Club do Recife")
    team_id = os.getenv("SPORT_TEAM_ID", "").strip() or _find_sport_team_id(team_name)

    url = (
        f"https://www.thesportsdb.com/api/v1/json/{_sportsdb_key()}"
        f"/eventsnext.php?id={urllib.parse.quote(team_id)}"
    )
    data = _http_get_json(url)
    events = data.get("events")

    if not isinstance(events, list) or not events:
        raise ContextSourceError("No upcoming Sport event returned")

    event = events[0]
    if not isinstance(event, dict):
        raise ContextSourceError("Unexpected sports event format")

    home = str(event.get("strHomeTeam", "")).strip()
    away = str(event.get("strAwayTeam", "")).strip()
    event_name = str(event.get("strEvent", "")).strip()

    matchup = f"{home} x {away}" if home and away else event_name
    date_text = str(event.get("dateEvent", "")).strip()
    time_text = str(event.get("strTime", "")).strip()[:5]
    schedule = " ".join(part for part in (date_text, time_text) if part)

    return {
        "available": True,
        "title": "SPORT",
        "value": _compact_ascii(matchup.upper(), 31),
        "secondary": _compact_ascii(schedule, 31),
    }


def fetch_cinema_context() -> Dict[str, Any]:
    """
    Returns one movie currently listed as now playing in Brazil for Mariana.

    TMDB provides national now-playing data, not a guaranteed Recife theater
    schedule. A local cultural-agenda adapter can replace this function later
    without changing the Tab5 contract.
    """
    token = os.getenv("TMDB_BEARER_TOKEN", "").strip()
    if not token:
        raise ContextSourceError("TMDB_BEARER_TOKEN is not configured")

    params = {
        "language": "pt-BR",
        "region": "BR",
        "page": "1",
    }
    url = (
        "https://api.themoviedb.org/3/movie/now_playing?"
        + urllib.parse.urlencode(params)
    )
    data = _http_get_json(
        url,
        headers={"Authorization": f"Bearer {token}"},
    )

    results = data.get("results")
    if not isinstance(results, list) or not results:
        raise ContextSourceError("TMDB returned no now-playing movie")

    movie = results[0]
    if not isinstance(movie, dict):
        raise ContextSourceError("Unexpected TMDB movie format")

    title = movie.get("title") or movie.get("original_title") or "MOVIE"
    release_date = str(movie.get("release_date", "")).strip()
    vote_average = float(movie.get("vote_average", 0.0))

    secondary_parts = []
    if release_date:
        secondary_parts.append(release_date)
    if vote_average > 0:
        secondary_parts.append(f"TMDB {vote_average:.1f}")

    return {
        "available": True,
        "title": "CINEMA BR",
        "value": _compact_ascii(str(title).upper(), 31),
        "secondary": _compact_ascii(" ".join(secondary_parts), 31),
    }


def fetch_steam_context() -> Dict[str, Any]:
    """Returns one Steam special offer for the student profile."""
    params = {"cc": "br", "l": "brazilian"}
    url = (
        "https://store.steampowered.com/api/featuredcategories?"
        + urllib.parse.urlencode(params)
    )
    data = _http_get_json(url)

    specials = data.get("specials")
    items = specials.get("items") if isinstance(specials, dict) else None

    if not isinstance(items, list) or not items:
        raise ContextSourceError("Steam returned no special offers")

    # Prefer the largest discount among the first public featured offers.
    candidates = [item for item in items if isinstance(item, dict)]
    if not candidates:
        raise ContextSourceError("Steam special-offer format is empty")

    game = max(candidates, key=lambda item: int(item.get("discount_percent", 0)))
    name = str(game.get("name", "STEAM GAME"))
    discount = int(game.get("discount_percent", 0))
    final_price = game.get("final_price")

    secondary = f"-{discount}%"
    if isinstance(final_price, int):
        secondary += f" R$ {final_price / 100.0:.2f}"

    return {
        "available": True,
        "title": "STEAM DEAL",
        "value": _compact_ascii(name.upper(), 31),
        "secondary": _compact_ascii(secondary, 31),
    }


def unavailable_personal(reason: str) -> Dict[str, Any]:
    LOG.warning("Personal context unavailable: %s", reason)
    return {
        "available": False,
        "title": "",
        "value": "",
        "secondary": "",
    }


def build_personal_context(profile_id: str) -> Dict[str, Any]:
    normalized = profile_id.strip().casefold()

    try:
        if normalized in {"claudio", "owner", "researcher"}:
            return fetch_currency_context()
        if normalized in {"herminio", "hermínio"}:
            return fetch_sport_context()
        if normalized in {"mariana"}:
            return fetch_cinema_context()
        if normalized in {"student", "estudante", "learner"}:
            return fetch_steam_context()
    except ContextSourceError as exc:
        return unavailable_personal(str(exc))

    return unavailable_personal(f"no adapter mapped for profile '{profile_id}'")


def build_ambient_context(
    config: RuntimeConfig,
    sequence: int,
) -> Dict[str, Any]:
    try:
        global_context = fetch_global_weather(config)
    except ContextSourceError as exc:
        LOG.warning("Global weather unavailable: %s", exc)
        global_context = {
            "available": False,
            "location": _compact_ascii(config.location_label.upper(), 23),
            "weather_summary": "",
            "temperature_c": 0.0,
            "uv_index": 0.0,
            "uv_label": "",
        }

    personal_context = build_personal_context(config.profile_id)

    return {
        "type": "ambient_context",
        "sequence": sequence,
        "ttl_seconds": config.ttl_seconds,
        "authenticated": bool(config.profile_id),
        "profile_id": _compact_ascii(config.profile_id, 23),
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "global": global_context,
        "personal": personal_context,
    }


def send_udp(payload: Dict[str, Any], host: str, port: int) -> int:
    encoded = json.dumps(
        payload,
        ensure_ascii=True,
        separators=(",", ":"),
    ).encode("utf-8")

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        return sock.sendto(encoded, (host, port))


def run(config: RuntimeConfig) -> None:
    sequence = 1

    while True:
        payload = build_ambient_context(config, sequence)
        rendered = json.dumps(payload, ensure_ascii=False, indent=2)
        print(rendered, flush=True)

        if not config.dry_run:
            sent = send_udp(payload, config.tab5_host, config.tab5_port)
            LOG.info(
                "Sent %d bytes to %s:%d | profile=%s sequence=%d",
                sent,
                config.tab5_host,
                config.tab5_port,
                config.profile_id,
                sequence,
            )

        if config.once:
            return

        sequence += 1
        time.sleep(config.interval_seconds)


def parse_args() -> RuntimeConfig:
    parser = argparse.ArgumentParser(
        description="Build and send Ambient Runtime context to the Tab5."
    )
    parser.add_argument(
        "--tab5-host",
        default=os.getenv("TAB5_HOST", ""),
        help="Tab5 IPv4 address. Environment: TAB5_HOST.",
    )
    parser.add_argument(
        "--tab5-port",
        type=int,
        default=int(os.getenv("TAB5_PORT", str(DEFAULT_TAB5_PORT))),
    )
    parser.add_argument(
        "--profile",
        default=os.getenv("AMBIENT_PROFILE", "claudio"),
        help="claudio, herminio, mariana, student or visitor.",
    )
    parser.add_argument(
        "--latitude",
        type=float,
        default=float(os.getenv("AMBIENT_LATITUDE", str(DEFAULT_LATITUDE))),
    )
    parser.add_argument(
        "--longitude",
        type=float,
        default=float(os.getenv("AMBIENT_LONGITUDE", str(DEFAULT_LONGITUDE))),
    )
    parser.add_argument(
        "--location",
        default=os.getenv("AMBIENT_LOCATION", DEFAULT_LOCATION_LABEL),
    )
    parser.add_argument(
        "--ttl",
        type=int,
        default=int(os.getenv("AMBIENT_TTL_SECONDS", str(DEFAULT_TTL_SECONDS))),
    )
    parser.add_argument(
        "--interval",
        type=int,
        default=int(os.getenv("AMBIENT_INTERVAL_SECONDS", "900")),
    )
    parser.add_argument("--once", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable debug logging.",
    )

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
        format="%(asctime)s %(levelname)s %(name)s: %(message)s",
    )

    if not args.dry_run and not args.tab5_host:
        parser.error("--tab5-host or TAB5_HOST is required unless --dry-run is used")

    if not (1 <= args.tab5_port <= 65535):
        parser.error("Tab5 UDP port must be between 1 and 65535")

    if args.ttl <= 0 or args.interval <= 0:
        parser.error("TTL and interval must be positive")

    return RuntimeConfig(
        tab5_host=args.tab5_host,
        tab5_port=args.tab5_port,
        profile_id=args.profile,
        latitude=args.latitude,
        longitude=args.longitude,
        location_label=args.location,
        ttl_seconds=args.ttl,
        interval_seconds=args.interval,
        once=args.once,
        dry_run=args.dry_run,
    )


def main() -> int:
    try:
        run(parse_args())
        return 0
    except KeyboardInterrupt:
        LOG.info("Stopped by user")
        return 130
    except Exception:
        LOG.exception("Ambient context service failed")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())

def send_ambient_context(
    profile_id: str,
    tab5_host: str,
    tab5_port: int = DEFAULT_TAB5_PORT,
):
    """
    Builds and immediately sends one Ambient Context packet.

    This helper is intended to be called directly from the Cognitive Runtime
    after an authenticated identity becomes the current context.
    """

    config = RuntimeConfig(
        tab5_host=tab5_host,
        tab5_port=tab5_port,
        profile_id=profile_id,
        latitude=DEFAULT_LATITUDE,
        longitude=DEFAULT_LONGITUDE,
        location_label=DEFAULT_LOCATION_LABEL,
        ttl_seconds=DEFAULT_TTL_SECONDS,
        interval_seconds=DEFAULT_TTL_SECONDS,
        once=True,
        dry_run=False,
    )

    payload = build_ambient_context(config, sequence=1)
    send_udp(payload, tab5_host, tab5_port)