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

int main() {
	struct sockaddr_in socket_str;
	socket_str.sin_family = AF_INET;
	socket_str.sin_port = htons(6092);
	inet_aton("127.0.0.1", &socket_str.sin_addr);
	
	int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_descriptor < 0) {
		printf("Error: socket could not be created.\n");
		return 0;
	}

	if (connect(socket_descriptor, (struct sockaddr*)&socket_str, sizeof(socket_str)) < 0) {	
		printf("Error: connection failed.\n");
		return 0;
	}

	printf("Enter a couple of numbers and get their sum.\n");
	printf("Each number is entered at once.\n");
	printf("To end the sequence of numbers, enter 0.\n");

	int array_size = 4;
	int number_of_elements = 0;
	int* array_of_numbers = malloc(sizeof(int)*array_size);

	while (true) {
		int number;
		printf(">>>");
		scanf("%d", &number);		

		send(socket_descriptor, (int*)&number, sizeof(int), 0);
		
		if (number == 0)
			break;
	}
	
	int sum;
	recv(socket_descriptor, (int*)&sum, sizeof(int), 0);	
	printf("The sum is: %d\n", sum);	
	return 0;
}
