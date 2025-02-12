#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "forwarder/forwarder.h"

#define PORT 25565

int *socket_in, *socket_out;

int* openlsocket(int* soc, int port) {
    if ((*soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Failed creating socket at %i\n", PORT);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(PORT);
    
    socklen_t addrsize = sizeof(addr);

    if((bind(*soc, (struct sockaddr*) addr, addrsize)) < 0) {
        fprintf(stderr, "Error binding the socket\n");
        exit(EXIT_FAILURE);
    }

    if(listen(*soc, 1) == -1) {
        fprintf(stderr, "Failed listening\n");
        exit(EXIT_FAILURE);
    } 

    int conn_s;
    int *conn_aux = NULL;
    if (conn_s = accept(*soc, (struct sockaddr*) addr, (socklen_t*)&addrsize) > 0) {
        int* conn_aux = malloc(sizeof(int));
        *conn_aux = conn_s;
    }
    free(addr);
    return conn_aux;
}

void startThreads() {
    pthread_t th_in;
    pthread_t th_out;

    pthread_create(&th_in, NULL, (void*) startForwardingToOut, NULL);
}

int main() {
    socket_in = malloc(sizeof(int));
    socket_out = malloc(sizeof(int));

    fd_in = openlsocket(socket_in, PORT);
    fd_out = openlsocket(socket_out, PORT + 1);

    startThreads();

    return 0;
}
