#ifndef PIECE_H
#include "piece.h"
#endif

int pos_set = 0;

/* Coordinata x dell'ultima cella visitata dalla pedina; aggiorna in move */
int old_x;
/* Coordinata x dell'ultima cella visitata dalla pedina; aggiorna in move */
int old_y;

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
    if ((semglobal = semget(IPC_PRIVATE, semnum, IPC_EXCL | 0600)) == -1)
    {
        error("errore nel semaforo", ECONNABORTED);
    }
    if ((sem_table = semget(sem_table_key, SO_BASE * SO_ALTEZZA, IPC_EXCL | 0600)) == -1)
    {
        error("Error nella creazione della tabella dei semafori", errno);
    }
    if ((semplayer = semget(getppid(), SO_NUM_P + 3, IPC_EXCL | 0600)) == -1)
    {
        error("Errore nel get del semaforo player", errno);
    }
    else
    {
        logg("Tabella semafori iniziata");
    }
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
    piece_cleaner();
    exit(0);
    return 0;
}

void getplay()
{
    msg_cnt response;
    reserveSem(semplayer, PIECE_SEM + piece_attr.piece_id);
    msgrcv(key_MO, &order, sizeof(msg_cnt), ORDER_CHANNEL, MSG_INFO);
    debug("orders received piece %d phase %d", piece_attr.piece_id, order.phase);
    play(order.phase);
    response.x = piece_attr.x;
    response.y = piece_attr.y;
    response.type = TACTIC_CHANNEL;
    response.pednum = piece_attr.piece_id;
    msgsnd(key_MO, &response, sizeof(msg_cnt), MSG_INFO);
}

