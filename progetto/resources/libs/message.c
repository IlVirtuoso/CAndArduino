#ifndef MESSAGE_H
#include "message.h"
#endif

void message_start(){
    if((msgqueue = msgget(IPC_PRIVATE,IPC_CREAT | 0600)) == -1){
        error("Errore nell'inizializzazione della msgqueue", EKEYREJECTED);
    }
    else {
        debug("Msgqueque creata");
    }
}