#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#define BUF_LEN 1024

// The client sends to the server two sorted arrays of chars. The server will merge sort the two arrays and return the result to the client.

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        printf("Socket cannot be created.\n");
        return 0;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Socket cannot be created.\n");
        return 0;
    }

    char first_message[BUF_LEN], second_message[BUF_LEN];
    printf("Enter the first string: ");
    fgets(first_message, BUF_LEN, stdin);
    printf("Enter the second string: ");
    fgets(second_message, BUF_LEN, stdin);
    send(socket_fd, first_message, BUF_LEN, 0);
    send(socket_fd, second_message, BUF_LEN, 0);

    char response[BUF_LEN];
    recv(socket_fd, response, BUF_LEN, 0);
    printf("The two strings, merged and sorted: %s\n", response);
    return 0;
}