#ifndef FORWARDER_H
#define FORWARDER_H

#include <stddef.h>

extern int *fd_in;
extern int *fd_out;

extern int *isAlive;

void startForwardingToOut();
void startForwardingToIn();

#endif // FORWARDER_H