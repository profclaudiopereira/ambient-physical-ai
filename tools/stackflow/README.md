# StackFlow Python Examples

Ambient Physical AI — AX630C + StackFlow Lab

These scripts document the evolution of the Python clients used during StackFlow Protocol Discovery and Context Discovery.

## Files

| File | Purpose |
|---|---|
| `v00_ping_test.py` | Validate TCP 10001 and `llm_sys` with `work_id=sys` / `action=ping`. |
| `v01_setup_test.py` | Create an LLM session and retrieve a dynamic `work_id`, such as `llm.1000`. |
| `v02_client.py` | First complete functional client: `setup -> inference -> streaming -> exit`. |
| `v03_client_context.py` | Multi-turn/context experiment. This may cause timeout depending on runtime state. |
| `v04_context_injection.py` | Safe one-shot context injection pattern. |

## Recommended execution order

```bash
python3 v00_ping_test.py
python3 v01_setup_test.py
python3 v02_client.py
python3 v04_context_injection.py
```

Run `v03_client_context.py` carefully because multi-turn tests produced timeouts during the lab.

## Runtime recovery

If the runtime stops responding:

```bash
systemctl restart llm-llm
systemctl restart llm-sys
```

Then validate with:

```bash
python3 v00_ping_test.py
```

## Stable pattern discovered

```text
setup
 ↓
single inference
 ↓
streaming response
 ↓
exit
```

## Unstable pattern observed

```text
setup
 ↓
multiple inference requests
 ↓
timeout
```

## Architectural recommendation

For Ambient Physical AI, each event should carry a full context package:

```text
Event
 ↓
Full Context Package
 ↓
Single Inference
 ↓
Response
```
