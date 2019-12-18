#include "piece.h"
#include "macro.h"
#include "table.h"
#include "message.h"

/*array che identifica i pezzi appartenenti a questo giocatore*/
pid_t pieces[SO_NUM_P];


pid_t pid;

/*id della msgqueue*/
int player_msgqueue;

/*pid del player*/
pid_t player_pid;

/*player id*/
char player_id;

/*enorme funzione che inizializza il player*/
int player();
 
/*generatore di pezzi*/
int piecegen(int piecenum);

/*cleaner del player*/
void player_clean();

/*handler del player*/
void player_handler(int signum);

/*segmento di memoria condivisa della table player*/
cell * player_shared_table;

