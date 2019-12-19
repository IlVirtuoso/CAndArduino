#define _GNU_SOURCE
#ifndef IPC_H
#include <sys/ipc.h>
#endif
#ifndef MSG_H
#include <sys/msg.h>
#endif
#ifndef WAIT_H
#include <sys/wait.h>
#endif
#ifndef ERRNO_H
#include <sys/errno.h>
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef MESSAGE_H
#define MESSAGE_H 

/*id della msgqueue*/
int msgqueue;

/*struttura del messaggio*/
typedef struct {
    long messageType;
    char message[1024];
}message;

void message_start();
#endif