#ifndef TABLE_H
#include "table.h"
#endif

/*Region metodi per il controllo della scacchiera*/


cell * tab(cell * shared_table, int x, int y){

    return (&(*(shared_table + x*y + y)));
}

void table_start(){
    if((table = shmget(IPC_PRIVATE,sizeof(cell)*SO_BASE*SO_ALTEZZA,IPC_CREAT | 0666)) > 0){
        debug("Memoria Condivisa Inizializzata");
    }
    else{
        error("Errore nell'inizializzazione del segmento di memoria",EKEYREJECTED);
    }
}
/*End Of Life*/



