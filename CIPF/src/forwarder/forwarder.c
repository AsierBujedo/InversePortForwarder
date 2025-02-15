#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "forwarder.h"

#define MAXBUFF 1024

/**
 * @file forwarder.c
 * @brief This file contains functions for forwarding data between file descriptors.
 *
 * The functions in this file are used to read data from one file descriptor and write it to another,
 * effectively forwarding the data. The forwarding continues as long as the `isAlive` flag is set.
 */

 /**
 * @brief Starts forwarding data from fd_app to fd_out.
 *
 * This function reads data from the file descriptor pointed to by fd_app and writes it to the file
 * descriptor pointed to by fd_out. The forwarding continues as long as the `isAlive` flag is set.
 * If an error occurs during reading or writing, an error message is printed and the function returns.
 */
void startForwardingToOut();

/**
 * @brief Starts forwarding data from fd_out to fd_app.
 *
 * This function reads data from the file descriptor pointed to by fd_out and writes it to the file
 * descriptor pointed to by fd_app. The forwarding continues as long as the `isAlive` flag is set.
 * If an error occurs during reading or writing, an error message is printed and the function returns.
 */
void startForwardingToIn();
int *fd_app;
int *fd_out;

int *isAlive;

void startForwardingToOut() {
    char buf[MAXBUFF];
    ssize_t nbytes;

    printf("The server is forwarding to outside. isAlive: %s\n", *isAlive ? "true" : "false");

    while (*isAlive) {
        nbytes = read(*fd_app, buf, MAXBUFF);
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

    printf("The server is forwarding to the local client. isAlive: %s\n", *isAlive ? "true" : "false");

    while (*isAlive) {
        nbytes = read(*fd_out, buf, MAXBUFF);
        if (nbytes < 0) {
            perror("Error reading from fd_out");
            return;
        } else if (nbytes == 0) {
            break;
        }

        ssize_t total_written = 0;
        while (total_written < nbytes) {
            ssize_t written = write(*fd_app, buf + total_written, nbytes - total_written);
            if (written < 0) {
                perror("Error writing to fd_in");
                return;
            }
            total_written += written;
        }
    }
}