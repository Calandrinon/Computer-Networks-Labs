#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

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
#define DOMAIN_NAME_LEN 100
#define BUF_LEN 2048

int lookup_host(char* host, char address[]) {
    struct hostent *result;

    result = gethostbyname(host);
    if (!result) {
        printf("Lookup Failed: %s\n", hstrerror(h_errno));
        return -1;
    }

    strcpy(address, inet_ntoa( (struct in_addr) *((struct in_addr *) result->h_addr_list[0])));
    return 0;
}


int main() {
    #ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("Windows socket API initialization failed.\n");
        return 0;
    }
    #endif

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_family = AF_INET;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket creation failed.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding failed.\n");
        perror(0);
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
            printf("Client connection failed.\n");
            continue;
        }

        char domain_name[DOMAIN_NAME_LEN];
        char address[100];
        recv(client_socket, domain_name, sizeof(domain_name), 0);

        if (lookup_host(domain_name, address) < 0) {
            close(client_socket);
            continue;
        }
        printf("Address: %s\n", address);

        struct sockaddr_in other_server_address;
        other_server_address.sin_family = AF_INET;
        other_server_address.sin_port = htons(80);
        other_server_address.sin_addr.s_addr = inet_addr(address);

        int connection_to_other_server = socket(AF_INET, SOCK_STREAM, 0);
        if (connection_to_other_server < 0) {
            printf("Socket for connecting to the other server failed.\n");
            char response[] = "Socket for connecting to the other server failed.\n"; 
            send(client_socket, response, sizeof(response), 0);
            continue;
        }

        if (connect(connection_to_other_server, (struct sockaddr*)&other_server_address, sizeof(other_server_address)) < 0) {
            printf("Connection to the server failed.\n");
            char response[] = "Connection to the server failed.\n"; 
            perror(0);
            send(client_socket, response, sizeof(response), 0);
            continue;
        }

        char request_string[] = "GET / HTTP/1.0\n\n";
        char buffer[BUF_LEN];
        send(connection_to_other_server, request_string, sizeof(request_string), 0);

        while (true) {
            memset(buffer, 0, BUF_LEN);
            int bytes_received = recv(connection_to_other_server, buffer, BUF_LEN, 0);

            if (bytes_received < 0) {
                printf("Can't read from socket.\n");
                perror(0);
                break;
            }

            if (!bytes_received) {
                /// Received all of the response.
                break;
            }

            send(client_socket, buffer, BUF_LEN, 0);
        }

        close(client_socket);
    }

    close(socket_fd);
    return 0;
}