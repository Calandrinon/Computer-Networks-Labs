import socket, sys, time, random
import threading, select
import pickle

try:
    socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except Exception as e:
    print("Socket creation failed: {}".format(e))

try:
    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    listener.bind(('', 9999))
    listener.listen(10)
except Exception as e:
    print("Listener creation failed: {}".format(e))


def compose_random_answer():
    random_string = ""
    array = []

    string_length = random.randint(3, 10)
    for _ in range(0, string_length):
        random_string += chr(ord('a') + random.randint(0, 25))
    
    array_length = random.randint(3, 10)
    for _ in range(0, array_length):
        array.append(random.randint(0, 100))

    return (random_string, array) 


master = read_fds = [listener]
print("Listening...")

while True:
    read_fds = master
    ready_sockets, _, _ = select.select(read_fds, [], [], 2.0)

    for fd in ready_sockets:
        if fd == listener:
            (client_connection, client_address) = listener.accept() 
            master.append(client_connection)
            print("Connected with a group leader ({})\n".format(client_address))
        else:
            # handle messages from clients(student group leaders)
            try:
                question = fd.recv(1024).decode() 
                print("Received question from group leader: {}".format(question))
                (string_answer, array_answer) = compose_random_answer()
                fd.send(string_answer.encode())
                fd.send(pickle.dumps(array_answer))
                print("Sent answer ({}, {}) to group leader.".format(string_answer, str(array_answer)))
            except Exception as e:
                print("ERROR: {}".format(e))
                fd.close()
                master.remove(fd)