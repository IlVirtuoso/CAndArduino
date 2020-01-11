#ifndef PIECE_H
#include "piece.h"
#endif
int pos_set = 0;

/*metodi per gestire il round*/
void play();
void getplay();

/*aspetta una tattica dal player e la esegue*/
void tactic();

/*variabile per l'Override del movimento, utilizzabile per esempio per l'interruzione di un round*/
int override;

struct sembuf sem;
/* Struttura adibita a ricevere i comandi tramite MQ */
msg_cnt order;
/* Struttura adibita a memorizzare le coordinate della cella target */
position target;

int piece()
{

    processSign = "Piece";

    srand(time(NULL));
    if ((semglobal = semget(IPC_PRIVATE, 3, IPC_EXCL)) == -1)
    {
        error("errore nel semaforo", ECONNABORTED);
    }
    if ((sem_table = semget(sem_table_key, SO_BASE * SO_ALTEZZA, IPC_EXCL)) == -1)
    {
        error("Error nella creazione della tabella dei semafori", EACCES);
    }
    if ((semplayer = semget(getppid(), 3, IPC_EXCL)) == -1)
    {
        error("Errore nel get del semaforo player", EACCES);
    }
    else
    {
        logg("Tabella semafori iniziata");
    }
    logger = fopen("Pieces.log", "a+");
    logg("Piece %d of player %c Started At %s", piece_attr.piece_id, player_id, __TIME__);
    cleaner = piece_cleaner;
    logg("Setup Struttura dei segnali");
    bzero(&piece_mask, sizeof(piece_mask));
    piece_signal.sa_handler = piece_handler;
    sigemptyset(&piece_mask);
    sigaddset(&piece_mask, SIGINT);
    sigaddset(&piece_mask, SIGROUND);
    sigaddset(&piece_mask, SIGUSR2);
    /*Questi due segnali serviranno per dare al player dei comandi addizionali*/
    sigprocmask(SIG_BLOCK, &piece_mask, NULL);
    piece_signal.sa_mask = piece_mask;
    sigaction(SIGINT, &piece_signal, NULL);
    sigaction(SIGROUND, &piece_signal, NULL);
    sigset(SIGINT, piece_handler);
    sigset(SIGROUND, piece_handler);
    if ((piece_shared_table = (cell *)shmat(table, NULL, 0)) == (void *)-1)
    {
        error("Errore nell'inizializzare la table per il pezzo", EKEYREJECTED);
    }
    while (order.phase != -1)
    {
        getplay();
    }
    return 0;
}

void getplay()
{
    do
    {
        msgrcv(key_MO, &order, sizeof(msg_cnt), 8, MSG_COPY & IPC_NOWAIT);
    } while (order.pednum != piece_attr.piece_id);

    msgrcv(key_MO, &order, sizeof(msg_cnt), 8, MSG_INFO);
    play(order.phase);
}

void play(int command)
{
    switch (command)
    {
    case 1:
        logg("Ordine ricevuto, Pedina: %d in X:%d e Y:%d", order.pednum, order.x, order.y);
        setpos(order.x, order.y);
        logg("Pezzo %d del player %d in X:%d Y:%d", piece_attr.piece_id, player_id, piece_attr.x, piece_attr.y);
        piece_attr.n_moves = SO_N_MOVES;
        break;

    case 2:

        break;
    default:
        break;
    }
}

void tactic()
{
    while (piece_attr.n_moves != 0)
    {
        if (getid(piece_shared_table, target.x, target.y) != FLAG)
        {
            switch (order.ask)
            {
            case 0:
                target = search(piece_shared_table, piece_attr.x, piece_attr.y, FLAG);
                break;
            case 1:
                /*creare il semaforo*/
                kill(getppid(), SIGTACTIC);
                order.x = piece_attr.x;
                order.y = piece_attr.y;
                order.type = 4;
                msgsnd(key_MO, &order, sizeof(msg_cnt) - sizeof(int), MSG_INFO);
                msgrcv(key_MO, &order, sizeof(msg_cnt) - sizeof(int), 4, MSG_INFO);
                target.x = order.x;
                target.y = order.y;
                order.type = 8;
                break;
            }
        }
        goto_loc(target.x, target.y, order.strategy, 1);
    }
}

void piece_handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        piece_cleaner();
        break;

    case SIGROUND:
        logg("Restart Execution");
        override = 1;
        getplay();
        break;

    case SIGUSR2:

        break;

    default:
        break;
    }
}

void piece_cleaner()
{
    logg("cleaning and go");
    shmdt(piece_shared_table);
    semctl(semglobal, 0, IPC_RMID);
    semctl(semplayer, 0, IPC_RMID);
    semctl(sem_table, 0, IPC_RMID);
    msgctl(key_MO, IPC_RMID, 0);
    exit(EXIT_SUCCESS);
}

void setpos(int x, int y)
{
    if (getid(piece_shared_table, x, y) == EMPTY)
    {
        setid(piece_shared_table, x, y, player_id, -1, -1);
        piece_attr.x = x;
        piece_attr.y = y;
    }
    else
    {
        setpos(rand() % SO_ALTEZZA, rand() % SO_BASE);
    }
}