void play(int command)
{
    switch (command)
    {
    case 1:
        if (pos_set)
            error("Cannot set pos 2 times", EBADR);
        if (setpos(order.x, order.y))
        {
            logg("Pezzo %d del player %d in X:%d Y:%d", piece_attr.piece_id, player_id, piece_attr.x, piece_attr.y);
            pos_set = 1;
        }
        else
        {
            debug("Posizionamento non riuscito Riprovo");
            srand(clock());
            order.x = rand() % SO_ALTEZZA;
            order.y = rand() % SO_BASE;
            play(1);
        }
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
    old_x = -1;
    old_y = -1;
    while (piece_attr.n_moves != 0)
    {
        if (getid(piece_shared_table, target.x, target.y) != FLAG)
        {
            switch (order.ask)
            {
            case 0:
                target = search(piece_shared_table, piece_attr.x, piece_attr.y, FLAG);
                if (!(reachable(piece_attr.n_moves, piece_attr.x, piece_attr.y, target.x, target.y) > 0))
                {
                    target = NULL;
                    order.ask = -1;
                }
                break;
            case 1:
                /*creare il semaforo*/
                kill(getppid(), SIGTACTIC);
                order.x = piece_attr.x;
                order.y = piece_attr.y;
                order.type = TACTIC_CHANNEL;
                msgsnd(key_MO, &order, sizeof(msg_cnt) - sizeof(int), MSG_INFO);
                msgrcv(key_MO, &order, sizeof(msg_cnt) - sizeof(int), 4, MSG_INFO);
                target.x = order.x;
                target.y = order.y;
                order.type = ORDER_CHANNEL;
                break;
            default:
                stand();
            }
        }
        if (target != NULL)
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

int setpos(int x, int y)
{
    if (x < 0)
        x = x * -1;
    if (y < 0)
        y = y * -1;
    if (getid(piece_shared_table, x, y) == EMPTY && semctl(sem_table, x * SO_ALTEZZA + y, GETVAL) == 1)
    {
        setid(piece_shared_table, x, y, player_id, -1, -1);
        piece_attr.x = x;
        piece_attr.y = y;
        return 1;
    }
    else
    {
        return 0;
    }
}

int goto_loc(int target_x, int target_y, char method, int evade)
{
    int x = target_x, y = target_y, check;
    char random, left, right, down, up;
    for (; piece_move > 0 && (piece_attr.x != x || piece_attr.y != y);)
    {
        if (getid(piece_shared_table, target_x, target_y) != FLAG)
            return -1;
        ;
        switch (method)
        {
        case Y_BEFORE:
            if (y != piece_attr.y)
            {
                if (piece_attr.y > y && (check = (int)cond_free(piece_attr.x, piece_attr.y - 1)))
                {
                    old_x = piece_attr.x;
                    old_y = piece_attr.y;
                    check = move(piece_attr.x, piece_attr.y - 1);
                }
                else if (piece_attr.y < y && (check = (int)cond_free(piece_attr.x, piece_attr.y + 1)))
                {
                    old_x = piece_attr.x;
                    old_y = piece_attr.y;
                    check = move(piece_attr.x, piece_attr.y + 1);
                }
            }
            else if (x != piece_attr.x)
            {
                method = X_BEFORE;
            }

            if (check == 0)
                method = EVASION_Y;
            break;

        case X_BEFORE:
            if (x != piece_attr.x)
            {
                if (piece_attr.x > x && (check = (int)cond_free(piece_attr.x - 1, piece_attr.y)))
                {
                    old_x = piece_attr.x;
                    old_y = piece_attr.y;
                    check = move(piece_attr.x - 1, piece_attr.y);
                }
                else if (piece_attr.x < x && (check = (int)cond_free(piece_attr.x + 1, piece_attr.y)))
                {
                    old_x = piece_attr.x;
                    old_y = piece_attr.y;
                    check = move(piece_attr.x + 1, piece_attr.y);
                }
            }
            else if (y != piece_attr.y)
            {
                method = Y_BEFORE;
            }

            if (check == 0)
                method = EVASION_X;
            break;

        case EVASION_Y:
            right = 0;
            left = 0;
            if (piece_attr.x >= x)
                left = 1;
            if (piece_attr.x <= x)
                right = 1;
            up = piece_attr.y > y ? 0 : 1;

            if (cond(piece_attr.x - 1, piece_attr.y) && left)
            {
                x = piece_attr.x - 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE; // x - 1
            }
            else if (left && cond(piece_attr.x + 1, piece_attr.y))
            {
                x = piece_attr.x + 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE; // x + 1
            }
            else if (cond(piece_attr.x + 1, piece_attr.y) && right)
            {
                x = piece_attr.x + 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE; // x + 1
            }
            else if (right && cond(piece_attr.x - 1, piece_attr.y))
            {
                x = piece_attr.x - 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE; // x-1
            }
            else if (up && cond(piece_attr.x, piece_attr.y - 1))
            {
                y = piece_attr.y - 1;
                if (right == left)
                {
                    x = (piece_attr.x > old_x ? (piece_attr.x + 1) : (piece_attr.x - 1));
                    signal = 1;
                }
                else
                    x = (right ? (piece_attr.x + 1) : (piece_attr.x - 1));
                method = Y_BEFORE;
            }
            else if (!up && cond(piece_attr.x, piece_attr.y + 1))
            {
                y = piece_attr.y + 1;
                if (right == left)
                {
                    x = (piece_attr.x > old_x ? (piece_attr.x + 1) : (piece_attr.x + 1));
                    signal = 1;
                }
                else
                    x = (right ? (piece_attr.x + 1) : (piece_attr.x - 1));
                method = Y_BEFORE;
            }
            else
            {
                old_x = -1;
                old_y = -1;
            }
            break;

        case EVASION_X:
            down = 0;
            up = 0;
            if (piece_attr.y >= y)
                down = 1;
            if (piece_attr.y <= y)
                up = 1;
            right = piece_attr.x > x ? 0 : 1;

            if (cond(piece_attr.x, piece_attr.y - 1) && down)
            {
                y = piece_attr.y - 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE; // y - 1
            }
            else if (down && cond(piece_attr.x, piece_attr.y + 1))
            {
                y = piece_attr.y + 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE; // y + 1
            }
            else if (cond(piece_attr.x, piece_attr.y + 1) && up)
            {
                y = piece_attr.y + 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE; // y + 1
            }
            else if (up && cond(piece_attr.x, piece_attr.y - 1))
            {
                y = piece_attr.y - 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE; // y - 1
            }
            else if (right && cond(piece_attr.x - 1, piece_attr.y))
            {
                x = piece_attr.x - 1;
                if (up == down)
                {
                    y = (piece_attr_y > old_y ? (piece_attr_y + 1) : (piece_attr_y - 1));
                }
                else
                    y = (right ? (piece_attr.y + 1) : (piece_attr.y - 1));
                method = X_BEFORE;
            }
            else if (!right && cond(piece_attr.x + 1, piece_attr.y))
            {
                x = piece_attr.x + 1;
                if (up == down)
                {
                    y = piece_attr_y > old_y ? piece_attr_y + 1 : piece_attr_y - 1;
                }
                else
                    y = (up ? (piece_attr.y + 1) : (piece_attr.y - 1));
                method = X_BEFORE;
            }
            else
            {
                old_x = -1;
                old_y = -1;
            }

            break;
        default:
            break;
        }
    }
    return 1;
}

/* Verifica se la cella obiettivo è libera */
<<<<<<< HEAD
char cond_free(int x, int y){
    return (getid(piece_shared_table, x, y) == EMPTY || getid(piece_shared_table, x, y) == FLAG) && cond_valid(x,y);
}

/* Verifica se la cella bersaglio non è stata già percorsa nell'immediato */
char cond_old(int x, int y){
=======
char cond_free(x, y)
{
    return (getid(piece_shared_table, x, y) == EMPTY || getid(piece_shared_table, x, y) == FLAG) && cond_valid(x, y);
}

/* Verifica se la cella bersaglio non è stata già percorsa nell'immediato */
char cond_old(x, y)
{
>>>>>>> 4f14dafee43fb096bc4613e6300a9347d6880193
    return old_x == x && old_y == y;
}

/* Verifica se la cella bersaglio non eccede i limiti della tabella */
<<<<<<< HEAD
char cond_valid(int x, int y){
=======
char cond_valid(x, y)
{
>>>>>>> 4f14dafee43fb096bc4613e6300a9347d6880193
    return x < SO_BASE && y < SO_ALTEZZA;
}

/* Verifica che la cella bersaglio sia ottimale per lo spostamento */
<<<<<<< HEAD
char cond(int x, int y){
    return cond_free(x,y) && !cond_old(x,y);
=======
char cond(x, y)
{
    return cond_free(x, y) && !cond_old(x, y);
>>>>>>> 4f14dafee43fb096bc4613e6300a9347d6880193
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
                old_x = piece_attr.x;
                old_y = piece_attr.y;
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
