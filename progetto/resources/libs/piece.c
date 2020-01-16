#ifndef PIECE_H
#include "piece.h"
#endif

int pos_set = 0;

/* Coordinata x dell'ultima cella visitata dalla pedina; aggiorna in move */
int old_x;
/* Coordinata x dell'ultima cella visitata dalla pedina; aggiorna in move */
int old_y;

/* Valore mai annullato della coordinata x precedentemente occupata */
int tmp_old_x;

/* Valore mai annullato della coordinata y precedentemente occupata */
int tmp_old_y;

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

position target;

int piece()
{

    processSign = "Piece";
    srand(time(NULL));
    if ((semglobal = semget(IPC_PRIVATE, semnum, 0600)) == -1)
    {
        error("errore nel semaforo", ECONNABORTED);
    }
    if ((sem_table = semget(sem_table_key, SO_BASE * SO_ALTEZZA, 0600)) == -1)
    {
        error("Error nella creazione della tabella dei semafori", errno);
    }
    if ((semplayer = semget(getppid(), SO_NUM_P + 2, 0600)) == -1)
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
    order.phase = 0;
    debug("Waiting message from Player");
    msgrcv(key_MO, &order, sizeof(msg_cnt) - sizeof(long), getpid(), MSG_INFO);
    debug("orders received piece %d phase %d", piece_attr.piece_id, order.phase);
    play(order.phase);
}

void play(int command)
{
    msg_cnt temp;
    switch (command)
    {
    case 1:
        if (pos_set)
            error("Cannot set pos 2 times", EBADR);
        while (!setpos(order.x, order.y))
        {
            debug("Posizionamento non riuscito Riprovo");
            srand(clock());
            order.x = rand() % SO_ALTEZZA;
            order.y = rand() % SO_BASE;
        }
        logg("Pezzo %d del player %d in X:%d Y:%d", piece_attr.piece_id, player_id, piece_attr.x, piece_attr.y);
        pos_set = 1;
        piece_attr.n_moves = SO_N_MOVES;
        temp.x = piece_attr.x;
        temp.y = piece_attr.y;
        temp.type = getppid() * 10;
        temp.pednum = piece_attr.piece_id;
        msgsnd(key_MO, &temp, sizeof(msg_cnt) - sizeof(long), MSG_INFO);
        break;

    case 2:
        target.x = order.x;
        target.y = order.y;
        piece_attr.strategy = order.strategy;
        debug("Target Acquired, Piece %d To X:%d Y:%d", piece_attr.piece_id, target.x, target.y);
        temp.type = getppid() * 10;
        msgsnd(key_MO, &temp, sizeof(msg_cnt) - sizeof(long), MSG_INFO);
        break;

    case 3:
        debug("Piece %d Start moving, with tactic %d", piece_attr.piece_id, order.strategy);
        tactic();
        break;

    default:
        break;
    }
}

void tactic()
{
    int result;
    char strategy = order.strategy;
    old_x = -1;
    old_y = -1;
    /* posizione provvisoria */ srand(clock() + getpid());
    while (piece_attr.n_moves > 0)
    {

        if (getid(piece_shared_table, target.x, target.y) != FLAG)
        {
            debug("Piece %d changing target", piece_attr.piece_id);
            target = search(piece_shared_table, piece_attr.x, piece_attr.y, FLAG);
            if ((reachable(piece_attr.n_moves, piece_attr.x, piece_attr.y, target.x, target.y) < 0))
            {
                debug("Insufficent moves for piece %d", piece_attr.piece_id);
                getplay();
            }
        }
        result = goto_loc(target.x, target.y, strategy);
        switch (result)
        {
        case 0:
            strategy = order.strategy;
            break;
        case 1:
            strategy = X_BEFORE;
            break;
        case 2:
            strategy = Y_BEFORE;
            break;
        default:
            break;
        }
    }
    debug("Moves Finished for piece %d", piece_attr.piece_id);
}

