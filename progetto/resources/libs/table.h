

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
    int semid;
    struct sembuf sem;
    
}cell;



/*metodo per muoversi nella tabella*/
cell * tab(cell * table, int x, int y);

/*id della table*/
int table;


/*inizializza la scacchiera in memoria condivisa*/
void table_start();
#endif