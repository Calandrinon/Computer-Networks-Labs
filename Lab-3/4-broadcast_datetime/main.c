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
#include <pthread.h>
#define MSG_LEN 1024
#define SHORT_MSG 256

struct sockaddr_in broadcast_address;
int server_socket_fd, client_socket_fd_time, client_socket_fd_date; 
pthread_t timequery_thread, datequery_thread; 

void get_time(char thetime[]) {
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcpy(thetime, asctime(timeinfo));
	char delimiter[2] = " ";
	char* token = strtok(thetime, delimiter);	
	
	while (token != NULL) {
		if (strstr(token, ":")) {
			sprintf(thetime, "TIME %s", token);
			return;
		}
		token = strtok(NULL, delimiter);
	}
}

void get_date(char thedate[]) {
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char day[5], month[5];
	if (timeinfo->tm_mday < 10)
		sprintf(day, "0%d", timeinfo->tm_mday);
	else
		sprintf(day, "%d", timeinfo->tm_mday);

	if (timeinfo->tm_mon + 1 < 10)
		sprintf(month, "0%d", 1 + timeinfo->tm_mon);
	else 	
		sprintf(month, "%d", 1 + timeinfo->tm_mon);

	sprintf(thedate, "DATE %s/%s/%d\n", day, month, 1900 + timeinfo->tm_year);
}


void* timequery_broadcaster(void* argument) {
    char query[] = "TIMEQUERY";
    char response[SHORT_MSG];

    while (true) {
        sleep(3);
        int bytes_sent = sendto(client_socket_fd_time, query, strlen(query), 0, (struct sockaddr*)&broadcast_address, sizeof(broadcast_address));
        if (bytes_sent < 0) {
            printf("Error sending message: %s\n", strerror(errno));
            continue;
        }
        printf("Timequery thread sent a request.\n");
        
        int address_size = sizeof(broadcast_address);
        printf("Timequery thread recvfrom called.\n");
        int bytes_received = recvfrom(client_socket_fd_time, response, SHORT_MSG, 0, (struct sockaddr*)&broadcast_address, &address_size);
        printf("Timequery thread recvfrom finished.\n");
        if (bytes_received < 0) {
            printf("Error sending message: %s\n", strerror(errno));
        }
        printf("Timequery thread received a response: %s\n", response);
    }
}


void* datequery_broadcaster(void* argument) {
    char query[] = "DATEQUERY";
    char response[SHORT_MSG];

    while (true) {
        sleep(10);
        int bytes_sent = sendto(client_socket_fd_date, query, strlen(query), 0, (struct sockaddr*)&broadcast_address, sizeof(broadcast_address));
        if (bytes_sent < 0) {
            printf("Error sending message: %s\n", strerror(errno));
        }
        printf("Datequery thread sent a request.\n");

        int address_size = sizeof(broadcast_address);
        printf("Datequery thread recvfrom called.\n");
        int bytes_received = recvfrom(client_socket_fd_date, response, SHORT_MSG, 0, (struct sockaddr*)&broadcast_address, &address_size);
        printf("Datequery thread recvfrom finished.\n");
        if (bytes_received < 0) {
            printf("Error sending message: %s\n", strerror(errno));
        }
        printf("Datequery thread received a response: %s\n", response);
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s [network broadcast address]\n", argv[0]);
        return 0;
    }

    broadcast_address.sin_family = AF_INET;
    broadcast_address.sin_port = htons(7777);
    broadcast_address.sin_addr.s_addr = inet_addr(argv[1]);

    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    client_socket_fd_time = socket(AF_INET, SOCK_DGRAM, 0);
    client_socket_fd_date = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_socket_fd < 0 || client_socket_fd_time < 0 || client_socket_fd_date < 0) {
        printf("Socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    int broadcast_option = 1;
    int sockopt_exit_status_1 = setsockopt(client_socket_fd_time, SOL_SOCKET, SO_BROADCAST, &broadcast_option, sizeof(broadcast_option)); 
    int sockopt_exit_status_2 = setsockopt(client_socket_fd_date, SOL_SOCKET, SO_BROADCAST, &broadcast_option, sizeof(broadcast_option)); 
    if (sockopt_exit_status_1 < 0 || sockopt_exit_status_2 < 0) {
        printf("Setting socket options failed: %s\n", strerror(errno));
        return -1;
    }

    if (bind(server_socket_fd, (struct sockaddr*)&broadcast_address, sizeof(broadcast_address)) < 0) {
        printf("Binding failed: %s\n", strerror(errno));
        return -1;
    }

    printf("Listening...\n");
    char message[MSG_LEN];

    pthread_create(&timequery_thread, NULL, timequery_broadcaster, NULL);
    pthread_create(&datequery_thread, NULL, datequery_broadcaster, NULL);

    while (true) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        printf("Main thread recvfrom called.\n");
        int bytes_received = recvfrom(server_socket_fd, message, MSG_LEN, 0, (struct sockaddr*)&client_address, &address_size);
        printf("Main thread recvfrom finished.\n");
        if (bytes_received < 0) {
            printf("Error receiving message: %s\n", strerror(errno));
            continue;
        }        

        printf("Main thread received a request from %s: %s\n", inet_ntoa(client_address.sin_addr), message);

        if (!strncmp(message, "TIMEQUERY", 9)) {
            char thetime[SHORT_MSG];
            get_time(thetime);
            int bytes_sent = sendto(server_socket_fd, thetime, SHORT_MSG, 0, (struct sockaddr*)&client_address, sizeof(client_address));
            if (bytes_sent < 0) {                    
                printf("Error sending message: %s\n", strerror(errno));
                continue;
            }
            printf("Sent TIMEQUERY response.\n");
        }

        if (!strncmp(message, "DATEQUERY", 9)) {
            char thedate[SHORT_MSG];
            get_date(thedate);
            int bytes_sent = sendto(server_socket_fd, thedate, SHORT_MSG, 0, (struct sockaddr*)&client_address, sizeof(client_address));
            if (bytes_sent < 0) {                    
                printf("Error sending message: %s\n", strerror(errno));
                continue;
            }
            printf("Sent DATEQUERY response.\n");
        }
    }

    pthread_join(timequery_thread, NULL);
    pthread_join(datequery_thread, NULL);

    close(client_socket_fd_time);
    close(client_socket_fd_date);
    close(server_socket_fd);
    return 0;
}