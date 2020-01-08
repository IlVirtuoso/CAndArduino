#define _GNU_SOURCE
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
#ifndef SIGNAL_H
#include <signal.h>
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



/*handler per il segnale di interruzione SIGINT*/
void handler(int signum);

/*cleaner per SIGINT*/
void clean();

/*definisce quante flag sono già state create*/
int placed;

/*uccide tutti i processi inizializzati dal processo master*/
void clean_process();


/*master methods*/
void init();
void sem_init();
void shared_table_init();
void playergen(int playernum);
void manual();
void restart();

struct sigaction sa;


/*struttura che definisce le bandiere, visibile solo al master*/
typedef struct{
    int x;
    int y;
    int score;
}vexillum;


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


/*status dell'exit process*/
int status;

/*numero di semafori da attivare, può cambiare in base alle necessità*/
int sem_num;

/* struttura dati per i punteggi */
typedef struct{
    /* Array contenente i pids di tutti i processi giocatori creati*/
    int * pid;
    /* Array contenente i nomi di tutti i processi giocatori creati*/
    char * name;
    /* Array contenente il punteggio di tutti i processi giocatori*/
    int * score;
}score_table;

score_table * st;

/*metodo del round*/
void round();

/*puntatore alla struttura vexillum per le bandiere*/
vexillum * vex;

/* stampa la tabella del punteggio */
void stamp_score(score_table * t);

/* Genera il numero di bandierine*/
int getNumflag();

/* crea ed inizializza l'array di bandierine NB: usare prima getNumFlag() e free(target)*/  
vexillum * getVex(int numFlag);

/*variabile che dice se le bandiere sono state create*/
int flagcreated = 0;

msg_master msg;

struct sembuf sem;

/*specifica i round da fare*/
int rounds;

int actual_round;

int main(int argc, char * argv[]){
    int i;
    int opt;
    verbosity = 0;
    isDebug = 0;
    rounds = 1;

    /*Region: inizializzazione e rilevamento argomenti*/
    while ((opt = getopt(argc,argv,":if:cvdr")) != -1)
    {
        switch (opt)
        {
        case 'c':
            ParseFile(fopen(optarg,"r"));
            break;

        case 'v':
        verbosity = 1;
        break;

        case 'd':
        isDebug = 1;
        break;

        case 'r':
        rounds = optarg;
        default:
            break;
        }
    }
    
    
    cleaner = clean;
    placed = 0;
    logger = fopen("Master.log","a+");    
    logg("Started At: %s\n",__TIME__);
    init();
    
    logg("Inizializzazione Memoria Condivisa");
    /**
     * Region: Shared Memory Set & semaphores
     * TODO: c'è un problema con segmenti di shared memory che rimangono attivi, bisogna capire perchè
    */
    sem_init();
    table_start();
    shared_table_init();
    playergen(SO_NUM_G);

    
    /* Inizio operazioni relative al round*/
    /*while( Condizione alarm ||){
        numFlag = getNumflag();
        free(vex);
    }*/

    /*End-Region*/
    
    logg("End Of Execution");
    logg("Stopped at %s",__TIME__);
    cleaner();
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

    case SIGALRM:
    logg("Fine del round, Sospensione");
    clean();
    break;
    
    default:
        break;
    }
}


void clean(){
    int i;
    if(playercreated){
        clean_process();
    }
    logg("MASTER_CLEANER_LAUNCHED");
    shmdt(master_shared_table);
    shmctl(table,IPC_RMID,NULL);
    for(i = 0; i < sem_num; i++){
        semctl(semid,i,IPC_RMID);
    }
    msgctl(master_msgqueue,IPC_RMID,NULL);
    msgctl(player_msgqueue,IPC_RMID,NULL);
    free(st);
    free(vex);
    fclose(logger);
    
    
    
}

void clean_process(){
    int i;
    msg_cnt msg;
    for(i = 0; i < SO_NUM_G;i++){
        kill((pid_t)st->pid[i],SIGINT);
        msgrcv(master_msgqueue,&msg,sizeof(msg_cnt),0,MSG_INFO);
        if(msg.ask == 42){
            logg("Closed Player %d",i);
        }
        else{
            wait(NULL);
        }
    }
}

/*End Region*/

void stamp_score(score_table * t){
    int i;
	logg("PLAYER         SCORE\n");
	for(i = 0; i < SO_NUM_G; i++){
		logg("PLAYER %c   |   %d  \n", t -> name[i], t -> score[i]);
	}
}

/*Master methods*/

void init(){
    int i;
    st =  malloc(sizeof(score_table));
    st->name = (char)malloc(sizeof(char) * SO_NUM_G);
    st->pid = (int)malloc(sizeof(int)*SO_NUM_G);
    st->score = (int)malloc(sizeof(int)*SO_NUM_G);
    for(i = 0; i < SO_NUM_G; i++){
        st -> name[i] = (char)((int)'A' + i);
        st -> score[i] = 0;
    }

    logg("Impostazione maschere e segnali");
    /*Region: inizializzazione dei segnali*/
    bzero(&sa,sizeof(sa));
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    sa.sa_mask = mask;
    /*sa.sa_flags = SA_RESTART;*/ /*Questa flag fa si che dopo l'handling del segnale il codice riparta da dove interrotto*/
    sa.sa_flags = SA_NODEFER; /*Questa flag permette all'handler di generare altri segnali*/
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGALRM,&sa,NULL);
    sigset(SIGINT,handler);
    sa.sa_handler = handler;
    /*End-Region*/
}



