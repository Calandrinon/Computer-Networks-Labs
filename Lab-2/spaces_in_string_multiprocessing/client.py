import socket, struct, sys

"""
	A client sends to the server a string. The server returns the count of spaces in the string. 
	Write a concurrent process-based implementation.
"""

def main():
	try:
		socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	except Exception as e:
		print("Socket creation failed: {}".format(e))
		sys.exit()

	try:
		socket_fd.connect(("127.0.0.1", 9999))
	except Exception as e:
		print("Connection failed: {}".format(e))
		sys.exit()

	string = input("Enter a string: ")	
	socket_fd.send(string.encode())
	number_of_spaces = socket_fd.recv(4)
	number_of_spaces = struct.unpack('!I', number_of_spaces)
	print("The number of spaces is {}".format(number_of_spaces[0]))

	socket_fd.close()

main()
