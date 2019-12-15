#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>
#include "./resources/libs/parameters.h"
#include "./resources/libs/piece.h"
#include "./resources/libs/player.h"
#include <sys/shm.h>

#include "./resources/libs/macro.h"

int isDebug = 0;
FILE * logger;
int verbosity = 1;
/* segnala un errore e ne mostra la causa, poi invoca clean() per prepararsi all'uscita*/
void error(char message []);

/* funzione per mostrare il [debug] su console, attenzione la variabile isDebug deve essere 1 */
int debug(char message []);

/*handler per il segnale di interruzione SIGINT*/
void handler(int signum);

/*cleaner per SIGINT*/
void clean();

/*permette di mostrare lo stato della tabella, il metodo è costruito per funzionare a frame */
void display();

/*scrive su console e su un file un messaggio, utile per verificare la corretta esecuzione !!USATA SOLO DAL PROCESSO MASTER*/
void logg(char message []);

/*uccide tutti i processi inizializzati dal processo master*/
void killall();

struct sigaction sa;

/*semaforo*/
struct semaphore * sem;

/*definizione della struttura della cella della tabella*/
typedef struct _cell{
    char flag;
    int isFull;
    int semid;
    struct sembuf sem;
    
}cell;



/*id della scacchiera*/
int table; 

/*clock per misurare il tempo di esecuzione*/
clock_t cl;

/*mask per i segnali*/
static sigset_t mask;

/*segmento di memoria condivisa della table*/
cell * shared_table;

/* Array contenente i pids di tutti i processi giocatori creati*/
int players[SO_NUM_G];

/* variabile che dice se i giocatori sono stati creati*/
int playercreated = 0;

/*variabile che dice se i pezzi sono stati creati*/
int piececreated = 0;

/*variabile che dice se le bandiere sono state create*/
int flagcreated = 0;

int i;
int j;
int pid;
/*buffer per i messaggi custom*/
char logbuffer[64];
int main(int argc, char * argv[]){

    /*Region: inizializzazione e rilevamento argomenti*/
    for(i = 0; i < argc; i++){ /*inizializza la variabile di debug se richiesto*/
        if(strcmp(argv[i],"-d")) isDebug = 1;
        if(strcmp(argv[i],"-v")) verbosity = 2;
        if(strcmp(argv[i],"-vv")) verbosity = 3;
    }
    
    logger = fopen("Master.log","a+");
    fprintf(logger,"Started At: %s\n",__TIME__);
    
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
    if((table = shmget(IPC_PRIVATE,sizeof(cell)*SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666)) > 0){
        debug("Memoria Condivisa Inizializzata");
    }
    else{
        error("Errore nell'inizializzazione del segmento di memoria");
    }
    if((shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
        error("Errore nell'attach della shared_table");
    }
    else{
        debug("Shared Table attach completato");
    }
    logg("Setup dei semafori");
    for(i = 0; i < SO_BASE; i++){
        for(j = 0; j < SO_ALTEZZA; j++){
            shared_table[i + i*j].sem.sem_num = 1;
            if(j%2 == 0){
                shared_table[i + i*j].flag = 'c';
            }
        }
    }
    display();
    /*End-Region*/
    logg("Memoria Condivisa Inizializzata");

    /*Region: Process Creation*/
    for(i = 0; i < SO_NUM_G; i++){
        if((pid = fork())){
            /*padre*/
            players[i] = pid;
            sprintf(logbuffer,"Player: %d started with pid: %d",i,pid);
            logg(logbuffer);
        }
        else{
            /*figlio*/
            if((shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
                error("Errore nell'innesto della shared_table");
            }
            if(player() == -1){
                error("Errore nell'inizializzare il player");
            }
            exit(1);
        }
    }
    /*End-Region*/

    /*Region Phase-1:flag*/

    /*End-Region*/

    /*Region Phase-2:Indication*/

    /*End-Region*/

    /*Region Phase-3:Anarchy*/

    /*End-Region*/
    
    return 0;
}


void handler(int signum){
    if(signum == SIGINT){
        logg("Ricevuto Segnale SIGINT");
        clean();
    }
}

void clean(){
    shmdt(shared_table);
    shmctl(table,IPC_RMID,NULL);
    fclose(logger);
    if(playercreated){
        killall();
    }
    
}

int debug(char message []){
    if(isDebug){
        printf("[Debug]: %s\n", message);
        fprintf(logger,"[DEBUG]: %s\n",message);
        return 1;
    }
    else{
        return 0;
    }
}


int x; 
int y;
void display(){ 
    
    system("clear");
    for(x = 0 ; x < SO_BASE; x++){
        for(y = 0; y < SO_ALTEZZA; y++){
            printf("|%c|", shared_table[x + x*y].flag);
        }
        printf("\n");
    }
}

void error(char message[]){
    printf("[ERROR]: %s\n",message);
    fprintf(logger,"[ERROR]: %s\n",message);
    clean();
    exit(-1);
    
}

void logg(char message[]){
    double time = (double)clock()/1000;
    printf("[LOG: %f]%s\n",(double)time,message);
    fprintf(logger,"[LOG : %f] %s\n",(double)time,message);
    bzero(logbuffer,sizeof(logbuffer));

}

void killall(){
    for(i = 0; i < sizeof(players);i++){
        kill(players[i],SIGINT);
    }
}