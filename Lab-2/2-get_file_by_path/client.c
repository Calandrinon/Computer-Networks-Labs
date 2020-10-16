#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
#define MAX_CONTENT_LEN 32767 

/**
 *  The client sends the complete path to a file. 
 *  The server returns back the length of the file and its content in the case the file exists. 
 *  When the file does not exist the server returns a length of -1 and no content. 
 *  The client will store the content in a file with the same name as the input file with the suffix –copy appended
 *  (ex: for f.txt => f.txt-copy).
 * **/

void get_filename_from_path(char path[], char filename[]) {
    char delimiter[2] = "/";
    char* token = strtok(path, delimiter);

    while (token != NULL) {
        strcpy(filename, token);
        token = strtok(NULL, delimiter);
    }

    strcat(filename, "-copy");
    char* newline_location = strstr(filename, "\n");
    int index_to_delete = newline_location - filename;
    memmove(&filename[index_to_delete], &filename[index_to_delete + 1], strlen(filename) - index_to_delete);
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
    server_address.sin_port = htons(1234);
    inet_aton("192.168.1.16", &server_address.sin_addr);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Socket could not be created.\n");
        return 0;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection could not be established.\n");
        return 0;
    }

    char contents[MAX_CONTENT_LEN], path[256];
    int32_t file_length;
    printf("Enter the path of the file to download: ");
    fgets(path, 256, stdin);
    send(socket_fd, path, sizeof(path), 0);
    recv(socket_fd, &file_length, sizeof(file_length), 0);
    file_length = ntohl(file_length);
    printf("File size: %d\n", file_length);

    if (file_length == -1) {
        printf("The file does not exist or it is a directory\n");
        return -1;
    }

    recv(socket_fd, contents, MAX_CONTENT_LEN, 0);

    char filename[100];
    get_filename_from_path(path, filename);

    printf("The file is called \"%s\" and is placed in the current directory.\n", filename);
    FILE* file_descriptor = fopen(filename, "w");
    fprintf(file_descriptor, "%s", contents);
    return 0;
}
