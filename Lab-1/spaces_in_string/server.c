#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#include <stdint.h>
#endif

#define BUF_LEN 1024
int main() {

    #ifdef WIN32
        WSADATA wsaData;
        if (WSAStartup() < 0) {
            printf("Could not initialize WinSock API.\n");
            return 0;
        }
    #endif

    struct sockaddr_in socket_str;
    socket_str.sin_family = AF_INET;
    socket_str.sin_port = htons(9999);
    socket_str.sin_addr.s_addr = INADDR_ANY;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        printf("Could not create socket...\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&socket_str, sizeof(socket_str)) < 0) {
        printf("Could not bind socket to address and port...\n");
        return 0;
    }

    if (listen(socket_fd, 5) < 0) {
        printf("Could not listen on the specified port and address\n");
        return 0;
    }
    printf("Listening...\n");

    while (true) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        int client_connection = accept(socket_fd, (struct sockaddr*)&client_address, &address_size);

        if (client_connection < 0) {
            printf("Cannot connect with the client!\n");
            continue;
        } 

        printf("Connected with a client.\n");

        char message[BUF_LEN];
        recv(client_connection, message, BUF_LEN, 0);
        printf("%s\n", message);
        strcpy(message, "Hello, stranger!");
        send(client_connection, message, BUF_LEN, 0);

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