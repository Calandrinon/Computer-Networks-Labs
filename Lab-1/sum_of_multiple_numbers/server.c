#include <stdio.h>
#include <stdbool.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#else
#include <WinSock2.h>
#include <stdint.h>
#endif

int main() {
	//signal();
	#ifdef WIN32
       WSADATA wsaData;
       if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
              printf("Error initializing the Windows Sockets LIbrary");
              return -1;
       }
	#endif

	struct sockaddr_in socket_str;
	int socket_descriptor;
	socket_str.sin_family = AF_INET;
	socket_str.sin_port = htons(6092);
	socket_str.sin_addr.s_addr = INADDR_ANY;

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
		#ifdef WIN32
        	err = WSAGetLastError();
		#endif
		
		if (client_connection < 0) {
			printf("Client connection failed! %d\n", err);
			continue;
		}
		
		printf("Incoming client from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		while (number != 0) {
			int result = recv(client_connection, (char*)&number, sizeof(number), 0);	

			if (result != sizeof(number)) {
				printf("Could not receive the number\n");
				continue;
			}
			
			//number = ntohs(number);
			sum += number;
		}
		
		int result = send(client_connection, (char*)&sum, sizeof(sum), 0);
		if (result < 0) {
			printf("Error sending the result...\n");
			continue;
		}
		#ifdef WIN32	
			closesocket(client_connection);
		#else
			close(client_connection);
		#endif
	}

	#ifdef WIN32
       WSACleanup();
	#endif

	return 0;
}
