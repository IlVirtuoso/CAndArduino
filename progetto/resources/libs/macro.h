#ifndef MACRO_H
#define MACRO_H

#ifndef SO_NUM_G
/*Numero di giocatori*/
#define SO_NUM_G 2
#endif

#ifndef SO_NUM_P
/*Numero di pedine*/
#define SO_NUM_P 10
#endif

#ifndef SO_MAX_TIME
/*Durata massima di un round*/
#define SO_MAX_TIME 3
#endif


#ifndef SO_BASE
/*Dimensione del parametro x della matrice*/
#define SO_BASE 60
#endif


#ifndef SO_ALTEZZA
/*Dimensione del parametro y della matrice*/
#define SO_ALTEZZA 20
#endif

#ifndef SO_FLAG_MIN
/*Numero minimo di bandierine posizionate in un round*/
#define SO_FLAG_MIN 5
#endif

#ifndef SO_FLAG_MAX
/*Numero massimo di bandierine posizionate in un round*/
#define SO_FLAG_MAX 5
#endif

#ifndef SO_ROUND_SCORE
/*Punteggio totale assegnato per round, da ripartire sulle bandierine*/
#define SO_ROUND_SCORE 10
#endif

#ifndef SO_N_MOVES
/*numero totale di mosse a disposizone delle pedine*/
#define SO_N_MOVES 20
#endif

#ifndef SO_MIN_HOLD_NSEC
/*numero minimo di nanosecondi di occupazione di una cella da parte di una pedina*/
#define SO_MIN_HOLD_NSEC 100000000
#endif

#ifndef SIGSTART
#define SIGSTART SIGUSR1
#endif

#ifndef FLAG
#define FLAG '!'
#endif

#ifndef EMPTY
#define EMPTY ' '
#endif

#ifndef MASTER_SEM
#define MASTER_SEM 0
#endif

#ifndef PLAYER_SEM
#define PLAYER_SEM 1
#endif


#ifndef PIECE_SEM
#define PIECE_SEM 2
#endif

/*DEV_IEM ho spostato i commenti per farli leggere a intellisense
*Piccolo Tutorial su intellisense per C/C++:
*Definizione commenti per FUNZIONI (vanno dichiarate inizialmente):
*  
* [COMMENTO]
* int funzione(); 
*
*Definizione commenti per costanti #DEFINE (vanno messi subito prima del #define):
*
*#ifndef var
*[COMMENTO]
*#define var
*#endif
*fine tutorial*/

#endif