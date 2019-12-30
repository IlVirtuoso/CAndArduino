#ifndef PLAYER_H
#include "player.h"
#endif

/* struct per message queue; default su MQ = 1 */ 
typedef struct msg_cnt{
    long type;
    char strategy;
    char x;
    char y;
    char ask;
}msg_cnt
/* NB: è possibile dividere il messaggio in diversi
 * char singoli in modo da avere più controllo sui
 * singoli elementi del messaggio:
 * es.
 *  char x       // coordinata x da raggiungere
 *  char y       // coordinata y da raggiungere 
 *  char ask     // attiva la richiesta al player in caso di blocco
 *  char method  // possibile valore di controllo per cambio algoritmo
 * In alternativa un unico array char ma confusione sugli indici?
 * La struct deve essere disponibile anche al Player
 */


char filename[24];

int status;


struct sembuf sem;

msg_cnt cnt;

int key_MO;

int player(){
    processSign = "Player";
    cleaner = player_clean;
    sprintf(filename,"Player %c.log", player_id);
    logger = fopen(filename,"a+");
    logg("Player Started At %s",__TIME__);
    if((semid = semget(IPC_PRIVATE,3,IPC_EXCL)) == -1){
        error("errore nel get del semaforo master",ECONNABORTED);
    }
    if((sem_table = semget(sem_table_key,SO_BASE*SO_ALTEZZA,IPC_EXCL)) == -1){
        error("Error nella creazione della tabella dei semafori",EACCES);
    }
    else{
        logg("Tabella semafori iniziata");
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

    /* Generazione chiave della coda per il controllo dei pezzi
       ereditata da ciascun pezzo (una coda per Player) */
    key_MO = msgget(getpid(), IPC_CREAT | 0600)
    /* Tipo di canale utilizzato per la coda (difficilmente 
       8 sarà utilizzato da qualcun altro o per errore) */
    msg_cnt.type = 8;

    /* Impostazioni tattica di gioco */

    sem.sem_num = PIECE_SEM;
    sem.sem_op = 1;
    semop(semid,&sem,SO_NUM_P);

    sem.sem_num = MASTER_SEM;
    sem.sem_op = 1;
    semop(semid,&sem,1);

    sem.sem_num = 4;
    sem.sem_op = -1;
    semop(semid,&sem,1);   

    pause();
    cleaner();

    return 0;

}

int piecegen(int numpieces){
    int i;
    player_pid = getpid();
    logg("Generazione pezzi iniziata");
    for(i = 0; i < numpieces; i++){
        if((pid = fork())){
            /*player*/
            pieces[i] = pid; 
            logg("Generato pezzo %d Attesa",i);
        
        }
        else{
            sleep(1);
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

void player_clean(){
    int i;
    logg("PLAYER_CLEANER:Interruzione esecuzione in corso");
    for(i = 0; i < sizeof(pieces); i++){
        kill(pieces[i],SIGINT);
        wait(NULL);
    }
    fclose(logger);
    shmdt(player_shared_table);
    msgctl(key_MO, IPC_RMID, NULL);
    exit(0);
}