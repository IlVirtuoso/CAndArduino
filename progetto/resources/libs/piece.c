#ifndef PIECE_H
#include "piece.h"
#endif


int piece(){
    struct sembuf sem;
    processSign = "Piece";
    if((semid = semget(IPC_PRIVATE,3,IPC_EXCL)) == -1){
        error("errore nel semaforo",ECONNABORTED);
    }
    logger = fopen("Pieces.log","a+");
    logg("Piece %d of player %c Started At %s",piece_id,player_id,__TIME__);
    player_msgqueue = msgqueue;
    cleaner = piece_cleaner;
    logg("Setup Struttura dei segnali");
    bzero(&piece_mask,sizeof(piece_mask));
    piece_signal.sa_handler = piece_handler;
    sigemptyset(&piece_mask);
    sigaddset(&piece_mask,SIGINT);
    sigaddset(&piece_mask,SIGUSR1);
    sigaddset(&piece_mask,SIGUSR2);
    /*Questi due segnali serviranno per dare al player dei comandi addizionali*/
    sigprocmask(SIG_BLOCK,&piece_mask,NULL);
    piece_signal.sa_mask = piece_mask;
    piece_signal.sa_flags = SA_RESTART;
    piece_signal.sa_flags = SA_NODEFER;
    sigaction(SIGINT,&piece_signal,NULL);
    if((piece_shared_table = (cell*)shmat(table,NULL,0)) == (void*) - 1){
        error("Errore nell'inizializzare la table per il pezzo",EKEYREJECTED);
    }
    else{
        logg("Pezzo %d del player %c attaccato alla table",piece_id,player_id);

    } 
    sem.sem_num = PLAYER_SEM;
    sem.sem_op = 1;
    semop(semid,&sem,1);
    exit(0);
    return 0;
}

void piece_handler(int signum){
    switch (signum)
    {
    case SIGINT:
        piece_cleaner();
        exit(-1);
        break;

    case SIGUSR1:
        logg("Restart Execution");
        break;

    case SIGUSR2:

        break;
    
    default:
        break;
    }
}

void piece_cleaner(){
    shmdt(piece_shared_table);
    
}

void goto_loc(int x, int y, int method){
    switch (method)
    {
    case PROBABLE_LESS_COSTLY:
        
        break;
    
    case X_BEFORE_Y:

        break;

    case Y_BEFORE_X:

        break;

    case STRAIGHT_TO:

        break;
    
    default:
        break;
    }
}