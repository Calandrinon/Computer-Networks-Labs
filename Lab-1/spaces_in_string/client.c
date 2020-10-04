#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#define BUF_LEN 1024

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

	/// This is currently just a test to see if sending and receiving a string over a network works.	
	char message[BUF_LEN];
	fgets(message, BUF_LEN, stdin);
	send(socket_fd, message, BUF_LEN, 0);
	recv(socket_fd, message, BUF_LEN, 0);	
	printf("Number of spaces in your message: %s\n", message);
	close(socket_fd);
	return 0;
}
