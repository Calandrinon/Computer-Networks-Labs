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
    server_address.sin_addr.s_addr = INADDR_ANY;


    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("Socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding failed: %s\n", strerror(errno));
        return -1;
    }

    printf("...\n");    

    bool finished = false;
    int lower_bound = 1, upper_bound = (int)1e6 -1;
    srand(time(NULL));
    char answer;

    struct sockaddr_in clients[1024];
    int number_of_clients = 0;

    int random_number = (rand() % (upper_bound - lower_bound + 1)) + lower_bound; 
    printf("The random number is %d\n", random_number);
    int client_number, client_id;

    while (!finished) {
        struct sockaddr_in client_address;
        int address_length = sizeof(client_address);
        int bytes_received = recvfrom(socket_fd, &client_number, sizeof(client_number), 0, (struct sockaddr*)&client_address, &address_length);
        client_number = ntohl(client_number);
        bool already_in_list = false;
        for (int i = 0; i < number_of_clients; i++) {
            if (clients[i].sin_addr.s_addr == client_address.sin_addr.s_addr) {
                already_in_list = false;
                client_id = i;
                break;
            }
        }

        if (!already_in_list) {
            number_of_clients++;
            clients[number_of_clients] = client_address;
        }


        if (bytes_received < 0) {
            printf("Recvfrom failed: %s\n", strerror(errno));
            continue;
        }

        if (client_number == random_number) {
            answer = 'G';
            if (sendto(socket_fd, &answer, sizeof(answer), 0, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
                printf("Sendto failed: %s\n", strerror(errno));
                continue;
            }

            answer = 'L';
            for (int i = 0; i < number_of_clients; i++) {
                sendto(socket_fd, &answer, sizeof(answer), 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
            }

            finished = true;
        }

        if (client_number > random_number) {
            answer = 'S';
            if (sendto(socket_fd, &answer, sizeof(answer), 0, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
                printf("Sendto failed: %s\n", strerror(errno));
                continue;
            }
        } 

        if (client_number < random_number) {
            answer = 'H';
            if (sendto(socket_fd, &answer, sizeof(answer), 0, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
                printf("Sendto failed: %s\n", strerror(errno));
                continue;
            }
        } 
    }

    close(socket_fd);

    return 0;
}