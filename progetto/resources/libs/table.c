#ifndef TABLE_H
#include "table.h"
#endif



int setid(cell * shared_table,int x, int y, char id, int previous_x, int previous_y){
    if((tab(shared_table,x,y)->id == EMPTY ||tab(shared_table,x,y)->id == FLAG) && (semctl(sem_table,x*y + y,GETVAL) == 1)){
        if(previous_x >= 0 && previous_y >= 0){
                sem_t.sem_num = previous_x * previous_y + previous_y;
                sem_t.sem_op = 1;
            if((semop(sem_table,&sem_t,1)) == 0){
                debug("Rilascio eseguito");   
                }
            else{
                error("Errore nella semop durante la fase di rilascio",EACCES);
                }
    }
    else if(previous_x == -1 && previous_y == -1){
        debug("Set della posizione, ignora i semafori");
    }
    else{
            error("Errore nei parametri precedenti, sono a zero", ERANGE);
    }
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
    tab(shared_table,x,y)->id = id;
    return 1;
    }
    else{
        return 0;
    }
}


char getid(cell * shared_table, int x, int y){
   return tab(shared_table,x,y)->id;
}

cell * tab(cell * shared_table, int x, int y){
    return (&(*(shared_table + x*y + y)));
    }


void placeflag(cell * shared_table, int x, int y){
    if(strcmp(processSign,"Master")){
        tab(shared_table,x,y)->id = FLAG;
    }
}

void removeflag(cell * shared_table, int x, int y){
    if(strcmp(processSign,"Master")){
        tab(shared_table,x,y)->id = EMPTY;
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


position search(cell * shared_table,int b, int h, char target){
    int x = 1, y = 0, n = 0;
    char flag = 1, z = 0, sign = 1;
    position pos;
    pos.x = -1;
    pos.y = -1;
    while(flag && n < (SO_BASE * SO_ALTEZZA)){
        if(z == 0 && sign){ b++; y++; }        /* +x */
        else if(z == 1 && sign){ h++; y++; }   /* +y */
        else if(z == 0 && sign){ b--; y++; }   /* -x */
        else if(z == 1 && !sign){ h--; y++; }  /* -y */
        
        if(y == x){ z++; y = 0; }

        if(z == 2){ sign = !sign; z = 0; x++; y = 0; } 

        if( b >= 0 && b < SO_BASE){
            if( h >= 0 && h < SO_ALTEZZA){
                if( getid(shared_table , b , h) == target){
                    pos.x = b;
                    pos.y = h;
                    return pos;
                }
                n++;
            }
        }
    }
    return pos;
}

int getDistance(int x, int y, int x_targ, int y_targ){
    return ABSOLUTE(x - x_targ) + ABSOLUTE(y - y_targ);
}

int reachable(int moves, int x, int y, int x_targ, int y_targ){
    int res;
    if((res = getDistance(x, y, x_targ, y_targ)) <= moves){ return res;
    }else{ return -1; }
}
/*End Of Life*/



