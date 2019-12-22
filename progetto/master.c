#define _GNU_SOURCE
#ifndef SIGNAL_H
#include <signal.h>
#endif
#ifndef STDIO_H
#include <stdio.h>
#endif
#ifndef STDLIB_H
#include <stdlib.h>
#endif
#ifndef UNISTD_H
#include <unistd.h>
#endif
#ifndef TYPES_H
#include <sys/types.h>
#endif
#ifndef SYSINFO_H
#include <sys/sysinfo.h>
#endif
#ifndef ERRNO_H
#include <sys/errno.h>
#endif
#ifndef IPC_H
#include <sys/ipc.h>
#endif
#ifndef MSG_H
#include <sys/msg.h>
#endif
#ifndef WAIT_H
#include <sys/wait.h>
#endif
#ifndef SEM_H
#include <sys/sem.h>
#endif
#ifndef TIME_H
#include <time.h>
#endif
#ifndef STRING_H
#include <string.h>
#endif
#ifndef PARAMETERS_H
#include "./resources/libs/parameters.h"
#endif
#ifndef PLAYER_H
#include "./resources/libs/player.h"
#endif
#ifndef PIECE_H
#include "./resources/libs/piece.h"
#endif
#ifndef MONITOR_H
#include "./resources/libs/monitor.h"
#endif
#ifndef SHM_H
#include <sys/shm.h>
#endif
#ifndef MACRO_H
#include "./resources/libs/macro.h"
#endif
#ifndef DEBUG_H
#include "./resources/libs/debug.h"
#endif
#ifndef TABLE_H
#include "./resources/libs/table.h"
#endif
#ifndef MESSAGE_H
#include "./resources/libs/message.h"
#endif
/*handler per il segnale di interruzione SIGINT*/
void handler(int signum);

/*cleaner per SIGINT*/
void clean();

/*metodo per la creazione di flag*/
void placeflag(int x, int y);

/*definisce quante flag sono già state create*/
int placed;

/*uccide tutti i processi inizializzati dal processo master*/
void clean_process();

struct sigaction sa;

/*semaforo*/
struct semaphore * sem;

/*struttura che definisce le bandiere, visibile solo al master*/
typedef struct{
    int x;
    int y;
    int score;
}vexillum;

/*buffer per messaggi*/
char master_logbuffer[128];


/*display debug per il master*/
void display_master();

/*id della scacchiera*/
int table; 

/*master 2 player msgqueue*/
int master_msgqueue;

/*clock per misurare il tempo di esecuzione*/
clock_t cl;

/*clock per il round, resettare a fine round*/
clock_t round_timer;

/*mask per i segnali*/
static sigset_t mask;

/*segmento di memoria condivisa della table*/
cell * master_shared_table;

/* variabile che dice se i giocatori sono stati creati*/
int playercreated = 0;

/*variabile che dice se i pezzi sono stati creati*/
int piececreated = 0;


int status;

/* struttura dati per i punteggi */
typedef struct{
    /* Array contenente i pids di tutti i processi giocatori creati*/
    int pid[SO_NUM_G];
    /* Array contenente i nomi di tutti i processi giocatori creati*/
    char name[SO_NUM_G];
    /* Array contenente il punteggio di tutti i processi giocatori*/
    int score[SO_NUM_G];
}score_table;

score_table * st;

/*puntatore alla struttura vexillum per le bandiere*/
vexillum * vex;

/* stampa la tabella del punteggio */
void stamp_score(score_table * t);

/*variabile che dice se le bandiere sono state create*/
int flagcreated = 0;

/*buffer per i messaggi modificati*/
char master_logbuffer[128];

message msg;

