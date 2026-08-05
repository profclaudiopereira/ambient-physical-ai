# TECHNICAL NOTE 004 — StackChan MCP Configuration and Validation

## Purpose

This note documents the operational configuration required for the StackChan MCP integration after a clean installation of the Ambient Physical AI Cognitive Runtime.

It preserves the validated architecture and records the deployment procedure used during the competition integration.

---

## Required Configuration

The Cognitive Runtime must start with the authenticated MCP endpoint available through the environment variable:

```text
STACKCHAN_MCP_URL=wss://api.xiaozhi.me/mcp/?token=<YOUR_TOKEN>
```

Never commit the token to Git.

After updating the runtime environment:

```bash
systemctl restart ambient-cognitive-runtime.service
```

---

## Expected Startup Validation

Successful startup should produce log messages similar to:

```text
Connecting to StackChan MCP broker
StackChan MCP connection established.
MCP initialize response sent.
MCP initialized notification received.
MCP tools/list response sent with 5 Tool(s).
```

---

## Functional Validation

Authenticate a user through the Identity Node and ask StackChan questions that require contextual information, for example:

- Who am I?
- Who is the current user?
- What is the current context?
- Which profile is active?

Monitor:

```bash
journalctl -u ambient-cognitive-runtime.service -f
```

Expected evidence:

```text
MCP Tool requested: ...
MCP Tool result sent: ...
```

---

## Engineering Notes

- The `StackChanNotifier` may remain in `dry-run`; this does not affect MCP integration.
- The validated production path is MCP Tool invocation through the XiaoZhi MCP Broker.
- Context Push remains future work.

---

## Conclusion

This procedure should be executed after any clean installation or service recreation to restore the validated StackChan MCP integration.
