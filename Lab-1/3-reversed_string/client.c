#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUF_SIZE 2048

// A client sends to the server a string. The server returns the reversed string to the client (characters from the end to the beginning).

int main() {
    struct sockaddr_in server_address;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        printf("Socket could not be created!\n");
        return 0;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("192.168.1.13");

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Could not connect to the server!\n");
        return 0;
    }

    char some_string[BUF_SIZE];
    printf("Enter a string:");
    fgets(some_string, BUF_SIZE, stdin);
    send(socket_fd, some_string, BUF_SIZE, 0);
    recv(socket_fd, some_string, BUF_SIZE, 0);
    printf("%s\n", some_string);
    close(socket_fd);
    return 0;
}
