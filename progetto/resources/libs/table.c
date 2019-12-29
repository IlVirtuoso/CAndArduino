#ifndef TABLE_H
#include "table.h"
#endif


int setid(cell * shared_table,int x, int y, char id){
    if(((&(*(shared_table + x*y + y)))->id == EMPTY ||(&(*(shared_table + x*y + y)))->id == FLAG) && (semctl(sem_table,x*y + y,GETVAL) == 1)){
    debug("Accesso alla cella X:%d Y:%d",x,y);
    sem_t.sem_num = x*y + y;
    sem_t.sem_op = -1;
    debug("Semop");
    if((semop(sem_table,&sem_t,1)) == 0){
     debug("Accesso Acquisito");   
    }
    else{
        error("Errore nella semop",EACCES);
    }
    (&(*(shared_table + x*y + y)))->id = id;
    sem_t.sem_num = x*y + y;
    sem_t.sem_op = 1;
    if((semop(sem_table,&sem_t,1)) == 0){
     debug("Rilascio eseguito");   
    }
    else{
        error("Errore nella semop",EACCES);
    }
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
    sem_table_key = ftok("./master.c",'a');
    if((table = shmget(IPC_PRIVATE,sizeof(cell)*SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666)) > 0){
        debug("Memoria Condivisa Inizializzata");
    }
    else{
        error("Errore nell'inizializzazione del segmento di memoria",EKEYREJECTED);
    }
    if((sem_table = semget(sem_table_key,SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666 |IPC_EXCL)) == -1){
        error("Error nella creazione della tabella dei semafori",EACCES);
    }
    else{
        logg("Tabella semafori iniziata");
    }
    for(i = 0; i < SO_ALTEZZA; i++){
        for(j = 0; j < SO_BASE; j++){
            if(semctl(sem_table,i*j + j, SETVAL, 1) == -1){
                error("error nel controllo della sem table",EACCES);
            }
        }
    }
    logg("tabella semafori configurata");
}
/*End Of Life*/



