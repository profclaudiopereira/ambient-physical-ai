# DISCOVERY_REPORT_STACKFLOW_PROTOCOL_V1

## Ambient Physical AI
### Descoberta do Protocolo StackFlow no AX630C

Este relatório documenta a identificação bem-sucedida do protocolo do runtime StackFlow.

Resultados principais:
- Porta TCP 10001 validada
- JSON sobre TCP identificado
- llm_sys validado
- Inferência local do Qwen2.5 validada
- Cliente Python validado

Arquitetura:

TCP 10001 -> llm_sys -> llm_llm -> Qwen2.5-0.5B

Fluxo descoberto:
1. ping
2. setup
3. inference
4. resposta em streaming
5. exit

Primeira resposta:
Hello! How can I assist you today?

Conclusão:
O AX630C comporta-se como um servidor local de IA adequado para integração ao Ambient Physical AI.
