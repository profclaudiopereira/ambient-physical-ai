# Ambient Context Service

## Ambient Physical AI

### Cognitive Runtime service for the Tab5 Mini OLED

This package completes the server-side data path for the Mini OLED:

```text
External context sources
        ↓
Ambient Context Service — AX630C
        ↓ normalized ambient_context JSON
UDP 5555
        ↓
semantic_event_receiver — Tab5
        ↓
ambient_context_snapshot
        ↓
oled_context_presenter
        ↓
M5Stack Unit OLED / SH1107
```

The implementation is native Python 3 and uses only the standard library. It
does not add Arduino code or external Python dependencies.

---

## Architectural responsibility

The Cognitive Runtime is responsible for:

- selecting external data sources;
- applying profile relevance;
- handling API credentials;
- reducing external payloads;
- normalizing the result;
- transmitting one stable contract to the Ambient Runtime.

The Tab5 remains responsible only for:

- receiving the normalized context;
- checking freshness through `ttl_seconds`;
- formatting the content;
- presenting it on the Mini OLED.

The Tab5 does not know which API produced the information.

---

## Content policy

### Global context

The same information is available to every profile:

- Recife location;
- current temperature;
- current weather condition;
- today's maximum UV index;
- UV risk label.

Source:

```text
Open-Meteo Forecast API
```

One request returns current conditions and daily forecast variables.

### Personal context

| Profile | Adapter | OLED content |
|---|---|---|
| `claudio` | AwesomeAPI | USD/BRL quotation and daily percentage change |
| `herminio` | TheSportsDB | next Sport Club do Recife fixture |
| `mariana` | TMDB | one movie currently listed as now playing in Brazil |
| `student` | Steam Storefront | one featured discounted game |
| `visitor` | none | global context only |

The Mariana adapter currently represents **Brazilian now-playing cinema data**.
It is not a guaranteed Recife theater schedule. A local cultural-agenda adapter
can replace it later without changing the Tab5 contract.

---

## Repository destination

Recommended location:

```text
runtime/cognitive/stackflow/services/ambient_context/
```

Files:

```text
ambient_context/
├── ambient_context_service.py
├── send_test_context.py
├── .env.example
├── ambient-context.service
└── README.md
```

No new top-level repository directory is required.

---

## Contract sent to the Tab5

```json
{
  "type": "ambient_context",
  "sequence": 1,
  "ttl_seconds": 900,
  "authenticated": true,
  "profile_id": "claudio",
  "global": {
    "available": true,
    "location": "RECIFE",
    "weather_summary": "PARTLY CLOUDY",
    "temperature_c": 28.4,
    "uv_index": 7.1,
    "uv_label": "HIGH"
  },
  "personal": {
    "available": true,
    "title": "USD BRL",
    "value": "R$ 5.42",
    "secondary": "+0.31%"
  }
}
```

This matches the existing `ambient_context_snapshot_t` contract.

---

## Installation on AX630C

Create the destination:

```bash
mkdir -p /root/ambient-runtime/runtime/cognitive/stackflow/services/ambient_context
```

Copy this package into that directory.

Set execution permissions:

```bash
chmod +x ambient_context_service.py
chmod +x send_test_context.py
```

Create the runtime environment file:

```bash
cp .env.example .env
nano .env
```

Set the fixed Tab5 IP:

```bash
TAB5_HOST=192.168.77.XX
```

Load the environment:

```bash
set -a
. ./.env
set +a
```

---

## Validation sequence

### 1. Validate the Tab5 contract without external APIs

```bash
python3 send_test_context.py 192.168.77.XX --profile claudio
```

Repeat:

```bash
python3 send_test_context.py 192.168.77.XX --profile herminio
python3 send_test_context.py 192.168.77.XX --profile mariana
python3 send_test_context.py 192.168.77.XX --profile student
```

Expected Tab5 log:

```text
Received ... bytes
Ambient context consumed | Seq=1 Profile=... Global=YES Personal=YES
```

Expected OLED behavior:

```text
global weather block
+
profile-specific block
```

### 2. Validate external sources without transmitting

```bash
python3 ambient_context_service.py --profile claudio --once --dry-run
python3 ambient_context_service.py --profile herminio --once --dry-run
python3 ambient_context_service.py --profile student --once --dry-run
```

Mariana requires a TMDB bearer token:

```bash
export TMDB_BEARER_TOKEN="..."
python3 ambient_context_service.py --profile mariana --once --dry-run
```

### 3. Send a live context to the Tab5

```bash
python3 ambient_context_service.py \
  --tab5-host 192.168.77.XX \
  --profile claudio \
  --once
```

### 4. Continuous mode

```bash
python3 ambient_context_service.py \
  --tab5-host 192.168.77.XX \
  --profile claudio \
  --interval 900
```

---

## Profile switching

For the end-to-end journey, the authenticated identity should provide the
`profile_id`. This isolated service accepts `--profile` so each adapter can be
validated independently before being attached to the existing identity event
flow.

Examples:

```bash
--profile claudio
--profile herminio
--profile mariana
--profile student
--profile visitor
```

---

## Failure behavior

External-source failure does not terminate the complete context:

- weather failure produces `global.available = false`;
- personal-source failure produces `personal.available = false`;
- the remaining valid section is still transmitted;
- errors are recorded in the AX630C log;
- no API error details are exposed to the Tab5.

This is deliberate graceful degradation.

---

## Optional systemd installation

Edit `.env` first, then:

```bash
cp ambient-context.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable ambient-context.service
systemctl start ambient-context.service
systemctl status ambient-context.service
```

The provided service starts with `--profile claudio`. Dynamic profile selection
should be connected to the Cognitive Runtime before enabling it permanently.

---

## Acceptance criteria

The laboratory may be closed when all of these are true:

- SH1107 text geometry validated on hardware;
- deterministic test JSON received on UDP 5555;
- global weather displayed;
- Claudio currency context displayed;
- Herminio sports context displayed or gracefully unavailable;
- Mariana cinema context displayed with configured TMDB token;
- student Steam context displayed;
- no changes made to unrelated Ambient Runtime components;
- documentation committed;
- working tree clean after push.
