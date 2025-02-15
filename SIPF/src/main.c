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

#define PORT 8000

int *socket_in, *socket_out;

void handleInterrupt() {
    *isAlive = 0;

    if(fd_in != NULL) {
        close(*fd_in);
    }
    
    if(fd_out != NULL) {
        close(*fd_out);
    }
     
    exit(EXIT_SUCCESS);
}

int* openlsocket(int *soc, int port) {
    if ((*soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Failed creating socket at %i\n", port);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    socklen_t addrsize = sizeof(addr);

    if((bind(*soc, (struct sockaddr*) &addr, addrsize)) < 0) {
        perror("Error binding the socket");
        exit(EXIT_FAILURE);
    }

    if(listen(*soc, 1) == -1) {
        perror("Failed listening");
        exit(EXIT_FAILURE);
    } 

    int conn_s;
    int *conn_aux = NULL;
    if (conn_s = accept(*soc, (struct sockaddr*) &addr, (socklen_t*) &addrsize) > 0) {
        conn_aux = malloc(sizeof(int));
        *conn_aux = conn_s;
    }
    return conn_aux;
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

int main() {

    signal(SIGINT, handleInterrupt);

    socket_in = malloc(sizeof(int));
    socket_out = malloc(sizeof(int));

    fd_in = openlsocket(socket_in, PORT);
    fd_out = openlsocket(socket_out, PORT + 1);

    printf("%i, %i\n", *fd_in, *fd_out);

    startThreads();

    return 0;
}
