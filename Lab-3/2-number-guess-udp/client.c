#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_SIZE 1024

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("Socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    bool finished = false;
    int lower_bound = 1, upper_bound = (int)1e6 - 1;
    srand(time(NULL));
    char answer;

    int step_count = 0;
    while (!finished) {
        int my_num = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
        int address_size = sizeof(server_address);
        printf("Sent: %d ", my_num);
        my_num = htonl(my_num);
        if (sendto(socket_fd, &my_num, sizeof(my_num), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
            printf("Sendto failed: %s\n", strerror(errno));
            close(socket_fd);
            return -2;
        }

        if (recvfrom(socket_fd, &answer, sizeof(char), 0, (struct sockaddr*)&server_address, &address_size) < 0) {
            printf("Recvfrom failed: %s\n", strerror(errno));
            close(socket_fd);
            return -2;
        }

        step_count++;
        printf("Answer: %c\n", answer);
        
        if (answer == 'H') {
            lower_bound = ntohl(my_num);
        }

        if (answer == 'S') {
            upper_bound = ntohl(my_num);
        }

        if (answer == 'G' || answer == 'L') {
            finished = true;
        }

        sleep(1);
    }

    close(socket_fd);

    if (answer == 'G') {
        printf("You won!\n");
    } else {
        printf("You lost.\n");
    }

    return 0;
}