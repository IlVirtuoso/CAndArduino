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
#include "./resources/libs/macro.h"

int isDebug = 0;

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


struct sigaction sa;

/*definizione della struttura della cella della tabella*/
typedef struct _cell{
    char flag;
    int isFull;

}cell;



cell table[SO_BASE][SO_ALTEZZA];
key_t tablekey; /*chiave per l'accesso alla tavola*/

static sigset_t mask;/*mask per i segnali*/

struct shmseg * shared_table;/*segmento di memoria condivisa della table*/

int players[SO_NUM_G];/* Array contenente i pids di tutti i processi giocatori creati*/



int i;
int pid;
int main(int argc, char * argv[]){

    /*Region: inizializzazione e rilevamento argomenti*/
    for(i = 0; i < argc; i++){ /*inizializza la variabile di debug se richiesto*/
        if(argv[i] == "-d") isDebug = 1;
    }
    /*End-Region*/


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
    

    /*Region: Shared Memory Set*/
    if(tablekey = shmget(IPC_PRIVATE,sizeof(table),IPC_CREAT)){
        debug("Memoria Condivisa Inizializzata");
    }
    else{
        error("Errore nell'inizializzazione del segmento di memoria");
    }
    shared_table = (struct shmseg *)shmat(tablekey,NULL,0);
    if((void*)shared_table == -1){
        error("Errore nell'innesto della shared_table");
    }
    /*End-Region*/

    /*Region: Process Creation*/
    for(i = 0; i < SO_NUM_G; i++){
        if(pid = fork()){
            /*padre*/
            players[i] = pid;
        }
        else{
            /*figlio*/
            if(player() == -1){
                error("Errore nell'inizializzare il player");
            }
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
        clean();
    }
}

void clean(){
    shmdt(shared_table);
    shmctl(tablekey,IPC_RMID,NULL);
    
}

int debug(char message []){
    if(isDebug == 1){
        printf("[Debug]: %s", message);
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
            printf("|%c|", 'a');
        }
        printf("\n");
    }
}

void error(char message[]){
    printf("[ERROR]: %s",message);
    clean();
    exit(99);
}