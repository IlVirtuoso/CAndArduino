
#ifndef PLAYER_H
#include "player.h"
#endif
char filename[24];

int player(){
    
    sprintf(filename,"Player %c.log", player_id);
    logger = fopen(filename,"a+");
    sprintf(logbuffer,"Player Started At %s",__TIME__);
    logg(logbuffer);
    if((player_shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
                error("Errore nell'innesto della shared_table",EIO);
            }
    /*puntatore alla funzione player_clean da sfruttare con error()*/
    cleaner = player_clean;
    piecegen(SO_NUM_P);
    return 0;

}

int i;



int piecegen(int numpieces){
    for(i = 0; (player_pid == getpid()) && (i < numpieces); i++){
        if((pid = fork())){
            /*player*/
            pieces[i] = pid;
        }
        
        else{
            /*pieces*/
            if(piece() == -1){
                return -1;
                exit(1);
            }
        }
    }
    return 0;
}



void player_handler(int signum){
    if(signum == SIGINT){
        player_clean();
    }
}

int i;
void player_clean(){
    logg("Interruzione esecuzione in corso");
    for(i = 0; i < sizeof(pieces); i++){
        kill(pieces[i],SIGINT);
    }
    fclose(logger);
    shmdt(player_shared_table);
    exit(0);
}

