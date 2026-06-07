# Cliente Python para StackFlow no AX630C

## `client.py` comentado e explicado em português

Projeto: **Ambient Physical AI**  
Módulo: **AX630C + StackFlow**  
Modelo validado: **Qwen2.5-0.5B-prefill-20e**  
Interface validada: **TCP 10001 / JSON over TCP**

---

# 1. Objetivo deste cliente

Este cliente Python demonstra como conversar diretamente com o runtime local do AX630C sem usar aplicação oficial da M5Stack.

O fluxo validado foi:

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

O cliente faz:

1. Abre conexão TCP com o StackFlow Runtime.
2. Envia uma mensagem `setup` para criar uma sessão LLM.
3. Recebe um `work_id` dinâmico, por exemplo `llm.1001`.
4. Envia um prompt para a LLM.
5. Recebe a resposta em streaming.
6. Envia `exit` para encerrar a sessão.

---

# 2. Código fonte completo comentado

Salve este conteúdo como:

```text
/root/client.py
```

no AX630C.

```python
# Importa o módulo socket.
# O socket permite abrir uma conexão TCP diretamente com o runtime StackFlow.
import socket

# Importa o módulo json.
# O protocolo descoberto usa mensagens JSON enviadas sobre TCP.
import json


# Endereço do servidor StackFlow.
# Como este script roda dentro do próprio AX630C, usamos localhost.
HOST = "127.0.0.1"

# Porta TCP onde o processo llm_sys está escutando.
# Esta porta foi descoberta com o comando:
# ss -lntp
PORT = 10001

# Nome do modelo LLM instalado no AX630C.
# Este modelo foi identificado em:
# /opt/m5stack/data/qwen2.5-0.5B-prefill-20e/
MODEL = "qwen2.5-0.5B-prefill-20e"


def send_json(sock, payload):
    """
    Envia uma mensagem JSON para o StackFlow Runtime.

    Parâmetros:
    - sock: socket TCP já conectado.
    - payload: dicionário Python que será convertido para JSON.

    Observação importante:
    O protocolo usa JSON sobre TCP com terminador de linha.

    Por isso enviamos:

        json + "\n"

    Sem o "\n", o servidor pode ficar aguardando o fim da mensagem.
    """

    # Converte o dicionário Python para string JSON.
    # ensure_ascii=False permite enviar caracteres Unicode corretamente.
    message = json.dumps(payload, ensure_ascii=False)

    # Adiciona o terminador de mensagem exigido pelo protocolo.
    message = message + "\n"

    # Converte a string para bytes UTF-8 e envia pelo socket.
    sock.sendall(message.encode("utf-8"))


def recv_json(sock):
    """
    Recebe uma mensagem JSON do StackFlow Runtime.

    O servidor envia respostas terminadas por "\n".
    Esta função lê dados do socket até encontrar uma quebra de linha.

    Retorno:
    - Um dicionário Python, se uma mensagem JSON válida for recebida.
    - None, se a conexão for encerrada.
    """

    # Buffer temporário onde acumulamos os dados recebidos.
    buf = ""

    # Continua recebendo até encontrar o terminador de mensagem.
    while "\n" not in buf:
        # Recebe até 4096 bytes do socket.
        chunk = sock.recv(4096)

        # Se não vier nenhum dado, a conexão foi encerrada.
        if not chunk:
            return None

        # Decodifica os bytes recebidos para texto.
        # errors="replace" evita falha caso algum caractere venha inválido.
        buf += chunk.decode("utf-8", errors="replace")

    # Divide no primeiro "\n".
    # line contém uma mensagem JSON completa.
    line, _ = buf.split("\n", 1)

    # Converte o JSON textual para dicionário Python.
    return json.loads(line)


def main():
    """
    Função principal do cliente.

    Ela executa o ciclo completo:

    1. Conectar
    2. Setup da LLM
    3. Enviar prompt
    4. Receber resposta em streaming
    5. Encerrar sessão
    """

    # Cria um socket TCP IPv4.
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Define timeout para evitar que o script fique preso indefinidamente.
    # Se o servidor não responder em 30 segundos, ocorrerá exceção.
    s.settimeout(30)

    # Conecta ao runtime StackFlow local.
    s.connect((HOST, PORT))


    # ------------------------------
    # 1. SETUP DA SESSÃO LLM
    # ------------------------------

    # Antes de inferir, precisamos criar uma sessão.
    # O work_id inicial é "llm".
    # O runtime responderá com um novo work_id dinâmico, como "llm.1001".
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

    # Envia a mensagem de setup.
    send_json(s, setup)

    # Aguarda a resposta do runtime.
    setup_resp = recv_json(s)

    # Mostra a resposta completa do setup.
    print("SETUP:", setup_resp)

    # Extrai o work_id real retornado pelo runtime.
    # Exemplo: "llm.1001"
    work_id = setup_resp["work_id"]


    # ------------------------------
    # 2. ENVIO DO PROMPT
    # ------------------------------

    # Mensagem de inferência.
    #
    # O campo "delta" contém o texto enviado para a LLM.
    # O campo "finish": True indica que este é o fim da entrada.
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

    # Envia o prompt para a LLM.
    send_json(s, inference)


    # ------------------------------
    # 3. RECEBIMENTO DA RESPOSTA
    # ------------------------------

    print("RESPONSE:", end=" ", flush=True)

    # O runtime responde em streaming.
    # Portanto, recebemos vários pacotes JSON.
    while True:
        # Lê uma mensagem JSON do socket.
        resp = recv_json(s)

        # Se não houver resposta, encerra.
        if resp is None:
            break

        # O campo "data" contém o fragmento da resposta.
        data = resp.get("data", {})

        # "delta" é o pedaço de texto gerado pela LLM.
        delta = data.get("delta", "")

        # "finish" indica se a resposta terminou.
        finish = data.get("finish", False)

        # Imprime o fragmento sem quebrar linha.
        print(delta, end="", flush=True)

        # Quando finish=True, a resposta terminou.
        if finish:
            break

    # Quebra de linha final.
    print()


    # ------------------------------
    # 4. ENCERRAMENTO DA SESSÃO
    # ------------------------------

    # Envia mensagem de saída.
    # Isso informa ao runtime que a sessão pode ser encerrada.
    exit_msg = {
        "request_id": "exit_client_001",
        "work_id": work_id,
        "action": "exit"
    }

    # Envia o comando exit.
    send_json(s, exit_msg)

    # Fecha o socket TCP.
    s.close()


# Ponto de entrada do script.
if __name__ == "__main__":
    main()
```

