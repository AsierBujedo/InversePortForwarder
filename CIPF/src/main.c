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

#define LO_ADDR (uint32_t) 0x7F000001 // 127.0.0.1
#define DEST_PORT (uint16_t) 0x1F91 // 8081

void handleInterrupt() {
    if(isAlive != NULL) {
        *isAlive = 0;
        free(isAlive);
    }

    if(fd_app != NULL) {
        close(*fd_app);
        free(fd_app);
    }

    if(fd_out != NULL) {
        close(*fd_out);
        free(fd_out);
    }

    exit(EXIT_FAILURE);
}

int opencsocket(int *soc, uint16_t port, uint32_t ip_addr) {

    soc = (int*) malloc(sizeof(int));

    if ((*soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip_addr;
    addr.sin_port = htons(port);

    socklen_t addrsize = sizeof(addr);

    if (connect(*soc, (struct sockaddr*) &addr, addrsize) < 0) {
        perror("Failed creating a new connection");
        return 0;
    }
    return 1;
}

void startThreads() {
    pthread_t th_in;
    pthread_t th_out;

    isAlive = malloc(sizeof(int));
    if(isAlive == NULL) {
        perror("Failed allocating memory");
        handleInterrupt();
    }

    *isAlive = 1;

    pthread_create(&th_in, NULL, (void*) startForwardingToOut, NULL);
    pthread_create(&th_out, NULL, (void*) startForwardingToIn, NULL);

    pthread_join(th_in, NULL);
    pthread_join(th_out, NULL);
}

int main(int argc, char* argv[]) {

    int PORT;
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

    if( opencsocket(fd_app, DEST_PORT, IP_ADD.s_addr) == 0 ) {
        perror("Could not connect to server. The connection with the app will not be established");
        exit(EXIT_FAILURE);
    }

    uint32_t lo = htonl(LO_ADDR);
    uint16_t APP_PORT = (uint16_t) PORT;
    if( opencsocket(fd_out, APP_PORT, lo) == 0 ) {
        perror("Could not connect to the app");
        exit(EXIT_FAILURE);
    }

    startThreads();

    return 0;
}