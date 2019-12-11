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
#include "macro.h"


/* funzione per mostrare il [debug] su console, attenzione la variabile debug deve essere 1 */
int debug(char message []);

/*handler per il segnale di interruzione SIGINT*/
void handler(int signum);

/*cleaner per SIGINT*/
void clean();

/*permette di mostrare lo stato della tabella, il metodo è costruito per funzionare a frame */
void display();


struct sigaction sa;

/*definizione della struttura della cella della tabella*/
typedef struct _cell{
    char flag;
    int isFull;

}cell;


cell table[SO_BASE][SO_ALTEZZA];



int i;
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


int x; 
int y;
void display(){ 
    
    system("clear");
    for(x = 0 ; x < SO_BASE; x++){
        for(y = 0; y < SO_ALTEZZA; y++){
            printf("|%c|", 'a');
        }
        printf("\n");
    }
}
