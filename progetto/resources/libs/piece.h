#define _GNU_SOURCE
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef SHM_H
#include <sys/shm.h>
#endif
#ifndef TABLE_H
#include "table.h"
#endif
#ifndef SIGNAL_H
#include <signal.h>
#endif
#ifndef MONITOR_H
#include "monitor.h"
#endif
#ifndef MESSAGE_H
#include "message.h"
#endif

#ifndef PIECE_H
#define PIECE_H
/*funzione che inizializza la pedina*/
int piece();



struct{
    int piece_id; /*id del pezzo, tipicamente è la sua posizione nell'array dei pezzi*/
    int x;
    int y; /*posizione della pedina*/ 
    int n_moves; /* numero mosse disponibili*/
}piece_attr;

char player_id;

/*messagequeue con il player*/
int player_msgqueue;

void piece_handler(int signum);

void piece_cleaner();

/*variabile di controllo, dice se ha o meno accesso a una cella della tabella*/
int hasaccess;

/*segmento di memoria condivisa della table*/
cell * piece_shared_table;

/*funzionano come sempre*/
sigset_t piece_mask;

/*buffer per i messaggi modificati*/
char piece_logbuffer[128];

/**
 * muove la pedina fino al punto da raggiungere
 * @param x  intero che indica la riga
 * @param y  intero che indica la colonna
 * @param method char che indica che metodo usare nello switch per raggiungere quel punto
 */
int goto_loc(int x, int y, char method);

/* Verifica se la cella obiettivo è libera */
char cond_free(x, y);

/* Verifica se la cella bersaglio non è stata già percorsa nell'immediato */
char cond_old(x, y);

/* Verifica se la cella bersaglio non eccede i limiti della tabella */
char cond_valid(x, y);

/* Verifica che la cella bersaglio sia ottimale per lo spostamento */
char cond(x,y);

int move(int x, int y);

struct sigaction piece_signal;

/*semaforo*/

int setpos(int x, int y);

int semglobal;

int semplayer;

int semnum;
enum{
    STRAIGHT_TO,
    X_BEFORE_Y,
    Y_BEFORE_X,
    PROBABLE_LESS_COSTLY,
    EVASION_Y,
    EVASION_X,
    CAOS_POWER,
    STAND
};


#endif



