

#define _GNU_SOURCE
#ifndef SHM_H
#include <sys/shm.h>
#endif
#ifndef DEBUG_H
#include "debug.h"
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
#ifndef MACRO_H
#include "macro.h"
#endif
#ifndef TABLE_H
#define TABLE_H


/*definizione della struttura della cella della tabella*/
typedef struct{
    char id;
    int isFull;
    int hasaccess;
}cell;



/*metodo per muoversi nella tabella*/
cell * tab(cell * table, int x, int y);


/*id della table*/
int table;

/*id della tabella semaforica*/
int sem_table;

/*inizializza la scacchiera in memoria condivisa*/
void table_start();

/*metodo per il controllo degli accessi alla cella*/
int ac(cell * shared_table, int x, int y);

/*metodo per il rilascio dell'accesso alla cella*/
int rel(cell * shared_table, int x, int y);

/*metodo usato per leggere le flag di una cella*/
char getflag(cell * shared_table, int x, int y);

/*metodo usato dal master per piazzare le bandiere*/
void placeflag(cell * shared_table, int x, int y);

/*metodo usato dal master per rimuovere le bandiere*/
void removeflag(cell * shared_table, int x, int y);

/*metodo per sapere se una cella è o meno libera*/
int isfree(cell * shared_table, int x, int y);

struct sembuf sem_t;
#endif