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

/*array che identifica i pezzi appartenenti a questo giocatore*/
pid_t pieces[SO_NUM_P];

/*player id*/
char player_id;


int player(){
    player_id = (rand())%127 + 43;
    return 0;
}

int i;
pid_t pid;
int piecegen(int numpieces){
    for(i = 0; i < numpieces; i++){
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
}