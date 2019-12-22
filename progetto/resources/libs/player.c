#ifndef PLAYER_H
#include "player.h"
#endif
char filename[24];

int status;




int player(){

    sprintf(filename,"Player %c.log", player_id);
    logger = fopen(filename,"a+");
    logg("Player Started At %s",__TIME__);
    if((player_shared_table = (cell *)shmat(table,NULL,0)) == (void*) - 1){
                error("Errore nell'innesto della shared_table",EIO);
            }
    

    /*inizializzazione semaforo*/
    if((semid = semget(IPC_PRIVATE,1,IPC_CREAT | 0660)) == -1){
        error("Errore nell'inizializzare il semaforo",ECHILD);
    }
    semattr.val = 0;
    semctl(semid,0,SETVAL,semattr.val);
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
    cleaner();
    exit(0);
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
            waitpid(pid,NULL,WEXITED);
            logg("Generato pezzo %d Attesa",i);

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
    play_sem->sem_op = 1;
    semop(semid,play_sem,1);
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
    logg("PLAYER_CLEANER:Interruzione esecuzione in corso");
    for(i = 0; i < sizeof(pieces); i++){
        kill(pieces[i],SIGINT);
    }
    fclose(logger);
    shmdt(player_shared_table);
    exit(0);
}

