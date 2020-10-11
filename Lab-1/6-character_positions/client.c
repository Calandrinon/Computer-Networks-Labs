#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define STR_LEN 1024

// The client sends to the server a string and a character. 
// The server returns to the client a list of all positions in the string where the specified character is found. 

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("192.168.1.10");

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("The socket could not be created.\n");
        return 0;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection with the server could not be created.\n");
        return 0;
    }

    char string[STR_LEN], character; 
    printf("Enter a string:");
    fgets(string, STR_LEN, stdin);
    printf("Enter a character:");
    scanf("%c", &character);
    send(socket_fd, string, STR_LEN, 0);
    send(socket_fd, &character, sizeof(char), 0);

    int32_t number_of_positions, position;
    recv(socket_fd, &number_of_positions, sizeof(number_of_positions), 0);
    number_of_positions = ntohl(number_of_positions);

    if (!number_of_positions) {
        printf("The character has not been found in the string!\n");
        return 0;
    }
    printf("number of positions: %d\n", number_of_positions);
    printf("The positions where the character was found are: ");
    for (int i = 0; i < number_of_positions; i++) {
        recv(socket_fd, &position, sizeof(position), 0);
        printf("%d ", position);
        if (i < number_of_positions - 1)
            printf(", "); 
    }
    printf("\n");
    close(socket_fd);
    return 0;
}