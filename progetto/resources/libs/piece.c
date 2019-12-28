#ifndef PIECE_H
#include "piece.h"
#endif
int pos_set = 0;



int piece(){
    struct sembuf sem;
    processSign = "Piece";

    sem.sem_num = PIECE_SEM;
    sem.sem_op = 1;
    semop(semid,&sem,1);
    if((semid = semget(IPC_PRIVATE,3,IPC_EXCL)) == -1){
        error("errore nel semaforo",ECONNABORTED);
    }
    logger = fopen("Pieces.log","a+");
    logg("Piece %d of player %c Started At %s",piece_attr.piece_id,player_id,__TIME__);
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
    sigaction(SIGINT,&piece_signal,NULL);
    if((piece_shared_table = (cell*)shmat(table,NULL,0)) == (void*) - 1){
        error("Errore nell'inizializzare la table per il pezzo",EKEYREJECTED);
    }
    else{
        logg("Pezzo %d del player %c attaccato alla table",piece_attr.piece_id,player_id);

    }
    
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
    /**
     * Handler per l'interruzione dell'esecuzione del metodo move, usato per cambiare strategia
     */
        break;
    
    default:
        break;
    }
}

void piece_cleaner(){
    shmdt(piece_shared_table);
    
}

void setpos(int x, int y){
    int dx,dy,pointx,pointy;
    cell * table = piece_shared_table;
    if(!pos_set){
        if(isfree(table,x,y)){
            ac(table,x,y);
            tab(table,x,y)->id = player_id - 65;
            piece_attr.x = x;
            piece_attr.y = y;
            pos_set = 1;
        }
        else{
            for(dx = -1; dx <= 1; dx++){
                for(dy = -1; dy <= 1; dy++){
                    pointx = x + dx;
                    pointy = y + dy;
                    if(pointx == -1) pointx = SO_ALTEZZA;
                    if(pointy == -1) pointy = SO_BASE;
                    else{
                        pointx = pointx % SO_ALTEZZA;
                        pointy = pointy % SO_BASE;
                    }
                    if(isfree(table,pointx,pointy)){
                        ac(table,x,y);
                        tab(table,x,y)->id = player_id - 65;
                        piece_attr.x = x;
                        piece_attr.y = y;
                        pos_set = 1;
                        break;
                    }
                }
            }
        }
    }
    else{
        error("Cannot set pos more times",EACCES);
    }
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

void move(int x, int y){
    int isvalid;
    cell * table = piece_shared_table;
    isvalid = ((piece_attr.x + x) == -1 || (piece_attr.x + x) == 1 || ((piece_attr.x + x)) == 0) && (((piece_attr.y + y) == -1) || ((piece_attr.y + y)) == 1 || ((piece_attr.y + y)) == 0);
    if(isvalid){
        rel(table,piece_attr.x,piece_attr.y);
        ac(table,x,y);
        tab(table,x,y)->id = player_id - 65;
        piece_attr.x = x;
        piece_attr.y = y;
    }
    else{
        error("Non puoi muoverti di più celle in una volta",EACCES);
    }
}