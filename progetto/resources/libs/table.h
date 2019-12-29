

#define _GNU_SOURCE
#ifndef SHM_H
#include <sys/shm.h>
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef STDIO_H
#include <stdio.h>
#endif
#ifndef STDLIB_H
#include <stdlib.h>
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
    int flag;
}cell;



/*metodo per muoversi nella tabella*/
cell * tab(cell * table, int x, int y);


/*id della table*/
int table;

/*id della tabella semaforica*/
int sem_table;

key_t sem_table_key;

/*inizializza la scacchiera in memoria condivisa*/
void table_start();

/*metodo usato per leggere l'id' di una cella*/
char getid(cell * shared_table, int x, int y);

/*metodo usato per settare l'id di una cella, ritorna 1 in caso di successo */
int setid(cell * shared_table,int x, int y, char id);

/*metodo usato dal master per piazzare le bandiere*/
void placeflag(cell * shared_table, int x, int y);

/*metodo usato dal master per rimuovere le bandiere*/
void removeflag(cell * shared_table, int x, int y);

struct sembuf sem_t;
#endif