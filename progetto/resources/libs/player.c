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
#include <sys/shm.h>
#include "macro.h"
#include "piece.h"
#include "player.h"

FILE * player_logger;
char player_logbuffer[1024];
char filename[14];


int player(){
    sprintf(filename,"Player %c.log", player_id);
    player_logger = fopen(filename,"a+");
    return -1;

}

int i;
/*typedef pid_t int */


int piecegen(int numpieces){
    for(i = 0; (player_pid == getpid()) && (i < numpieces); i++){
        if((pid = fork())){
            /*player*/
            pieces[i] = pid;
        }
        
        else{
            /*pieces*/
            if(piece() == -1){
                return -1;
                exit(1);
            }
        }
    }
    return -1;
}

void player_logg(char message []){
    double time = (double)clock()/1000;
    printf("[LOG: %f]%s\n",(double)time,message);
    fprintf(player_logger,"[LOG : %f] %s\n",(double)time,message);
    bzero(player_logbuffer,sizeof(player_logbuffer));
}