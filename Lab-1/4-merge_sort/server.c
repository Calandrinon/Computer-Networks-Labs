#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
#define BUF_LEN 1024

int main() {
    #ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) < 0) {
        printf("Could not initialize WinSock API...\n");
        return 0;
    }
    #endif

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        printf("Cannot create socket.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Cannot create socket.\n");
        return 0;
    }

    if (listen(socket_fd, 10) < 0) {
        printf("Cannot listen on the specified socket.\n");
        return 0;
    }

    printf("Listening...\n");


    while (true) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        int client_connection = accept(socket_fd, (struct sockaddr*)&client_address, &address_size);

        if (client_connection < 0) {
            printf("Can't connect with the client\n");
            return 0;
        }

        printf("%s:%d connected.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        char first_message[BUF_LEN], second_message[BUF_LEN];
        recv(client_connection, first_message, BUF_LEN, 0);
        recv(client_connection, second_message, BUF_LEN, 0);

        printf("First message: %s\n", first_message);
        printf("Second message: %s\n", second_message);

        #ifdef WIN32
        closesocket(client_connection);
        #else
        close(client_connection);
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