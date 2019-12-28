#ifndef PLAYER_H
#include "player.h"
#endif
char filename[24];

int status;


struct sembuf sem;

char msg[128];


int player(){
    /**
     * TODO: bisogna capire come far funzionare la pipe tra i player e i loro pezzi
     * */
    processSign = "Player";
    pipe(playerpipe);
    sprintf(msg,"pezzi siete del giocatore %c",player_id);
    write(playerpipe[0],msg,sizeof(msg));
    cleaner = player_clean;
    sprintf(filename,"Player %c.log", player_id);
    logger = fopen(filename,"a+");
    logg("Player Started At %s",__TIME__);
    if((semid = semget(IPC_PRIVATE,3,IPC_EXCL)) == -1){
        error("errore nel get del semaforo master",ECONNABORTED);
    }
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
  /*player_signal.sa_flags = SA_RESTART;
    player_signal.sa_flags = SA_NODEFER; da usare solo se servono*/
    sigaction(SIGINT,&player_signal,NULL);
    piecegen(SO_NUM_P);

    sem.sem_num = PIECE_SEM;
    sem.sem_op = -1;
    semop(semid,&sem,SO_NUM_P);

    sem.sem_num = MASTER_SEM;
    sem.sem_op = 1;
    semop(semid,&sem,1);

    
    cleaner();

    return 0;

}

int i;



int piecegen(int numpieces){
    logg("Generazione pezzi iniziata");
    player_pid = getpid();
    for(i = 0; i < numpieces; i++){
        if((pid = fork())){
            /*player*/
            pieces[i] = pid; 
            logg("Generato pezzo %d Attesa",i);
        
        }
        else{
            /*pieces*/
            piece_attr.piece_id = i;
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
    logg("PLAYER_CLEANER:Interruzione esecuzione in corso");
    for(i = 0; i < sizeof(pieces); i++){
        kill(pieces[i],SIGINT);
        wait(NULL);
    }
    fclose(logger);
    shmdt(player_shared_table);
    exit(0);
}

