
#ifndef PLAYER_H
#include "player.h"
#endif
char filename[24];

int status;


int player(){
    sprintf(filename,"Player %c.log", player_id);
    logger = fopen(filename,"a+");
    logbuffer = player_logbuffer;
    sprintf(logbuffer,"Player Started At %s",__TIME__);
    logg(logbuffer);
    if((player_shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
                error("Errore nell'innesto della shared_table",EIO);
            }
    
    /*puntatore alla funzione player_clean da sfruttare con error()*/
    cleaner = player_clean;
    logg("Setup Struttura dei segnali");
    bzero(&player_mask,sizeof(player_mask));
    player_signal.sa_handler = player_handler;
    sigemptyset(&player_mask);
    sigaddset(&player_mask,SIGINT);
    sigprocmask(SIG_BLOCK,&player_mask,NULL);
    player_signal.sa_mask = player_mask;
    master_msgqueue = msgqueue;
  /*player_signal.sa_flags = SA_RESTART;
    player_signal.sa_flags = SA_NODEFER; da usare solo se servono*/
    sigaction(SIGINT,&player_signal,NULL);
    piecegen(SO_NUM_P);
    return 0;

}

int i;



int piecegen(int numpieces){
    logg("Generazione pezzi iniziata");
    player_msgqueue = message_start(IPC_PRIVATE);
    player_pid = getpid();
    for(i = 0; i < numpieces; i++){
        if((pid = fork())){
            /*player*/
            pieces[i] = pid;
            sprintf(logbuffer,"Generato pezzo %d Attesa",i);
            logg(logbuffer);
            waitpid(pid,&status,0);
        }
        
        else{
            /*pieces*/
            piece_id = i;
            if(piece() == -1){
                return -1;
                exit(1);
            }
        }
    }
    return 0;
}


void play(){
    
}



void player_handler(int signum){
    switch (signum)
    {
    case SIGINT:
        player_clean();
        break;
    
    case SIGUSR1:

    break;

    case SIGUSR2:

    break;
    
    default:
        break;
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

