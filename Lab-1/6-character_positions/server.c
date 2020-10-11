#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#else
#include <WinSock2.h>
#endif
#define STR_LEN 1024

int main() {
    #ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("The Winsock API could not be initialized.\n");
        return 0;
    }
    #endif
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("The socket could not be created.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding the socket to the specified address and port failed.\n");
        return 0;
    }

    if (listen(socket_fd, 10) < 0) {
        printf("Listening to connections failed.\n");
        return 0;
    }

    printf("Listening...\n");

    while (true) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        int client_socket = accept(socket_fd, (struct sockaddr*)&client_address, &address_size);

        if (client_socket < 0) {
            printf("Client connection failed.\n");
            continue;
        }

        printf("%s:%d connected.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        char string[STR_LEN], character;        
        int32_t positions[STR_LEN];
        recv(client_socket, string, STR_LEN, 0);
        recv(client_socket, &character, sizeof(char), 0);

        int32_t number_of_occurences = 0;
        for (int i = 0; i < strlen(string); i++) {
            if (string[i] == character) {
                positions[number_of_occurences] = i;
                number_of_occurences++;
            }
        }

        int32_t host_number_of_occurences = number_of_occurences; 
        number_of_occurences = htonl(number_of_occurences);
        send(client_socket, &number_of_occurences, sizeof(number_of_occurences), 0);
        if (host_number_of_occurences == 0) {
            continue;
        }
        
        for (int i = 0; i < host_number_of_occurences; i++) {
            send(client_socket, &positions[i], sizeof(positions[i]), 0);
        } 

        #ifdef WIN32
        closesocket(client_socket);
        #else
        close(client_socket);
        #endif
    }
    
    #ifdef WIN32
    closesocket(socket_fd);
    WSACleanup();
    #else
    close(socket_fd);
    #endif

    return 0;
}