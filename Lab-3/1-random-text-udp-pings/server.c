#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUF_LEN 128

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s [port]\n", argv[0]);
        return 0;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("UDP socket creation failed: %s", strerror(errno));
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Error in binding the socket: %s\n", strerror(errno));
        return 0;
    }

    char buffer[BUF_LEN];
    memset(buffer, 0, BUF_LEN);

    int bytes_sent = sendto(socket_fd, buffer, BUF_LEN, 0, (struct sockaddr*)&server_address, sizeof(server_address));
    if (bytes_sent < 0) {
        printf("Message could not be sent: %s", strerror(errno));
        return 0;
    }

    while (true) {
        struct sockaddr_in client_address;
        int address_length = sizeof(client_address);
        int bytes_received = recvfrom(socket_fd, buffer, BUF_LEN, 0, (struct sockaddr*)&client_address, &address_length);
        if (bytes_received < 0) {
            printf("Error in receiving client message: %s\n", strerror(errno));
            continue;
        }

        bytes_received = sendto(socket_fd, buffer, BUF_LEN, 0, (struct sockaddr*)&client_address, address_length);
        if (bytes_received < 0) {
            printf("Error in sending message back to the client: %s\n", strerror(errno));
            continue;
        }
    }

    return 0;
}