int semid;
void sem_init(){
    int i;
    sem_num = 4; /*per adesso è di default*/
    if(sem_num < 3){sem_num = 3;} /*semafori necessari per il minimo funzionamento*/

    if((semid = semget(IPC_PRIVATE,sem_num,IPC_CREAT | IPC_EXCL | 0666)) == -1){
        error("errore nell'inizializzare il semaforo master",ECONNABORTED);
    }
    for(i = 0; i < sem_num; i++){
            if(semctl(semid,i,SETVAL,0) == -1){
            error("Error in semctl semaforo master",ECOMM);
        }
    }
    
}

int master_msgqueue;

void shared_table_init(){
    int x,y;
    if((master_shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
        error("Errore nell'attach della shared_table",EIO);
    }
    else{
        debug("Shared Table attach completato");
    }
    for(x = 0; x < SO_ALTEZZA;x++){
        for(y = 0; y < SO_BASE; y++){
            tab(master_shared_table,x,y)->id = EMPTY;
        }
    }
    board = master_shared_table;
    if((master_msgqueue = msgget(getpid(),IPC_CREAT | 0600)) == -1){
        error("Errore nella creazione della msgqueue master",EACCES);
    }

    logg("Memoria Condivisa Inizializzata");
}

void playergen(int playernum){
    /*Region: Process Creation*/
    int i;
    pid_t pid;
    playercreated = 1;
    for(i = 0; i < playernum; i++){
        if((pid = fork())){
            /*padre*/
            st -> pid[i] = pid;
            processSign = "Master";
            logg("Player: %d started with pid: %d",i,st ->pid[i]);
            sem.sem_num = MASTER_SEM;
            sem.sem_op = -1;
            semop(semid,&sem,1);
            /*attesa*/
        }
        else{
            /*figlio*/
            player_id = st -> name[i]; /* Assegnazione del nome al Player*/
            if(player() == -1){
                error("Errore nell'inizializzare il player",ECHILD);
            }
            exit(0);
        }
    }
}

int getNumflag(){
    int ln, i, numFlag;
    int * collection;
    srand(clock() + getpid());
    for(i =SO_FLAG_MIN, ln = 0; i <= SO_FLAG_MAX; i++){
        if((SO_ROUND_SCORE % i) == 0) ln++;
    }
    if(ln == 0){
        printf("ASSENZA DI UNA COMBINAZIONE PER CUI SCORE POSSA ESSERE DIVISO \n");
        numFlag = (SO_FLAG_MIN + rand()) % (SO_FLAG_MAX + 1 - SO_FLAG_MIN) + SO_FLAG_MIN;
    }else{
        printf("PRESENZA DI POSSIBILI COMBINAZIONI PER DIVISIONE INTERA \n"); 
        collection = malloc(ln * sizeof(collection));
        for(i = SO_FLAG_MIN, ln = 0; i <= SO_FLAG_MAX; i++){
            if(SO_ROUND_SCORE % i == 0){
                collection[ln] = i;
                ln++;
            }
        }
        numFlag = (0 + rand()) % ((ln - 1)+ 1 - 0) + 0;
        numFlag = collection[numFlag];
        printf("%d \n", numFlag);
    }
    return numFlag;
}

vexillum * getVex(int numFlag){
    vexillum * p;
    char flag;
    int i, x, y, r = SO_ROUND_SCORE % numFlag;
    p = (vexillum *)malloc(numFlag * sizeof(vexillum));
    for(i = 0; i < numFlag; i++){
        (p[i]).score = SO_ROUND_SCORE/numFlag;
        if(r != 0){ (p[i]).score = (p[i]).score + 1; r --;}
        for(flag = 0;flag;){
            x = (0 + rand()) % (SO_BASE + 1 - 0);
            y = (0 + rand()) % (SO_ALTEZZA + 1 - 0);
            if((getid(master_shared_table, x, y)) == EMPTY){
                p[i].x = x;
                p[i].y = y;  
                placeflag(master_shared_table, x, y);     
                flag = 1;
            }
        }
    }
    return p;
}

void round(){
        /*Region Phase-1:flag*/
        alarm(3);
        rounds++;
        msg.round = 1;
        msg.phase = 1;
        getVex(getNumflag());/*inizializza le bandiere*/
        msgsnd(master_msgqueue,&msg,sizeof(msg_master),MSG_COPY);
        sem.sem_num = PLAYER_SEM;
        sem.sem_op = 1;
        semop(semid,&sem,SO_NUM_G);

        sem.sem_num = MASTER_SEM;
        sem.sem_op = -1;
        semop(semid,&sem,SO_NUM_G);
    /*End-Region*/

    /*Region Phase-2:Indication*/

    /*End-Region*/

    /*Region Phase-3:Anarchy*/
    /*End-Region*/
    
}

void restart(){
    int i;
    for(i = 0; i < SO_NUM_G; i++){
        kill(st->pid[i] , SIGROUND);
    }
    semctl(semid,PLAYER_SEM,0);
    semctl(semid,PIECE_SEM,0);
    semctl(semid,MASTER_SEM,0);

    sem.sem_num = MASTER_SEM;
    sem.sem_op = -1;
    semop(semid,&sem,SO_NUM_G);
    round();
}


void manual(){
    printf("Gioco A pedine IDLE sviluppato da Mattia Borrelli e Ielacqua Matteo(DEV_SANS):\n\nEsecuzione: $master <command> -c <config.sc>\ncommand:\n-d attiva i messaggi di DEBUG\n-v attiva modalità verbosa !!! Rischio di console flooding\n-r int round, specifica quanti round si vuole che il gioco faccia\n");
}

/*End Region*/


