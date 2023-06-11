#
# Importar as bibliotecas necessárias.
# socket - Interface de sockets para Python.
# threading - Permite suportar múltiplas atividades (threads) em cada programa.
# tkinter - Interface Python para o ambiente gráfico Tcl/Tk.
#
import socket
from threading import Thread
import tkinter
from tkinter import *


def receive():
    """
    Receber menssagens em ciclo e mostra-las numa janela tkinter.

    :return: None.
    """
    while True:
        try:
            msg = s.recv(1024).decode()        # Receber mensagem e descodificá-la como string.
            msg_list.insert(tkinter.END, msg)  # Inserir mensagem no final da janela.
        except Exception:
            print("There is an Error Receiving Message")

def send():
    """
    Obter mensagem do campo de input tkinter e enviá-la para o servidor.

    :return:
    """
    msg = my_msg.get()          # Obter mensagem do campo de input do tkinter.
    my_msg.set("")              # Limpar o campo de input para permitir nova mensagem.
    s.send(bytes(msg, "utf8"))  # Enviar mensagem codificada em utf-8 para o servidor.


window = Tk()
window.title("Chat Application")
window.configure(bg="green")

message_frame = Frame(window, height=100, width=100, bg='white')
message_frame.pack()

my_msg = StringVar()
my_msg.set("")

scroll_bar = Scrollbar(message_frame)
msg_list = Listbox(message_frame, height=15, width=100, bg="white", yscrollcommand=scroll_bar.set)
scroll_bar.pack(side=RIGHT, fill=Y)
msg_list.pack(side=LEFT, fill=BOTH)
msg_list.pack()

entry_field = Entry(window, textvariable=my_msg, fg="black", width=50)
entry_field.pack()

send_button = Button(window, text="Send", font="Aerial", fg="black", bg="blue", command=send)
send_button.pack()

host = "localhost"
port = 8080

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

receive_thread = Thread(target=receive)
receive_thread.start()

mainloop()
