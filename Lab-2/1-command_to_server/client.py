import socket, struct

"""
    1. The client takes a string from the command line and sends it to the server.
       The server interprets the string as a command with its parameters. It executes
       the command and returns the standard output and the exit code to the client.
"""

try:
    socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except Exception as e:
    print("Error creating socket: {}".format(e))

socket_fd.connect(("127.0.0.1", 9999))

command = input("Enter a command:")
socket_fd.send(command.encode())

response_size = socket_fd.recv(4)
response_size = struct.unpack('!I', response_size)[0]
print("response size: {}".format(response_size))
result = socket_fd.recv(response_size).decode()
print("Result:\n\n", result)
