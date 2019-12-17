#include "piece.h"
#include "macro.h"

/*array che identifica i pezzi appartenenti a questo giocatore*/
pid_t pieces[SO_NUM_P];


pid_t pid;

/*pid del player*/
pid_t player_pid;

/*player id*/
char player_id;

/*enorme funzione che inizializza il player*/
int player();
 
/*generatore di pezzi*/
int piecegen(int piecenum);

