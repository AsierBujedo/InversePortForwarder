#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "forwarder/forwarder.h"

#define LO_ADDR (uint32_t) 0x7F000001 // IP 127.0.0.1
#define DEST_PORT (uint16_t) 0x1F91 // PORT 8081

int *socket_app, *socket_out;

void handleInterrupt() {
    isAlive = 0;

    if (socket_app != NULL) {
        close(*socket_app);
    }

    if (socket_out != NULL) {
        close(*socket_out);
    }

    exit(EXIT_SUCCESS);
}

int opencsocket(int *soc, uint16_t *port, uint32_t *ip_addr) {
    if ((*soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *ip_addr;
    addr.sin_port = htons(*port);

    socklen_t addrsize = sizeof(addr);

    if (connect(*soc, (struct sockaddr*) &addr, addrsize) < 0) {
        perror("Failed creating a new connection");
        return 1;
    }

    return 0;
}

void startThreads() {
    pthread_t th_in;
    pthread_t th_out;

    pthread_create(&th_in, NULL, (void*) startForwardingToOut, NULL);
    pthread_create(&th_out, NULL, (void*) startForwardingToIn, NULL);

    pthread_join(th_in, NULL);
    pthread_join(th_out, NULL);
}

int main(int argc, char* argv[]) {

    unsigned int PORT = 8080;
    struct in_addr IP_ADD;

    if (argc == 3) {
        PORT = atoi(argv[2]);
        if (inet_pton(AF_INET, argv[1], &IP_ADD) != 1) {
            perror("Enter a valid input");
            printf("Usage: ./CIPF dst_ip_addr port\n");
            printf("Example usage: ./CIPF 88.87.86.85 8080\n");
            exit(EXIT_FAILURE);
        }
        if (PORT == 0) {
            perror("Enter a valid input");
            printf("Usage: ./CIPF dst_ip_addr port\n");
            printf("Example usage: ./CIPF 88.87.86.85 8080\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Usage: ./CIPF dst_ip_addr src_app_port\n");
        printf("Example usage: ./CIPF 88.87.86.85 8080\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handleInterrupt);

    socket_app = malloc(sizeof(int));
    socket_out = malloc(sizeof(int));

    uint16_t DPORT = DEST_PORT;
    int res = opencsocket(socket_out, &DPORT, &IP_ADD.s_addr);
    if (res == 1) {
        perror("Could not connect to server. The connection with the app will not be established");
        exit(EXIT_FAILURE);
    }

    uint32_t lo = LO_ADDR;
    uint16_t APP_PORT = (uint16_t) PORT;
    res = opencsocket(socket_app, &APP_PORT, &lo);
    if (res == 1) {
        perror("Could not connect to the app");
        exit(EXIT_FAILURE);
    }

    startThreads();

    return 0;
}