int goto_loc(int x, int y, char method, char evasion)
{
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
        if (y != piece_attr.y)
        {
            if (piece_attr.y > y)
                check = move(piece_attr.x, piece_attr.y - 1);
            else
                check = move(piece_attr.x, piece_attr.y + 1);
        }
        else if (x != piece_attr.x)
        {
            check = goto_loc(x, y, X_BEFORE_Y, 1);
        }

        if (check == 0 && evasion)
            goto_loc(x, y, EVASION_Y, 1);
        else if (check == 0 && !evasion)
            return 0;

        /* Gestione dell'evasione alla macro EVASION_Y */

        break;

    /* Precedenza al movimento orizzontale */
    case X_BEFORE_Y:
        if (x != piece_attr.x)
        {
            if (piece_attr.x > x)
                check = move(piece_attr.x - 1, piece_attr.y);
            else
                check = move(piece_attr.x + 1, piece_attr.y);
        }
        else if (y != piece_attr.y)
        {
            check = goto_loc(x, y, Y_BEFORE_X, 1);
        }

        if (check == 0 && evasion)
            goto_loc(x, y, EVASION_X, 1);
        else if (check == 0 && !evasion)
            return 0;

        /* Gestione dell'evasione alla macro EVASION_X */

        break;

    case STRAIGHT_TO:
        break;

    /* Estrazione casuale tra movimento orizzontale e verticale*/
    case CAOS_POWER:
        random = (0 + rand()) % ((1 - 1) + 1 - 0) + 0;
        switch (random)
        {
        case 0:
            goto_loc(x, y, Y_BEFORE_X, 1);
            break;
        case 1:
            goto_loc(x, y, X_BEFORE_Y, 1);
            break;
        }
        break;

    /* Gestione dell'evasione in caso di casella occupata vericalmente*/
    case EVASION_Y:
        if ((x - piece_attr.x) > 0)
        {
            check = goto_loc(piece_attr.x + 1, piece_attr.y + 1, X_BEFORE_Y, 0);
            if (check == 0)
            {
                if ((goto_loc(piece_attr.x - 1, piece_attr.y - 1, X_BEFORE_Y, 0)) == 0)
                    return 0;
            }
        }
        else
        {
            check = ((goto_loc(piece_attr.x - 1, piece_attr.y - 1, X_BEFORE_Y, 0)) == 0);
            if (check == 0)
            {
                if ((goto_loc(piece_attr.x + 1, piece_attr.y + 1, X_BEFORE_Y, 0)) == 0)
                    return 0;
            }
        }
        break;

    /* Gestione dell'evasione in caso di casella occupata orizzontalmente*/
    case EVASION_X:
        if ((y - piece_attr.y) > 0)
        {
            check = goto_loc(piece_attr.x + 1, piece_attr.y + 1, Y_BEFORE_X, 0);
            if (check == 0)
            {
                if ((goto_loc(piece_attr.x - 1, piece_attr.y - 1, Y_BEFORE_X, 0)) == 0)
                    return 0;
            }
        }
        else
        {
            check = goto_loc(piece_attr.x - 1, piece_attr.y - 1, Y_BEFORE_X, 0);
            if (check == 0)
            {
                if ((goto_loc(piece_attr.x + 1, piece_attr.y + 1, Y_BEFORE_X, 0)) == 0)
                    return 0;
            }
        }
        break;

    /*non ti muovere fino a nuovo ordine*/
    case STAND:
        break;

    default:
        break;
    }
    return 1;
}

int move(int x, int y)
{
    struct timespec move, remain;
    int moved;
    int isValid = 0;
    move.tv_nsec = SO_MIN_HOLD_NSEC;
    move.tv_sec = 0;
    nanosleep(&move, &remain);
    if (override)
    {
        isValid = 1;
        override = 0;
    }
    else
        isValid = ((piece_attr.x - x) <= 1 && ((piece_attr.x - x) >= -1) && ((piece_attr.y - y) <= 1 && (piece_attr.y - y) >= -1));
    if (isValid && piece_attr.n_moves >= 0)
    {
        if (isValid && pos_set)
        {
            moved = setid(piece_shared_table, x, y, player_id, piece_attr.x, piece_attr.y);
            if (moved)
            {
                piece_attr.x = x;
                piece_attr.y = y;
                piece_attr.n_moves--;
                return 1;
            }
            else if (moved == -1)
            {
                capture(piece_shared_table, x, y, player_id);
                piece_attr.x = x;
                piece_attr.y = y;
                piece_attr.n_moves--;
            }
            else
            {
                return 0;
            } /*questo fa partire la gestione evasion*/
        }
        else
        {
            error("Posizione iniziale della pedina non settata", EBADR);
            return 0;
        }
    }
    else if (piece_attr.n_moves == 0)
        logg("Questa pedina ha finito le mosse");
    else
    {
        error("Non ti puoi muovere di due celle nella stessa manovra", EBADR);
        return -1;
    }
    return -1;
}
