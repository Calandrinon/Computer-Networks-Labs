#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#define STR_LEN 2048

// The client sends to the server an integer. The server returns the list of divisors for the specified number.

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_family = AF_INET;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket could not be created.\n");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection with the server failed..\n");
        return -1;
    }

    int32_t number;
    char divisors_string[STR_LEN];
    printf("Enter a number and find out its divisors: ");
    scanf("%d", &number);
    number = htonl(number);

    send(socket_fd, &number, sizeof(number), 0);
    recv(socket_fd, &divisors_string, STR_LEN, 0);
    printf("The divisors of the entered number are: %s\n", divisors_string);

    close(socket_fd);
    return 0;
}