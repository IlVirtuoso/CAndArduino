#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>
#include "./resources/libs/parameters.h"
#include "./resources/libs/piece.h"
#include "./resources/libs/player.h"
#include <sys/shm.h>
#include "macro.c"



void handler(int signum);
void clean();
struct sigaction sa;
typedef struct _cell{
    char flag;
    int isFull;
}cell;






int main(int argc, char * argv[]){
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handler;



    return 0;
}


void handler(int signum){
    if(signum == SIGINT){
        clean();
    }
}

void clean(){
    
    
}

int debug(char message []){
    printf("[Debug]: %s", message);
    return 0;
}
