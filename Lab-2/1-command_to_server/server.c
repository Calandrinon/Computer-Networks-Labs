#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define COMMAND_LENGTH 100
#define BUFFER_SIZE 8192

int main() {
    signal(SIGCHLD, SIG_IGN);
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
        printf("Binding could not be created.\n");
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
            printf("Connection with the client failed.\n");
            continue;
        }

        char command[COMMAND_LENGTH];
        if (fork() == 0) {
            recv(client_socket, command, COMMAND_LENGTH, 0);
            char command_name[20];
            char delimiter[] = " ";
            char* arguments[10];
            int arg_index = 0;
            char* token = strtok(command, delimiter);
            strcpy(command_name, token);
            printf("command name: %s\n", command_name);
            while (token != NULL) {
                int string_length = strlen(token) + 1;
                arguments[arg_index] = malloc(sizeof(char)*string_length);
                strcpy(arguments[arg_index], token); 
                arguments[arg_index][string_length - 1] = '\0'; 
                printf("arguments[%d] = %s\n", arg_index, arguments[arg_index]);
                arg_index++;
                token = strtok(NULL, delimiter);
            }

            int p[2];
            pipe(p);

            int pid = fork();
            if (pid == 0) {
                close(p[0]);
                dup2(p[1], 1);
                dup2(p[1], 2);
                execvp(command_name, arguments);
            }

            wait(0);
            close(p[1]);

            char buffer[BUFFER_SIZE], *current_char = buffer - 1;
            do {
                current_char++;
                read(p[0], current_char, 1);
            } while (*current_char != '\0');

            close(p[0]);
            printf("Result:\n%s\n", buffer);
            uint32_t content_size = htonl(current_char - buffer);
            send(client_socket, &content_size, sizeof(content_size), 0);
            send(client_socket, buffer, BUFFER_SIZE, 0);
            exit(0);
        }

        close(client_socket);
    }

    close(socket_fd);
    return 0;
}
