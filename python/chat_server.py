#
# Importar as bibliotecas necessárias.
# socket - Interface de sockets para Python.
# threading - Permite suportar múltiplas atividades (threads) em cada programa.
#
import socket
from threading import Thread

# Definir IP e porto do servidor.
host = "localhost"
port = 8080

# Umq estrutura de dados do tipo "dicionário" para armazenar informação sobre ligações dos clientes.
clients = {}

# Criar socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Configurar socket para suportar múltiplos clientes ligados a um único porto.
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# Associar endereço IP e porto ao socket.
sock.bind((host, port))


def handle_clients(conn):
    """
    Obtem nome de um cliente a partir de um socket e envia-o para todos (broadcast/difusão).

    :param conn: socket que suporta ligação a um cliente.
    :return: None.
    """
    # Obter nome do cliente a partir da ligação.
    name = conn.recv(1024).decode()

    # Enviar mensagem de boas vindas ao cliente
    welcome = f"Welcome {name}. Good to see you :)"
    conn.send(bytes(welcome, "utf8"))

    # Gerar mensagem com indicação de que um novo cliente se ligou
    msg = name + " has recently joined us"

    # Enviar mensagem a todos os clientes já existentes.
    broadcast(bytes(msg, "utf8"))

    # Guardar informação do novo cliente no dicionário.
    clients[conn] = name

    # Ciclo de receção de mensagens do cliente e envio em difisão para os restantes.
    while True:
        msg = conn.recv(1024)
        broadcast(msg, name + ":")


def broadcast(msg, prefix=""):
    """
    Envar mensagem para todos os clientes ligados.

    :param msg: mensagem
    :param prefix: prefixo da mensagem
    :return:
    """
    for client in clients:  # Percorrer todos os cliente guardados no dicionário
        client.send(bytes(prefix, "utf8") + msg)


def accept_client_connection():
    """
    Aceitar pedidos de novos clientes.

    :return:
    """
    while True:  # accept client's request
        client_conn, client_address = sock.accept()  # Esperar ligação de um novo cliente
        print(client_address, " has Connected")

        # Enviar mensagem de boas vindas ao cliente e pedir-lhe o nome.
        client_conn.send(bytes("Welcome to the chat room, Please type your name to continue", "utf8"))

        # Iniciar a função handle_clients() como uma nova atividade (thread).
        Thread(target=handle_clients, args=(client_conn,)).start()


if __name__ == "__main__":
    # O servidor fica em espera ........
    sock.listen(3)  # Configurar socket para aceitar a ligação de três clientes em simultâneo.
    print("listening on port : ", port, "......")

    # Iniciar a espera por novos clientes como uma thread independente para permitir múltiplas conversas em simultâneo.
    t = Thread(target=accept_client_connection)

    t.start()  # Iniciar a thread
    t.join()   # Esperar até a thread principal teminar.
