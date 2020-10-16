import socket, struct, sys 
from os import path
from multiprocessing import Process
import signal
PATH = 256

def request_handler(client_socket):
    file_path = client_socket.recv(PATH).decode().strip(chr(0)).strip(chr(10))
    print("File path: {}".format(file_path))

    if not path.isfile(file_path):
        print("File doesn't exist or it is a directory")
        client_socket.send(struct.pack('!i', -1))
        return

    file_size = path.getsize(file_path)
    client_socket.send(struct.pack('!i', file_size))
    file_contents = ""

    with open(file_path, "r") as f:
        lines = f.readlines()
        for line in lines:
            file_contents += line

    client_socket.send(file_contents.encode())
    client_socket.close()


def main():
    signal.signal(signal.SIGCHLD, signal.SIG_IGN)

    try:
        socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except Exception as e:
        print("Socket could not be created: {}".format(e))
        sys.exit()

    try:
        socket_fd.bind(("0.0.0.0", 1234))
    except Exception as e:
        print("Binding failed: {}".format(e))
        sys.exit()

    try:
        socket_fd.listen(10)
    except Exception as e:
        print("Listening failed: {}".format(e))
        sys.exit()

    print("Listening...")

    while True:
        (client_socket, client_address) = socket_fd.accept()

        process = Process(target=request_handler, args=(client_socket,))
        process.start()

main()
