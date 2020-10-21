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

/**
 * 1.The client sends periodical PING datagrams with a random content to a <server> and <port> specified in command line. 
 *   The server returns back (echoes) the same packets (content). 
 *   The client checks the content of the received packets to match what was sent
 *   and computes the round trip time and displays it to the user – for each sent packet. 
 * **/

void generate_message(char message[]) {
    int random_message_length = 1 + rand() % (BUF_LEN - 1);
    char random_characters[3];

    for (int i = 0; i < random_message_length; i++) {
        random_characters[0] = (int)'A' + rand() % ((int)'Z' - (int)'A' + 1);     
        random_characters[1] = (int)'a' + rand() % ((int)'z' - (int)'a' + 1);     
        random_characters[2] = (int)'0' + rand() % ((int)'9' - (int)'0' + 1);     
        message[i] = random_characters[rand() % 3];
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (argc != 3) {
        printf("Usage: %s [server IP address] [port]\n", argv[0]);
        return 0;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    server_address.sin_addr.s_addr = inet_addr(argv[1]);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("UDP socket creation failed: %s", strerror(errno));
    }

    char buffer[BUF_LEN], server_response[BUF_LEN];
    int request_number = 0;
    while (true) {
        sleep(2);
        request_number++;
        printf("Request no. %d:\n", request_number);
        memset(buffer, 0, BUF_LEN);
        generate_message(buffer);

        printf("The random message to be sent is: %s\n", buffer);

        clock_t start_time = clock();
        int bytes_sent = sendto(socket_fd, buffer, BUF_LEN, 0, (struct sockaddr*)&server_address, sizeof(server_address));

        if (bytes_sent < 0) {
            printf("Message could not be sent: %s", strerror(errno));
            continue;
        }  else if (!bytes_sent) {
            printf("No content has been sent to the server.\n");
            continue;
        }

        int address_length = sizeof(server_address);
        bytes_sent = recvfrom(socket_fd, server_response, BUF_LEN, 0, (struct sockaddr*)&server_address, &address_length);

        if (bytes_sent < 0) {
            printf("Error in receiving message: %s", strerror(errno));
            continue;
        }

        clock_t end_time = clock();
        printf("Received a response from the server: %s\n", server_response);

        if (!strcmp(buffer, server_response)) {
            printf("The server's echo response is identical.\n");
        } else {
            printf("The server's echo response is different.\n");
        }

        double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Time taken: %f seconds\n\n", time_spent);
    }

    return 0;
}