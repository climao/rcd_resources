#
# Servidor TCP/IP.
#
# Demonstra a utilização de interface de sockets para criação de um servidor.
#
# Autor: Carlos Limão
#
import socket

PORT_NUMBER = 1050          # Número do porto (arbitrário >= 1024) usado pelo servidor


def read_file(filename):
    lines = []
    with open(filename) as f:
        #for line in f:
        #    lines.append(line.strip() + )
        content = f.read()
    return content


def server_program():
    # Obter nome do Host en que estamos a executar
    host = socket.gethostname()
    port = PORT_NUMBER

    server_socket = socket.socket()                 # Criação de um socket
    server_socket.bind(("127.0.0.1", PORT_NUMBER))  # Associar endereço do host ao porto

    server_socket.listen(2)                         # configurar ligações simultâneas que o servidor pode tratar.

    print(f"Servidor TCP/IP: À espera de ligações no porto {port} na máquina local ({host}) ...");

    msg_nr = 0
    while (True):
        msg_nr += 1
        conn, address = server_socket.accept()          # Aceitar ligação de um cliente
        print("Ligação desde: " + str(address))

        # Receber dados. 8192 é a maior mensagem que posso receber de uma vez
        data = conn.recv(8192).decode()
        if not data:
            # Se sair sem dados isso significa que servidor terminou a ligação
            break
        print("Recebido do cliente: " + str(data))
        # message = input(' -> ')
        message = f"{msg_nr}a mensagem do meu servidor"
        #content = f"<html><body><h1>{message}</h1></body></html>"
        #data = "HTTP/1.1 200 OK\r\nContent-type:  text/html\r\n" + "\r\n" + content;
        data = message
        conn.send(data.encode())  # send data to the client

        conn.close()  # Terminar ligação


if __name__ == '__main__':
    server_program()
