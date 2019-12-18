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


/*file di log del player*/
FILE * player_logger;

/*buffer per log modificati*/
char player_logbuffer[1024];

/*nome del file di log*/
char filename[24];

int player(){
    sprintf(filename,"Player %c.log", player_id);
    player_logger = fopen(filename,"a+");
    return 0;

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
    return 0;
}

void player_logg(char message []){
    double time = (double)clock()/1000;
    printf("[LOG: %f]%s\n",(double)time,message);
    fprintf(player_logger,"[LOG : %f] %s\n",(double)time,message);
    bzero(player_logbuffer,sizeof(player_logbuffer));
}

void player_handler(int signum){
    if(signum == SIGINT){
        player_clean();
    }
}

int i;
void player_clean(){
    player_logg("Interruzione esecuzione in corso");
    for(i = 0; i < sizeof(pieces); i++){
        kill(pieces[i],SIGINT);
    }
    fclose(player_logger);
    shmdt(player_shared_table);
    exit(0);
}

