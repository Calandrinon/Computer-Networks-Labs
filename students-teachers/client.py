import socket, sys, time, random
import threading 

"""
Students

Each “student” will read from the command line a port number group_port and an integer that states if the student is the group leader (1) or not (0).
The group leader periodically (from 5 to 5 seconds) sends the message “leader” the port group_port using broadcast to notify the students that he/she is the group leader. 
The other students will send their questions (a string), randomly from 3 to 3 seconds to the group leader (i.e. from 3 to 3 seconds, generate a random number between 0 and 1; if it is larger than 0.5 compose a random string and send it; otherwise do nothing). You can choose if you want to use TCP or UDP for this.
The group leader will “forward” these questions to the teacher using TCP. When the teacher responds, the group leader will send the original question and the teacher’s response on the port group_port using broadcast.

Teacher

The teacher is the “server”; using select and TCP communication it waits for questions from the group leaders. The group leaders know the IP and the port that the teacher listens on.
When a question is received, the sever generates the response (a string and an array of integers – the string and the values in this array should be randomly generated) and sends back the response to the group leader. 
Create at least 3 groups with minimum 5 students each.
"""


group_port = int(input("Enter the group port: "))
is_leader = int(input("Are you the group leader? No(0)/Yes(1)"))


def leader():
    try:
        socket_fd_classmates = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        socket_fd_classmates.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    except Exception as e:
        print("Socket_fd_classmates creation failed: ", e)

    try:
        socket_fd_teacher = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socket_fd_teacher.connect(("192.168.1.13", 9999))
    except Exception as e:
        print("Socket_fd_teacher creation failed: ", e)

    try:
        listener_classmates = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        listener_classmates.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        listener_classmates.bind(('', group_port))
    except Exception as e:
        print("Classmates-listener socket creation failed: ", e)

    try:
        listener_teacher = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        listener_teacher.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        listener_teacher.bind(('', 9999))
        listener_teacher.listen(10)
    except Exception as e:
        print("Teacher-listener socket creation failed: ", e)


    def notification_sending_thread():
        while True:
            time.sleep(5)
            print("Leader thread sleep done.")
            message = "leader"
            socket_fd_classmates.sendto(message.encode(), ('255.255.255.255', group_port))
            print("Sent notification to classmates.")

    def classmates_listener_thread():
        while True:
            (data, address) = listener_classmates.recvfrom(1024)
            print("Received question from a classmate: {} {}".format(data.decode(), address))
            socket_fd_teacher.send(data)
            answer_from_teacher = socket_fd_teacher.recv(1024).decode()
            broadcast_answer_for_classmates = "Original question: " + data.decode() + "; Teacher's answer: " + answer_from_teacher
            socket_fd_classmates.sendto(broadcast_answer_for_classmates.encode(), ('255.255.255.255', group_port))
            print("Sent answer from teacher to classmates: {}".format(broadcast_answer_for_classmates))

    print("Group leader is online.")

    classmates_listener = threading.Thread(target=classmates_listener_thread)
    classmates_listener.start()
    notification_sender = threading.Thread(target=notification_sending_thread)
    notification_sender.start()



def regular_student():
    try:
        socket_fd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except Exception as e:
        print("Socket_fd creation failed: ", e)

    try:
        listener = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        listener.bind(('', group_port))
        listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    except Exception as e:
        print("Listener creation failed: ", e)

    group_leader_address = ""
    mutex = threading.Lock()

    def compose_random_string():
        string_length = random.randint(3, 10)
        random_string = ""
        for _ in range(0, string_length):
            random_string += chr(ord('a') + random.randint(0, 25))

        return random_string 


    def question_sender_thread():
        while True:
            time.sleep(3)
            print("Question thread sleep over.")
            random_float = random.random()
            if random_float >= 0.5:
                message = compose_random_string()

                mutex.acquire()
                group_leader_address_copy = group_leader_address
                mutex.release()

                socket_fd.sendto(message.encode(), (group_leader_address_copy, group_port))
                print("Sent a question to the group leader: {} {}".format(message, (group_leader_address_copy, group_port)))


    no_leader_address = True
    print("Question sender thread started...")
    question_sender = threading.Thread(target=question_sender_thread)
    question_sender.start()
    
    print("Question sender listener started...")
    while True:
        (data, address) = listener.recvfrom(1024)
        if address[0] == "127.0.0.1":
            print("Received message from myself: {}".format(data.decode()))
            continue
        print("Received forwarded answer from the group leader: {} {}".format(data.decode(), address))

        mutex.acquire()
        group_leader_address_copy = group_leader_address
        mutex.release()

        if no_leader_address and data.decode() == "leader":
            mutex.acquire()
            group_leader_address = address
            mutex.release()
            print("Got the leader's address: {}".format(group_leader_address))
            group_leader_address = group_leader_address[0]
            no_leader_address = False
        elif address == group_leader_address_copy:
            print("Got a response: {}".format(data.decode()))



if is_leader:
    leader()
else:
    regular_student()