int i;
int j;
int pid;
int num_flag;
int main(int argc, char * argv[]){
    cleaner = clean;
    placed = 0;
    srand(clock() + getpid());
    num_flag = (SO_FLAG_MIN + rand()) % (SO_FLAG_MAX + 1 - SO_FLAG_MIN) + SO_FLAG_MIN;
    /*@HPF4 il tuo esperimento si trova qui*/
    vex = (vexillum *) malloc(num_flag * sizeof(vexillum));
    /* Inizializzazione tabella degli score */
    st =  malloc(sizeof(score_table)); 
    for(i = 0; i < SO_NUM_G; i++){
        st -> name[i] = (char)((int)'A' + i);
        st -> score[i] = 0;
    }
    /* Inizializzazione struttura bandiere */

    


    /*Region: inizializzazione e rilevamento argomenti*/
    for(i = 0; i < argc; i++){ /*inizializza la variabile di debug se richiesto*/
        if(strcmp(argv[i],"-d")) isDebug = 1;
        if(strcmp(argv[i],"-v")) verbosity = 2;
        if(strcmp(argv[i],"-vv")) verbosity = 3;
    }
    logger = fopen("Master.log","a+");
    logg("Started At: %s\n",__TIME__);
    
    /*End-Region*/
    
    logg("Impostazione maschere e segnali");
    /*Region: inizializzazione dei segnali*/
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    sa.sa_mask = mask;
    sa.sa_flags = SA_RESTART; /*Questa flag fa si che dopo l'handling del segnale il codice riparta da dove interrotto*/
    sa.sa_flags = SA_NODEFER; /*Questa flag permette all'handler di generare altri segnali*/
    sigaction(SIGINT,&sa,NULL);
    sigset(SIGINT,handler);
    /*End-Region*/
    
    logg("Segnali Impostati");
    
    logg("Inizializzazione Memoria Condivisa");
    /*Region: Shared Memory Set*/
    master_msgqueue = message_start(IPC_PRIVATE);
    table_start();
    if((master_shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
        error("Errore nell'attach della shared_table",EIO);
    }
    else{
        debug("Shared Table attach completato");
    }

    logg("Setup dei semafori");
    for(i = 0; i < SO_BASE; i++){
        for(j = 0; j < SO_ALTEZZA; j++){
            tab(master_shared_table,i,j)->sem.sem_num = 0;
            tab(master_shared_table,i,j)->id = EMPTY;
            tab(master_shared_table,i,j)->sem.sem_op = 1;
            
        }
    }
    /*End-Region*/
    logg("Memoria Condivisa Inizializzata");
    /*Region: Process Creation*/
    for(i = 0; i < SO_NUM_G; i++){
        if((pid = fork())){
            /*padre*/
            st -> pid[i] = pid;
            logg("Player: %d started with pid: %d",i,pid);
            /*waitpid(pid,NULL,WEXITED);*/
            /*attesa*/
        }
        else{
            /*figlio*/
            player_id = st -> name[i]; /* Assegnazione del nome al Player*/
            if(player() == -1){
                error("Errore nell'inizializzare il player",ECHILD);
            }
        }
    }
    playercreated = 0;
    /*End-Region*/

    /*Region Phase-1:flag*/
    /*End-Region*/

    /*Region Phase-2:Indication*/

    /*End-Region*/

    /*Region Phase-3:Anarchy*/

    /*End-Region*/
    logg("MASTER:End Of Execution");
    logg("MASTER:Stopped at %s",__TIME__);
    return 0;
}

/*Region handler segnali*/
void handler(int signum){
    switch (signum)
    {
    case SIGINT:
        logg("Ricevuto Segnale SIGINT");
        clean();
        break;

    case SIGUSR1:
    logg("Segnale di creazione pezzi inviato al player");
    break;
    
    default:
        break;
    }
}

void clean(){
    logg("MASTER_CLEANER_LAUNCHED");
    shmdt(master_shared_table);
    
    shmctl(table,IPC_RMID,NULL);
    
    free(st);
    free(vex);
    fclose(logger);
    if(playercreated){
        clean_process();
    }
    
}

void clean_process(){
    for(i = 0; i < sizeof(st->pid);i++){
        kill(st->pid[i],SIGINT);
    }
}

/*End Region*/

/*Regione metodi per il debug*/
int x; 
int y;
void display_master(){ 
    system("clear");
    for(x = 0 ; x < SO_BASE; x++){
        for(y = 0; y < SO_ALTEZZA; y++){
            printf("|%c|", tab(master_shared_table,x,y)->id);
        }
        printf("\n");
    }
}



void stamp_score(score_table * t){
	printf("PLAYER         SCORE\n");
	for(i = 0; i < SO_NUM_G; i++){
		printf("PLAYER %c   |   %d  \n", t -> name[i], t -> score[i]);
	}
}

void placeflag(int x, int y){
    vex[placed].x = x;
    vex[placed].y = y;
    tab(master_shared_table,x,y)->id = FLAG;
    placed++;
}

/*End Region*/


