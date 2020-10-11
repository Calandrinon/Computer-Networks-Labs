#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define STR_LEN 1024

// The client sends to the server a string and two numbers (s, I, l). 
// The server returns to the client the substring of s starting at index I, of length l.

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("192.168.1.10");

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket creation failed.\n");
        return 0;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection with the server failed.\n");
        return 0;
    }

    char string[STR_LEN];
    int32_t I, l;

    printf("Enter a string: ");
    fgets(string, STR_LEN, stdin);
    printf("Enter the index: ");
    scanf("%d", &I);
    printf("Enter the length of the substring: ");
    scanf("%d", &l);

    I = htonl(I);
    l = htonl(l);
    send(socket_fd, string, STR_LEN, 0);
    send(socket_fd, &I, sizeof(I), 0); 
    send(socket_fd, &l, sizeof(l), 0); 

    char substring[STR_LEN];
    recv(socket_fd, substring, STR_LEN, 0);
    printf("The substring is: %s\n", substring);
    return 0;
}