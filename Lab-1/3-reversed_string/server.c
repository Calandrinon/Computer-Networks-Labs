#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif
#define BUF_SIZE 2048

char* reverse_string(char string[]) {
    char* reversed_string = malloc(BUF_SIZE);
    for (int i = strlen(string) - 1; i >= 0; i--) {
        reversed_string[strlen(string) - i - 1] = string[i]; 
    }

    return reversed_string;
}

int main() {
    #ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("Cannot initialize WinSock API.\n");
        return 0;
    }
    #endif

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd < 0) {
        printf("Socket creation failed.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding failed.\n");
        return 0;
    }

    if (listen(socket_fd, 10) < 0) {
        printf("Listening failed.\n");
        return 0;
    }

    printf("Listening...\n");

    while (true) {
        struct sockaddr_in client_address;
        int address_length = sizeof(client_address);
        int client_connection = accept(socket_fd, (struct sockaddr*)&client_address, &address_length);

        if (client_connection < 0) {
            printf("Could not connect with the client.\n");
            continue;
        }

        printf("Client %s:%d connected.\n", client_address.sin_addr, client_address.sin_port);

        char message[BUF_SIZE];
        recv(client_connection, message, BUF_SIZE, 0);
        printf("%s\n", message);
        char* reversed_message = reverse_string(message);
        send(client_connection, reversed_message, BUF_SIZE, 0);
        free(reversed_message);

        #ifdef WIN32
            closesocket(client_connection);
        #else
            close(client_connection);
        #endif
    }

    #ifdef WIN32
    WSACleanup();
    closesocket(socket_fd);
    #else
    close(socket_fd);
    #endif 

    return 0;
}