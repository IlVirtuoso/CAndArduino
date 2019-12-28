#ifndef TABLE_H
#include "table.h"
#endif
/*fatto per evitare un ciclo di inclusioni*/
#ifndef PIECE_H
#include "piece.h"
#endif

/*metodo locale per l'accesso alla tabella, nessuno può usarlo all'infuori dei metodi dichiarati qui*/
cell * ruler(cell * shareded_table,int x, int y);

/*Region metodi per il controllo della scacchiera*/


int ac(cell * shared_table,int x, int y){
    if(hasaccess){
        error("non puoi accedere ad un altra cella senza rilasciarne una",EACCES);
    }
    else{
        sem_t.sem_num = x*y + y;
        sem_t.sem_op = -1;
        semop(sem_table,&sem_t,1);
        hasaccess = 1;
        (&(*(shared_table + x*y + y)))->isFull = 1;
        
    }
    return 0;
}

int rel(cell * shared_table,int x, int y){
    if(hasaccess){
        sem_t.sem_num = x*y + y;
        sem_t.sem_op = 1;
        semop(sem_table,&sem_t,1);
        hasaccess = 0;
        (&(*(shared_table + x*y + y)))->isFull = 0;
    }
    else{
        error("Non puoi rilasciare una cella se non hai accesso a nessuna",EACCES);
    }
    return 0;
}

int isfree(cell * shared_table, int x, int y){
    return (&(*(shared_table + x*y + y)))->isFull;
}

cell * tab(cell * shared_table, int x, int y){
    if(semctl(sem_table,x*y + y,GETVAL) == 0 && hasaccess){
    return (&(*(shared_table + x*y + y)));
    }
    else if(semctl(sem_table,x*y + y,GETVAL) == -1){
        error("semctl fallita",EACCES);
    }
    else{
        error("non hai accesso a questo semaforo",EACCES);
    }
    return 0;
}

char getflag(cell * shared_table, int x, int y){
    return (&(*(shared_table + x*y + y)))->id;
}

void placeflag(cell * shared_table, int x, int y){
    if(strcmp(processSign,"Master")){
        (&(*(shared_table + x*y + y)))->id = FLAG;
    }
}

void removeflag(cell * shared_table, int x, int y){
    if(strcmp(processSign,"Master")){
        (&(*(shared_table + x*y + y)))->id = EMPTY;
    }
}

void table_start(){
    int i,j;
    if((table = shmget(IPC_PRIVATE,sizeof(cell)*SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666)) > 0){
        debug("Memoria Condivisa Inizializzata");
    }
    else{
        error("Errore nell'inizializzazione del segmento di memoria",EKEYREJECTED);
    }
    if((sem_table = semget(IPC_PRIVATE,SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666 |IPC_EXCL)) == -1){
        error("Error nella creazione della tabella dei semafori",EACCES);
    }
    else{
        logg("Tabella semafori iniziata");
    }
    for(i = 0; i < SO_ALTEZZA; i++){
        for(j = 0; j < SO_BASE; j++){
            if(semctl(sem_table,j, SETVAL, 1) == -1){
                error("error nel controllo della sem table",EACCES);
            }
        }
    }
    logg("tabella semafori configurata");
}
/*End Of Life*/



