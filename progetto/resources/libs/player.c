#ifndef PLAYER_H
#include "player.h"
#endif

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

/*metodo per gestire un round*/
void stand();

char filename[24];

int status;

int piececreated = 0;

struct sembuf sem;

msg_cnt cnt;
msg_master master;
int player()
{
    int i;
    processSign = "Player";
    pieces = (piece_type *)malloc(sizeof(piece_type) * SO_NUM_P);
    cleaner = player_clean;
    sprintf(filename, "Player %c.log", player_id);
    logg("Player Started At %s", __TIME__);
    if ((semglobal = semget(getppid(), semnum, IPC_EXCL | 0600)) == -1)
    {
        error("errore nel get del semaforo master", errno);
    }
    if ((semplayer = semget(getpid(), SO_NUM_P + 3, IPC_CREAT | IPC_EXCL | 0600)) == -1)
    {
        error("errore nella creazione del semaforo per il player", errno);
    }
    for (i = 0; i < SO_NUM_P + 3; i++)
    {
        initsemReserved(semplayer, i);
    }
    if ((sem_table = semget(sem_table_key, SO_BASE * SO_ALTEZZA, IPC_EXCL | 0600)) == -1)
    {
        error("Error nella creazione della tabella dei semafori", errno);
    }
    else
    {
        logg("Tabella semafori iniziata");
    }
    if ((player_shared_table = (cell *)shmat(table, NULL, 0)) == (void *)-1)
    {
        error("Errore nell'innesto della shared_table", EIO);
    }
    /*puntatore alla funzione player_clean da sfruttare con error()*/
    cleaner = player_clean;
    logg("Setup Struttura dei segnali");
    bzero(&player_mask, sizeof(player_mask));
    sigemptyset(&player_mask);
    sigaddset(&player_mask, SIGINT);
    sigaddset(&player_mask, SIGROUND);
    sigprocmask(SIG_BLOCK, &player_mask, NULL);
    player_signal.sa_mask = player_mask;
    player_signal.sa_flags = SA_RESTART;
    player_signal.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &player_signal, NULL);
    sigaction(SIGUSR1, &player_signal, NULL);
    sigaction(SIGUSR2, &player_signal, NULL);
    sigset(SIGINT, player_handler);
    sigset(SIGUSR1, player_handler);
    sigset(SIGUSR2, player_handler);
    player_signal.sa_handler = player_handler;
    /* Generazione chiave della coda per il controllo dei pezzi
       ereditata da ciascun pezzo (una coda per Player) */
    if ((key_MO = msgget(getpid(), IPC_CREAT | IPC_EXCL | 0600)) == -1)
    {
        error("Errore nella creazione della coda di controllo", errno);
    }

    master.type = 1; /*canale 1 per la comunicazione con il master*/
    piecegen(SO_NUM_P);
    /* Impostazioni tattica di gioco */;

    piececreated = 1;
    if (releaseSem(semglobal, MASTER_SEM))
        error("error in semop", errno);
    while (master.phase != -1)
    { /*la fase -1 determina l'interruzione del processo*/
        stand();
    }

    player_clean();
    return 0;
}

void stand()
{
    master.type = 1;
    msgsnd(master_msgqueue, &master, sizeof(msg_master), MSG_INFO);
    msgrcv(master_msgqueue, &master, sizeof(msg_master), ORDER_CHANNEL, MSG_INFO);
    logg("Received Message Phase:%d ", master.phase);
    phase(master.phase);
}

void phase(int phase)
{
    int i;
    switch (phase)
    {
    case 1: /*dice ad ogni pezzo di posizionarsi sulla scacchiera*/
        for (i = 0; i < SO_NUM_P; i++)
        {
            cnt.x = rand() % SO_ALTEZZA;
            cnt.y = rand() % SO_BASE;
            cnt.type = ORDER_CHANNEL;
            cnt.phase = 1;
            cnt.pednum = i;
            releaseSem(semplayer, PIECE_SEM + i);
            msgsnd(key_MO, &cnt, sizeof(msg_cnt), MSG_INFO);
            msgrcv(key_MO, &cnt, sizeof(msg_cnt), TACTIC_CHANNEL, MSG_INFO);
            pieces[i].x = cnt.x;
            pieces[i].y = cnt.y;
        }
        msgsnd(master_msgqueue, &master, sizeof(msg_master), MSG_INFO);
        break;

    case 2:

        sem.sem_num = MASTER_SEM;
        sem.sem_op = 1;
        semop(semglobal, &sem, 1);
        break;

    case 3:

        break;

    case ROUND_STOP:
        player_clean();
        break;

    default:
        stand();
        break;
    }
}

int piecegen(int numpieces)
{
    int i;
    player_pid = getpid();
    logg("Generazione pezzi iniziata");
    for (i = 0; i < numpieces; i++)
    {
        if ((pid = fork()))
        {
            /*player*/
            pieces[i].piece_id = pid;
            logg("Generato pezzo %d", i);
        }
        else
        {
            /*pieces*/
            piece_attr.piece_id = i;
            if (piece() == -1)
            {
                return -1;
                exit(1);
            }
        }
    }
    return 0;
}

void player_handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        player_clean();
        break;

    case SIGROUND:
        phase(ROUND_STOP);
        break;

    case SIGTACTIC:
        /*dare tattica alla pedina che la richiede*/
        break;

    default:
        break;
    }
}

void player_clean()
{
    int i;
    logg("PLAYER_CLEANER:Interruzione esecuzione in corso");
    if (piececreated)
    {
        for (i = 0; i < SO_NUM_P; i++)
        {
            kill(pieces[i].piece_id, SIGINT);
            wait(NULL);
        }
    }
    semctl(semglobal, 0, IPC_RMID);
    semctl(semplayer, 0, IPC_RMID);
    semctl(sem_table, 0, IPC_RMID);
    shmdt(player_shared_table);
    msgctl(key_MO, IPC_RMID, NULL);
    exit(0);
}
