#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/sem.h>

/*definizione della struttura della cella della tabella*/
typedef struct{
    char id;
    int isFull;
    int semid;
    struct sembuf sem;
    
}cell;


/*metodo per muoversi nella tabella*/
cell * tab(cell * table, int x, int y);