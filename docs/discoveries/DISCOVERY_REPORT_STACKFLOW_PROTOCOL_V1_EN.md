# DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1

## Ambient Physical AI
### AX630C + StackFlow Protocol Discovery

This report documents the successful identification of the StackFlow runtime protocol.

Key Results:
- TCP Port 10001 validated
- JSON over TCP identified
- llm_sys validated
- Qwen2.5 local inference validated
- Python client validated

Architecture:

TCP 10001 -> llm_sys -> llm_llm -> Qwen2.5-0.5B

Workflow discovered:
1. ping
2. setup
3. inference
4. streaming response
5. exit

First response:
Hello! How can I assist you today?

Conclusion:
The AX630C behaves as a local AI server suitable for Ambient Physical AI integration.
