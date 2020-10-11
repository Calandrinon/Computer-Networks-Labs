#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
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
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket could not be created.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding the socket failed.\n");
        return 0;
    }

    if (listen(socket_fd, 10) < 0) {
        printf("Listening failed.\n");
        return 0;
    }

    printf("Listening...\n");

    while (true) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        int client_socket = accept(socket_fd, (struct sockaddr*)&client_address, &address_size);

        if (client_socket < 0) {
            printf("Could not establish connection with the client.\n");
            continue;
        }

        printf("%s:%d connected.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        
        char string[STR_LEN], substring[STR_LEN];
        int32_t I, l;
        recv(client_socket, string, STR_LEN, 0);
        recv(client_socket, &I, sizeof(I), 0);
        recv(client_socket, &l, sizeof(l), 0);

        I = ntohl(I);
        l = ntohl(l);
        memcpy(substring, &string[I], l);
        substring[l] = '\0';

        send(client_socket, substring, STR_LEN, 0);

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