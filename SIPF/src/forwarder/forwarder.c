/**
 * @file forwarder.c
 * @brief This file contains the implementation of functions to forward data between file descriptors.
 *
 * The forwarder module provides two main functions to forward data between file descriptors:
 * - startForwardingToOut: Forwards data from fd_in to fd_out.
 * - startForwardingToIn: Forwards data from fd_out to fd_in.
 *
 * The module uses global variables to manage the file descriptors and the state of the forwarding process.
 */

#define MAXBUFF 1024 ///< Maximum buffer size for reading and writing data.

int *fd_in;  ///< Pointer to the input file descriptor.
int *fd_out; ///< Pointer to the output file descriptor.
int *isAlive; ///< Pointer to a flag indicating whether the forwarding process should continue.

/**
 * @brief Starts forwarding data from fd_in to fd_out.
 *
 * This function reads data from the input file descriptor (fd_in) and writes it to the output file descriptor (fd_out).
 * The forwarding process continues as long as the isAlive flag is set to a non-zero value.
 * If an error occurs during reading or writing, the function prints an error message and returns.
 */
void startForwardingToOut();

/**
 * @brief Starts forwarding data from fd_out to fd_in.
 *
 * This function reads data from the output file descriptor (fd_out) and writes it to the input file descriptor (fd_in).
 * The forwarding process continues as long as the isAlive flag is set to a non-zero value.
 * If an error occurs during reading or writing, the function prints an error message and returns.
 */
void startForwardingToIn();


#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>

#include "forwarder.h"

#define MAXBUFF 1024

int *fd_in;
int *fd_out;

int *isAlive;

void startForwardingToOut() {
    char buf[MAXBUFF];
    ssize_t nbytes;

    if (isAlive == NULL) {
        printf("isAlive is NULL!\n");
    }
    if (fd_in == NULL) {
        printf("fd_in is NULL!\n");
    }

    while (*isAlive) {
        nbytes = recv(*fd_in, buf, MAXBUFF, 0);
        if (nbytes < 0) {
            perror("Error reading from fd_in");
            *isAlive = 0;
            return;
        } else if (nbytes == 0) {
            printf("Connection ended.\n");
            *isAlive = 0;
            break;
        }

        ssize_t total_written = 0;
        while (total_written < nbytes) {
            ssize_t written = send(*fd_out, buf + total_written, nbytes - total_written, 0);
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

    while (*isAlive) {
        nbytes = recv(*fd_out, buf, MAXBUFF, 0);
        if (nbytes < 0) {
            perror("Error reading from fd_out");
            *isAlive = 0;
            return;
        } else if (nbytes == 0) {
            printf("Connection ended.\n");
            *isAlive = 0;
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