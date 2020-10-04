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

int count_spaces(char text[]) {
    int number_of_spaces = 0;
    for (int i = 0; i < strlen(text); i++) {
        if (text[i] == ' ') {
            number_of_spaces++;
        }
    }

    return number_of_spaces;
}

char* convert_int_to_string(int number) {
    char* converted_int = malloc(BUF_LEN);
    int number_of_digits = 1, number_copy = number;
    while (number_copy >= 10) {
        number_of_digits++;
        number_copy /= 10;
    }

    for (int i = number_of_digits - 1; i >= 0; i--) {
        converted_int[i] = (number % 10) + '0'; 
        number /= 10;
    }

    converted_int[number_of_digits] = '\0';

    return converted_int;
}

int main() {
    #ifdef WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
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
        int number_of_spaces = count_spaces(message);
        printf("%s\n", message);
        printf("Number of spaces: %d\n", number_of_spaces);
        char* response = convert_int_to_string(number_of_spaces);
        send(client_connection, response, BUF_LEN, 0);
        free(response);
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