void piece_handler(int signum)
{
    msg_cnt temp;
    switch (signum)
    {
    case SIGINT:
        piece_cleaner();
        break;

    case SIGROUND:
        debug("Restart Execution");
        temp.type = getppid() * 10;
        msgsnd(key_MO, &temp, sizeof(msg_cnt) - sizeof(long), MSG_INFO);
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

int goto_loc(int target_x, int target_y, char strategy)
{
    int x = target_x, y = target_y, check;
    char left, right, down, up, result = 0, method = strategy, changeT = (char)((1 + rand()) % ((2)) + 1);
    for (; piece_attr.n_moves > 0 && (piece_attr.x != x || piece_attr.y != y);)
    {
        if (getid(piece_shared_table, target_x, target_y) != FLAG)
            return 0;
        ;
        switch (method)
        {
        /* Precedenza all'asse orizzontale */
        case Y_BEFORE:
            if (y != piece_attr.y)
            {
                if (piece_attr.y > y && (check = cond_free(piece_attr.x, piece_attr.y - 1)))
                {
                    check = move(piece_attr.x, piece_attr.y - 1);
                }
                else if (piece_attr.y < y && (check = cond_free(piece_attr.x, piece_attr.y + 1)))
                {
                    check = move(piece_attr.x, piece_attr.y + 1);
                }
            }
            else if (x != piece_attr.x)
            {
                method = X_BEFORE;
            }

            if (check == 0)
                method = EVASION_Y;
            else if (changeT > 0 && strategy == DIAGONAL)
                method = DIAGONAL;
            else if (changeT > 0 && strategy == CHAOS_THEORY)
                method = CHAOS_THEORY;
            break;

        /* Precedenza all'asse verticale*/
        case X_BEFORE:
            if (x != piece_attr.x)
            {
                if (piece_attr.x > x && (check = cond_free(piece_attr.x - 1, piece_attr.y)))
                {
                    check = move(piece_attr.x - 1, piece_attr.y);
                }
                else if (piece_attr.x < x && (check = cond_free(piece_attr.x + 1, piece_attr.y)))
                {
                    check = move(piece_attr.x + 1, piece_attr.y);
                }
            }
            else if (y != piece_attr.y)
            {
                method = Y_BEFORE;
            }

            if (check == 0)
                method = EVASION_X;
            else if (changeT > 0 && strategy == DIAGONAL)
                method = DIAGONAL;
            else if (changeT > 0 && strategy == CHAOS_THEORY)
                method = CHAOS_THEORY;
            break;

        /* Schivata da asse orizzontale */
        case EVASION_Y:
            changeT = 0;
            right = 0;
            left = 0;
            if (piece_attr.x >= x)
                left = 1;
            if (piece_attr.x <= x)
                right = 1;
            up = piece_attr.y > y ? 0 : 1;

            /* Schiva in basso*/
            if (cond(piece_attr.x - 1, piece_attr.y) && left)
            {
                x = piece_attr.x - 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE;
            }
            /* Schiva in alto perchè in basso è occupato*/
            else if (left && cond(piece_attr.x + 1, piece_attr.y))
            {
                x = piece_attr.x + 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE;
            }
            /* Schiva in alto */
            else if (cond(piece_attr.x + 1, piece_attr.y) && right)
            {
                x = piece_attr.x + 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE;
            }
            /* Schiva in basso perchè in alto è occupato*/
            else if (right && cond(piece_attr.x - 1, piece_attr.y))
            {
                x = piece_attr.x - 1;
                y = (piece_attr.y > y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                method = X_BEFORE;
            }
            /* Schiva verso sinistra (torna indietro) */
            else if (up && cond(piece_attr.x, piece_attr.y - 1))
            {
                y = piece_attr.y - 1;
                x = (piece_attr.x <= tmp_old_x ? (piece_attr.x - 1) : (piece_attr.x + 1));
            }
            /* Schiva verso destra (torna indietro) */
            else if (!up && cond(piece_attr.x, piece_attr.y + 1))
            {
                y = piece_attr.y + 1;
                x = (piece_attr.x >= tmp_old_x ? (piece_attr.x + 1) : (piece_attr.x - 1));
                result = 2;
            }
            /*Nessuna tattica trovata: disattiva blocco per movimento all'indietro */
            else
            {
                old_x = -1;
                old_y = -1;
            }
            break;

        /* Schivata da asse verticale */
        case EVASION_X:
            changeT = 0;
            down = 0;
            up = 0;
            if (piece_attr.y >= y)
                down = 1;
            if (piece_attr.y <= y)
                up = 1;
            right = piece_attr.x > x ? 0 : 1;

            /* Schiva verso sinistra */
            if (cond(piece_attr.x, piece_attr.y - 1) && down)
            {
                y = piece_attr.y - 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE;
            }
            /* Schiva verso destra perchè a sinistra è occupato */
            else if (down && cond(piece_attr.x, piece_attr.y + 1))
            {
                y = piece_attr.y + 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE;
            }
            /* Schiva verso destra */
            else if (cond(piece_attr.x, piece_attr.y + 1) && up)
            {
                y = piece_attr.y + 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE;
            }
            /* Schiva verso sinistra perchè a destra è occupato */
            else if (up && cond(piece_attr.x, piece_attr.y - 1))
            {
                y = piece_attr.y - 1;
                x = (piece_attr.x > x ? (piece_attr.x - 1) : (piece_attr.x + 1));
                method = Y_BEFORE;
            }
            /* Schiva verso il basso (torna indietro) */
            else if (right && cond(piece_attr.x - 1, piece_attr.y))
            {
                x = piece_attr.x - 1;
                y = (piece_attr.y >= tmp_old_y ? (piece_attr.y + 1) : (piece_attr.y - 1));
                result = 1;
                method = X_BEFORE;
            }
            /*Schiva verso l'alto (torna indietro) */
            else if (!right && cond(piece_attr.x + 1, piece_attr.y))
            {
                x = piece_attr.x + 1;
                y = (piece_attr.y <= tmp_old_y ? (piece_attr.y - 1) : (piece_attr.y + 1));
                result = 1;
                method = X_BEFORE;
            }
            /* Nessuna tattica trovata: disattiva blocco per movimento all'indietro */
            else
            {
                old_x = -1;
                old_y = -1;
            }

            break;
        /* Alterna il movimento sulle assi */
        case DIAGONAL:
            switch (changeT)
            {
            case 1:
                method = X_BEFORE;
                changeT = 2;
                break;
            case 2:
                method = Y_BEFORE;
                changeT = 1;
                break;
            }
            break;
        /* Estrae casualmente l'asse su cui muoversi */
        case CHAOS_THEORY:
            switch (changeT)
            {
            case 1:
                method = X_BEFORE;
                changeT = (char)((1 + rand()) % ((2)) + 1);
                break;
            case 2:
                method = Y_BEFORE;
                changeT = (char)((1 + rand()) % ((2)) + 1);
                break;
            }
            break;
        default:
            break;
        }
    }
    return result;
}

/* Verifica se la cella obiettivo è libera */

char cond_free(int x, int y)
{
    return cond_valid(x, y) && (getid(piece_shared_table, x, y) == EMPTY || getid(piece_shared_table, x, y) == FLAG) && semctl(sem_table,x*SO_BASE + y,GETVAL) == 1;
}

/* Verifica se la cella bersaglio non è stata già percorsa nell'immediato */
char cond_old(int x, int y)
{
    return old_x == x && old_y == y;
}

/* Verifica se la cella bersaglio non eccede i limiti della tabella */
char cond_valid(int x, int y)
{
    return (x > -1 && x < SO_BASE) && (y > -1 && y < SO_ALTEZZA);
}

/* Verifica che la cella bersaglio sia ottimale per lo spostamento */

char cond(int x, int y)
{
    return cond_free(x, y) && !cond_old(x, y);
}

int move(int x, int y)
{
    msg_cnt captured;
    struct timespec move, remain;
    int moved;
    int isValid = 0;
    logg("Moving piece %d to X:%d Y:%d, Remaining Moves: %d", piece_attr.piece_id, x, y,piece_attr.n_moves);
    move.tv_nsec = SO_MIN_HOLD_NSEC;
    move.tv_sec = 0;
    nanosleep(&move, &remain);
    isValid = ((piece_attr.x - x) <= 1 && ((piece_attr.x - x) >= -1) && ((piece_attr.y - y) <= 1 && (piece_attr.y - y) >= -1));
    if (isValid && piece_attr.n_moves >= 0)
    {
        if (isValid && pos_set)
        {
            moved = setid(piece_shared_table, x, y, player_id, piece_attr.x, piece_attr.y);
            if (moved == 1)
            {
                tmp_old_x = old_x = piece_attr.x;
                tmp_old_y = old_y = piece_attr.y;
                piece_attr.x = x;
                piece_attr.y = y;
                piece_attr.n_moves--;
                return 1;
            }
            else if (moved == -1)
            {
                debug("Capturing x:%d, y:%d", x, y);
                captured.type = getppid() * 10;
                captured.x = x;
                captured.y = y;
                captured.id = piece_attr.piece_id;
                msgsnd(key_MO, &captured, sizeof(msg_cnt) - sizeof(long), MSG_INFO);
                debug("Sended message to player");
                msgrcv(key_MO, NULL, sizeof(msg_cnt) - sizeof(long), getpid(), MSG_INFO);
                releaseSem(sem_table,piece_attr.x*SO_BASE + piece_attr.y);
                tab(piece_shared_table,piece_attr.x,piece_attr.y)->id = EMPTY;
                tab(piece_shared_table, x, y)->id = player_id;
                piece_attr.x = x;
                piece_attr.y = y;
                piece_attr.n_moves--;
                debug("Restart");
                return 1;
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
