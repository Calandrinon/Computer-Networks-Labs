#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

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
#define STR_LEN 2048

int qsort_compare(const void* a, const void* b) {
    int* first_number = (int*)a;
    int* second_number = (int*)b;
    if (*first_number <= *second_number)
        return -1; 
    return 1;
}

typedef struct {
    int32_t divisors[500]; 
    int32_t size;
} DivisorArray;

DivisorArray* get_divisors(int32_t number) {
    DivisorArray* divisors = malloc(sizeof(DivisorArray));
    divisors->size = 0;

    for (int i = 1; i <= sqrt(number); i++) {
        if (number % i == 0) {
            char number_as_string[12];
            if (i == number / i) {
                divisors->divisors[divisors->size] = i;
                divisors->size++;
            } else {                
                divisors->divisors[divisors->size] = i;
                divisors->size++;

                divisors->divisors[divisors->size] = number / i;
                divisors->size++;
            }
        }
    }

    qsort(divisors->divisors, divisors->size, sizeof(int32_t), qsort_compare);
    return divisors;
}

int main() {
    #ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("Initializing the WinSock API failed.\n");
        return 0;
    }
    #endif

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("The socket could not be created.\n");
        return 0;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Binding the socket on the specified address and port failed.\n");
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
        int client_connection = accept(socket_fd, (struct sockaddr*)&client_address, &address_size);

        if (client_connection < 0) {
            printf("Could not connect with the client.\n");
            continue;
        }

        int32_t number, number_of_divisors;
        recv(client_connection, &number, sizeof(number), 0);
        number = ntohl(number);
        DivisorArray* divisors = get_divisors(number);
        int divisors_index = 0;

        number_of_divisors = htonl(divisors->size);
        //sends the number of divisors
        send(client_connection, &number_of_divisors, sizeof(number_of_divisors), 0);

        for (divisors_index; divisors_index < divisors->size; divisors_index++) {
            printf("%d\n", divisors->divisors[divisors_index]); 
            int32_t number_to_send = divisors->divisors[divisors_index]; 
            number_to_send = htonl(number_to_send);
            send(client_connection, &number_to_send, sizeof(int32_t), 0);
        }

        free(divisors);

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