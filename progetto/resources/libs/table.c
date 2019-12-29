#ifndef TABLE_H
#include "table.h"
#endif


int setid(cell * shared_table,int x, int y, char id){
    if((&(*(shared_table + x*y + y)))->id == EMPTY ||(&(*(shared_table + x*y + y)))->id == FLAG ){
    sem_t.sem_num = x*y + y;
    sem_t.sem_op = -1;
    semop(sem_table,&sem_t,1);
    (&(*(shared_table + x*y + y)))->id = id;
    sem_t.sem_num = x*y + y;
    sem_t.sem_op = 1;
    semop(sem_table,&sem_t,1);
    return 1;
    }
    else{
        return 0;
    }
}

char getid(cell * shared_table, int x, int y){
   return (&(*(shared_table + x*y + y)))->id;
}

cell * tab(cell * shared_table, int x, int y){
    return (&(*(shared_table + x*y + y)));
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



