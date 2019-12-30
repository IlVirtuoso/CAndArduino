#define _GNU_SOURCE
#ifndef SIGNAL_H
#include <signal.h>
#endif
#ifndef STDIO_H
#include <stdio.h>
#endif
#ifndef SHM_H
#include <sys/shm.h>
#endif
#ifndef STDLIB_H
#include <stdlib.h>
#endif
#ifndef UNISTD_H
#include <unistd.h>
#endif
#ifndef TYPES_H
#include <sys/types.h>
#endif
#ifndef SYSINFO_H
#include <sys/sysinfo.h>
#endif
#ifndef ERRNO_H
#include <sys/errno.h>
#endif
#ifndef IPC_H
#include <sys/ipc.h>
#endif
#ifndef MSG_H
#include <sys/msg.h>
#endif
#ifndef WAIT_H
#include <sys/wait.h>
#endif
#ifndef SEM_H
#include <sys/sem.h>
#endif
#ifndef TIME_H
#include <time.h>
#endif
#ifndef STRING_H
#include <string.h>
#endif
#ifndef MACRO_H
#include "macro.h"
#endif
#ifndef PIECE_H
#include "piece.h"
#endif
#ifndef MONITOR_H
#include "monitor.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef MESSAGE_H
#include "message.h"
#endif
#ifndef TABLE_H
#include "table.h"
#endif
#ifndef SIGNAL_H
#include <signal.h>
#endif

#ifndef PLAYER_H
#define PLAYER_H

/* Lunghezza dei messaggi per controllo pedine*/
#define CONTROL_SIZE 10

/*array che identifica i pezzi appartenenti a questo giocatore*/
pid_t pieces[SO_NUM_P];

/*struttura dei segnali del player*/
struct sigaction player_signal;

/*mask per i segnali del player*/
sigset_t player_mask;

pid_t pid;

/*player 2 piece msgqueue*/
int player_msgqueue;

/*pid del player*/
pid_t player_pid;

/*player id*/
char player_id;


struct timespec * clockd;

/*enorme funzione che inizializza il player*/
int player();
 
/*generatore di pezzi*/
int piecegen(int piecenum);

/*cleaner del player*/
void player_clean();

/*handler del player*/
void player_handler(int signum);

/*ciclo del player*/
void play();

/*segmento di memoria condivisa della table player*/
cell * player_shared_table;



int semid;


char player_logbuffer[128];

#endif