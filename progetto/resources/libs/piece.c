#ifndef PIECE_H
#include "piece.h"
#endif
int pos_set = 0;



int piece(){
    char msg[128];
    struct sembuf sem;
    processSign = "Piece";
    read(playerpipe[0],msg,sizeof(msg));
    printf("%s",msg);
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
    sem.sem_num = PIECE_SEM;
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
    int dx,dy;
    cell * table = piece_shared_table;
    if(!pos_set){
        if(!tab(table,x,y)->isFull){
            tab(table,x,y)->id = player_id + 65 ; /*set dell'id a un carattere dell'alfabeto stampabile in base al numero del player*/
            tab(table,x,y)->isFull = 1;
            piece_attr.x = x;
            piece_attr.y = y;
            pos_set = 1;
        }
        else{
            for(dx = -1; dx < 2; dx++){
                for(dy = -1; dy < 2; dy++){ /*cerca nelle celle adiacenti se c'è una posizione libera*/
                    if(!tab(table,x+dx,y+dy)->isFull){ /*nel caso il pezzo trovi quella cella libera la occupa*/
                        tab(table,x,y)->id = player_id + 65 ; /*set dell'id a un carattere dell'alfabeto stampabile in base al numero del player*/
                        tab(table,x,y)->isFull = 1;
                        piece_attr.x = x+dx;
                        piece_attr.y = y+dy;
                        pos_set = 1;
                        break;
                    }
                }
            }
            if(!pos_set){
                setpos(x+3,y);/*se non trova una posizione libera prova ricorsivamente sulla stessa riga*/
            }        
        }
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