#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>

int main() {
	//signal();

	struct sockaddr_in socket_str;
	int socket_descriptor;
	socket_str.sin_family = AF_INET;
	socket_str.sin_port = htons(6092);
	inet_aton("127.0.0.1", &socket_str.sin_addr);

	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);	
	if (socket_descriptor == -1) {
		printf("Socket could not be created!\n");
		return 0;
	}
	
	if (bind(socket_descriptor, (struct sockaddr*)&socket_str, sizeof(socket_str)) == -1) {	
		printf("Binding could not be established!\n");
		return 0;
	}
	
	if (listen(socket_descriptor, 10) == -1) {
		printf("Listening failed...\n");
		return 0;
	}
	printf("Listening...\n");

	while (true) {
		struct sockaddr_in client;
		int sizeof_client = sizeof(client);
		int client_connection = accept(socket_descriptor, (struct sockaddr*)&client, &sizeof_client);
		int number = 1, array_size = 4, number_of_elements = 0, sum = 0;
		int err = errno;	
		
		if (client_connection < 0) {
			printf("Client connection failed! %d\n", err);
			continue;
		}
		
		printf("Incoming client from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		while (number != 0) {
			printf("%d\n", number);
			printf("!!!In the 2nd while loop...\n");
			int result = recv(client_connection, &number, sizeof(number), 0);	

			if (result != sizeof(number)) {
				printf("Could not receive the number\n");
				continue;
			}
			
			//number = ntohs(number);
			printf("element=%d\n", number);
			sum += number;
		}
		
		int result = send(client_connection, &sum, sizeof(sum), 0);
		if (result < 0) {
			printf("Error sending the result...\n");
			continue;
		}
		
		close(client_connection);
	}

	return 0;
}
