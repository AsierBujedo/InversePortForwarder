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

#define PORT_IN (uint16_t) 0x1F90 // 8080
#define PORT_OUT (uint16_t) 0x1F91 // 8081

void handleInterrupt() {
    if(isAlive != NULL) {
        *isAlive = 0;
        free(isAlive);
    }

    if(fd_in != NULL) {
        close(*fd_in);
        free(fd_in);
    }

    if(fd_out != NULL) {
        close(*fd_out);
        free(fd_out);
    }

    exit(EXIT_FAILURE);
}

int openlsocket(int *fd, uint16_t port) {
    
    int soc;
    if ( (soc = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "Failed creating socket at %i\n", port);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    socklen_t addrsize = sizeof(addr);

    if( (bind(soc, (struct sockaddr*) &addr, addrsize)) < 0 ) {
        perror("Error binding the socket");
        exit(EXIT_FAILURE);
    }

    if( listen(soc, 1) == -1 ) {
        perror("Failed listening");
        exit(EXIT_FAILURE);
    } 

    int conn_s;
    if ( (conn_s = accept(soc, (struct sockaddr*) &addr, (socklen_t*) &addrsize)) > 0) {
        *fd = conn_s;
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

    pthread_create(&th_in, NULL, (void*) startForwardingToOut, NULL);
    pthread_create(&th_out, NULL, (void*) startForwardingToIn, NULL);

    *isAlive = 1;

    pthread_join(th_in, NULL);
    pthread_join(th_out, NULL);
}

int main() {

    signal(SIGINT, handleInterrupt);

    // First, listen for to the client endpoint
    fd_out = malloc(sizeof(int));
    if (fd_out == NULL) {
        perror("Failed allocating memory");
    }
    if( openlsocket(fd_out, (uint16_t) PORT_OUT) == 0 ) {
        printf("Connected to the stream.\n");
    }

    // Then, listen for the forwarded app
    fd_in = malloc(sizeof(int));
    if (fd_in == NULL) {
        perror("Failed allocating memory");
    }
    if( openlsocket(fd_in, (uint16_t) PORT_IN) == 0) {
    printf("Connected to the app.\n");
    }

    startThreads();

    return 0;
}
