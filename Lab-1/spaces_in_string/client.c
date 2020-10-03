#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>

// A client sends to the server a string. The server returns the count of spaces in the string.

int main() {
	struct sockaddr_in socket_structure;
	socket_structure.sin_family = AF_INET;
	socket_structure.sin_port = htons(9999);
	inet_aton("192.168.1.13", &socket_structure.sin_addr);
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (socket_fd < 0) {
		printf("Error: Socket could not be created\n");	
		return 0;
	}

	if (connect(socket_fd, (struct sockaddr*)&socket_structure, sizeof(socket_structure)) < 0) {
		printf("Error: Could not establish a connection to the server.\n");	
		return 0;
	}
	
	char string_request[50];
	// First, we have to send the size of the string.
	printf("Send a string to the server and receive the number of spaces in the string.\n");
	printf("Enter a string: ");
	fgets(string_request, 50, stdin);
	send(socket_fd, string_request, sizeof(string_request), 0);	

	int number_of_spaces;
	recv(socket_fd, &number_of_spaces, sizeof(number_of_spaces), 0);	
	printf("The number of spaces is %d\n", number_of_spaces);
	return 0;
}
