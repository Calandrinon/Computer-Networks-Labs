import socket, struct, sys
from multiprocessing import Process
import signal

def handle_connection(client_socket):
    client_string = client_socket.recv(1024).decode()
    number_of_spaces = client_string.count(' ')
    client_socket.send(struct.pack('!I', number_of_spaces))

    client_socket.close()


def main():
    signal.signal(signal.SIGCHLD, signal.SIG_IGN)

    try:
        socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except Exception as e:
        print("Socket creation failed: {}".format(e))
        sys.exit()

    try:
        socket_fd.bind(("127.0.0.1", 9999))
    except Exception as e:
        print("Binding failed: {}".format(e))
        sys.exit()

    try:
        socket_fd.listen(20)
    except Exception as e:
        print("Binding failed: {}".format(e))
        sys.exit()

    print("Listening...")

    while True:
        (client_socket, client_address) = socket_fd.accept()
        print("{} connected.".format(client_address))
        
        process = Process(target=handle_connection, args=(client_socket,))
        process.start() 

main()