#include "table.h"


/*Region metodi per il controllo della scacchiera*/


cell * tab(cell * shared_table, int x, int y){

    return (&(*(shared_table + x*y + y)));
}

/*End Of Life*/



