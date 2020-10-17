import socket, sys
BUF_SIZE = 2048

"""
    5.
    The client sends a domain name taken from the command line (Ex: www.google.com) to the server. 
    The server opens a TCP connection to the IP address corresponding to the received domain name on port 80 (called HTTP-Srv). 
    It sends on the TCP connection the string: “GET / HTTP/1.0\n\n” and relays the answer back to the client. 
    When HTTP-Srv closes connection to the server, the server closes the connection to the client at its turn. 
"""

def main():
    try:
        socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except Exception as e:
        print("Socket creation failed: {}\n".format(e))
        sys.exit(0)

    try:
        socket_fd.connect(("127.0.0.1", 9999))
    except Exception as e:
        print("Connection failed: {}\n".format(e))
        sys.exit(0)

    domain_name = input("Enter a domain name: ")
    socket_fd.send(domain_name.encode())
    response = ""

    print("Response: ")
    while True:
        response = socket_fd.recv(BUF_SIZE).decode("ISO-8859-1")
        if len(response) == 0:
            break

        print(response)

main()