---

# 3. Como executar

No shell do AX630C:

```bash
python3 /root/client.py
```

Resultado esperado:

```text
SETUP: {'created': ..., 'error': {'code': 0, 'message': ''}, 'work_id': 'llm.1001'}
RESPONSE: Hello! How can I assist you today?
```

---

# 4. Explicação do protocolo

## Porta

```text
10001/TCP
```

## Processo responsável

```text
llm_sys
```

## Protocolo

```text
JSON sobre TCP
```

## Terminador de mensagem

```text
\n
```

Cada mensagem enviada ao runtime precisa terminar com quebra de linha.

---

# 5. Fluxo lógico

```text
1. connect()
2. setup
3. receber work_id
4. inference
5. receber streaming
6. exit
7. close()
```

---

# 6. O que é o `work_id`?

O `work_id` identifica uma sessão dentro do runtime.

O primeiro setup usa:

```json
"work_id": "llm"
```

O runtime responde com algo como:

```json
"work_id": "llm.1001"
```

Depois disso, todos os comandos de inferência precisam usar esse novo `work_id`.

---

# 7. O que é streaming?

A resposta não chega inteira de uma vez.

Ela chega em pedaços:

```json
{"delta":"Hello! How","finish":false}
{"delta":" can I assist","finish":false}
{"delta":" you today?","finish":false}
{"delta":"","finish":true}
```

O cliente precisa juntar os campos `delta` até receber:

```json
"finish": true
```

---

# 8. Como trocar o prompt

No código, altere:

```python
"delta": "Hello"
```

por exemplo:

```python
"delta": "Explain what MQTT is in one sentence."
```

---

# 9. Cuidados importantes

## Usar timeout

O socket usa:

```python
s.settimeout(30)
```

Isso evita que o script fique preso indefinidamente.

## Sempre enviar `\n`

Sem quebra de linha, o runtime pode não processar a mensagem.

## Sempre fazer setup antes da inferência

Inferência direta sem setup retornou erro:

```text
inference data push false
```

---

# 10. Conclusão

Este cliente prova que o AX630C pode atuar como um servidor local de IA.

A partir dele, futuros nós do projeto Ambient Physical AI poderão conversar com o runtime cognitivo via TCP, incluindo:

- CoreS3 Lite
- StackChan
- PoE-P4
- Voice Node
- Ambient Node

Este é o primeiro cliente funcional validado para comunicação direta com o StackFlow Runtime.
