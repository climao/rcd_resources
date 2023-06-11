#
# Cliente TCP.
#
# Demonstra a utilização de interface de sockets para acesso à rede.
#
# Autor: Carlos Limão
#
# Notas:
#     1. Este programa pressupõe que o programa servidor (tcp_serv.py) está a
#        executar na máquina cujo endereço IP está especificado em IP_ADDRESS.
#     2. Este programa estabelece uma ligação ao servidor e espera que este
#        lhe envie uma mensagem. Depois disso responde com uma mensagem para o
#        servidor.
#     3. Após a sequência descrita no passo anterior o programa termina.
#
import socket

PORT_NUMBER = 80            # Número do porto usado pelo servidor
IP_ADDRESS = "127.0.0.1"    # Endereço IP do servidor


def client_program():

    client_socket = socket.socket()                   # Criação de um socket
    client_socket.connect((IP_ADDRESS, PORT_NUMBER))  # Estabelecer ligação ao servidor

    message = input("Mensagem a enviar -> ")          # Obter mensagem a enviar do utilizador

    while message.lower().strip() != 'bye':
        if message.startswith('GET'):
            message += '\r\n\r\n'

        client_socket.send(message.encode())          # send message
        data = client_socket.recv(8192).decode()      # receive response

        print('Recebido do servidor: ' + data)        # Mostrar mensagem recebida no terminal

        message = input("Mensagem a enviar -> ")      # Repetir leitura da mensagem

    client_socket.close()                             # Terminar a ligação


if __name__ == '__main__':
    client_program()
    
