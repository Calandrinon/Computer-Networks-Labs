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

// The client sends to the server an integer. The server returns the list of divisors for the specified number.

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("192.168.1.8");

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket could not be created.\n");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection with the server failed..\n");
        return -1;
    }

    int32_t number, number_of_divisors;
    printf("Enter a number and find out its divisors: ");
    scanf("%d", &number);
    number = htonl(number);

    send(socket_fd, &number, sizeof(number), 0);
    printf("The divisors of the entered number are:\n");

    recv(socket_fd, &number_of_divisors, sizeof(number_of_divisors), 0);
    number_of_divisors = ntohl(number_of_divisors);
    
    for (int i = 0; i < number_of_divisors; i++) {
        recv(socket_fd, &number, sizeof(number), 0);
        number = ntohl(number);
        printf("%d ", number);
    }

    printf("\n");
    close(socket_fd);
    return 0;
}