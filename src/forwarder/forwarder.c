#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "forwarder.h"

#define MAXBUFF 1024

int* fd_in;
int* fd_out;

void startForwardingToOut() {
    char buf[MAXBUFF];
    ssize_t nbytes;

    while (1) {
        nbytes = read(*fd_in, buf, MAXBUFF);
        if (nbytes < 0) {
            perror("Error reading from fd_in");
            return;
        } else if (nbytes == 0) {
            // End of file
            break;
        }

        ssize_t total_written = 0;
        while (total_written < nbytes) {
            ssize_t written = write(*fd_out, buf + total_written, nbytes - total_written);
            if (written < 0) {
                perror("Error writing to fd_out");
                return;
            }
            total_written += written;
        }
    }
}

void startForwardingToIn() {
    char buf[MAXBUFF];
    ssize_t nbytes;

    while (1) {
        nbytes = read(*fd_out, buf, MAXBUFF);
        if (nbytes < 0) {
            perror("Error reading from fd_out");
            return;
        } else if (nbytes == 0) {
            // End of file
            break;
        }

        ssize_t total_written = 0;
        while (total_written < nbytes) {
            ssize_t written = write(*fd_in, buf + total_written, nbytes - total_written);
            if (written < 0) {
                perror("Error writing to fd_in");
                return;
            }
            total_written += written;
        }
    }
}