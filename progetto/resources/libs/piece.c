#ifndef PIECE_H
#include "piece.h"
#endif
int pos_set = 0;



int piece(){
    
    struct sembuf sem;
    /* Struttura adibita a ricevere i comandi tramite MQ */
    msg_cnt order;
    /* Struttura adibita a memorizzare le coordinate della cella target */
    position target;

    processSign = "Piece";
    /* Esperimento sul funzionamento della coda di controllo*/
    /*msgrcv(key_MO, &order, sizeof(msg_cnt) - sizeof(long), 8, IPC_NOWAIT);
    printf("%c \n %c \n %c \n %c \n", order.strategy, order.x, order.y, order.ask);*/

    srand(time(NULL));
    if((semid = semget(IPC_PRIVATE,3,IPC_EXCL)) == -1){
        error("errore nel semaforo",ECONNABORTED);
    }
    if((sem_table = semget(sem_table_key,SO_BASE*SO_ALTEZZA,IPC_EXCL)) == -1){
        error("Error nella creazione della tabella dei semafori",EACCES);
    }
    else{
        logg("Tabella semafori iniziata");
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


    
    msgrcv(key_MO, &order, sizeof(msg_cnt),8, MSG_INFO);
    logg("Ordine ricevuto, Pedina: %d in X:%d e Y:%d",order.pednum,order.x,order.y);
    setpos(order.x,order.y);
    logg("Pezzo %d del player %d in X:%d Y:%d",piece_attr.piece_id,player_id,piece_attr.x,piece_attr.y);
    piece_attr.n_moves = SO_N_MOVES;

     /* Attesa della tattica */
    target = search(piece_shared_table,piece_attr.x,piece_attr.y,FLAG);

     while(piece_attr.n_moves <= SO_N_MOVES){
         if(getid(piece_shared_table, target.x, target.y) != FLAG){
             switch(order.ask){
                 case 0: /* search new target */ break;
                 case 1: /* invia signal a player per nuovo target */ break;
             }
         }
         goto_loc(target.x, target.y, order.strategy, 1);
     }

    sem.sem_num = PIECE_SEM;
    sem.sem_op = -1;
    semop(semid,&sem,1);

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


void setpos(int x, int y){
    
   if(setid(piece_shared_table,x,y,player_id,-1,-1) == 0){
       debug("Posizione Occupata");
   }
  else{
       piece_attr.x = x; 
       piece_attr.y = y;
   }
}

int goto_loc(int x, int y, char method, char evasion){
    /* Variabile check: successo dello spostamento
     * -1: insuccesso
     *  0: cella bloccata
     *  1: successo
     */ 
    int random, check;
    switch (method)
    {
    case PROBABLE_LESS_COSTLY:
        /* Effettua la ricerca e seleziona una nuova tattica? */
        break;
    
    /* Precedenza al movimento verticale */
    case Y_BEFORE_X:
        if(y != piece_attr.y){
            if(piece_attr.y > y) check = move(piece_attr.x, piece_attr.y - 1);
            else check = move(piece_attr.x, piece_attr.y + 1);
        }else if(x != piece_attr.x){
            check = goto_loc(x, y, X_BEFORE_Y, 1);
        }

        if(check == 0 && evasion) goto_loc(x,y, EVASION_Y, 1);
        else if(check == 0 && !evasion) return 0;

        /* Gestione dell'evasione alla macro EVASION_Y */ 

        break;

    /* Precedenza al movimento orizzontale */
    case X_BEFORE_Y:
        if(x != piece_attr.x){
            if(piece_attr.x > x) check = move(piece_attr.x - 1, piece_attr.y);
            else check = move(piece_attr.x + 1, piece_attr.y);
        }else if(y != piece_attr.y){
            check = goto_loc(x, y, Y_BEFORE_X, 1);
        }

        if(check == 0 && evasion) goto_loc(x,y,EVASION_X, 1);
        else if(check == 0 && !evasion) return 0;

        /* Gestione dell'evasione alla macro EVASION_X */

        break;

    case STRAIGHT_TO:
        break;
    
    /* Estrazione casuale tra movimento orizzontale e verticale*/
    case CAOS_POWER:
        random = (0 + rand()) % ((1 - 1)+ 1 - 0) + 0;
        switch(random){
            case 0:  goto_loc(x, y, Y_BEFORE_X, 1); break;
            case 1:  goto_loc(x, y, X_BEFORE_Y, 1); break;
        }
        break;
    
    /* Gestione dell'evasione in caso di casella occupata vericalmente*/
    case EVASION_Y:
        if((x - piece_attr.x) >= 0){ 
            check = goto_loc(piece_attr.x + 1, piece_attr.y + 1, X_BEFORE_Y, 0);
            if(check == 0){ 
                if((goto_loc(piece_attr.x - 1, piece_attr.y -1, X_BEFORE_Y, 0)) == 0) return 0;}
        } else{
             check = ((goto_loc(piece_attr.x - 1, piece_attr.y - 1, X_BEFORE_Y,0)) == 0);
             if(check == 0){ 
                 if((goto_loc(piece_attr.x + 1, piece_attr.y + 1, X_BEFORE_Y, 0)) == 0) return 0;}
        }
        break;

    /* Gestione dell'evasione in caso di casella occupata orizzontalmente*/
    case EVASION_X:
        if((y - piece_attr.y) >= 0){ 
            check = goto_loc(piece_attr.x + 1, piece_attr.y + 1, Y_BEFORE_X, 0);
            if(check == 0){ 
                if((goto_loc(piece_attr.x - 1, piece_attr.y -1, Y_BEFORE_X, 0)) == 0) return 0;
            }
        } else{ 
            check = goto_loc(piece_attr.x - 1, piece_attr.y - 1, Y_BEFORE_X, 0);
            if(check == 0){ 
                if((goto_loc(piece_attr.x + 1, piece_attr.y + 1, Y_BEFORE_X, 0)) == 0) return 0; }
        }
        break;

    default:
        break;
    }
    return 1;
}

int move(int x, int y){
    int isValid = 0;
    isValid = ((piece_attr.x - x) <= 1 && ((piece_attr.x - x) >=-1) && ((piece_attr.y - y) <= 1 && (piece_attr.y -y) >= -1));
    if(isValid){
        if(pos_set){
            setid(piece_shared_table,x,y,player_id,piece_attr.x,piece_attr.y);
            piece_attr.x = x;
            piece_attr.y = y;
            return 1;
        }
        else{
            error("Posizione iniziale della pedina non settata",EBADR);
            return 0;
        }
    }
    else
    {
        error("Non ti puoi muovere di due celle nella stessa manovra",EBADR);
        return 0;
    }
    
}
