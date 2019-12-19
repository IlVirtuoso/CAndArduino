#ifndef MESSAGE_H
#include "message.h"
#endif

int message_start(key_t key){
    if((msgqueue = msgget(key,IPC_CREAT | 0600)) == -1){
        error("Errore nell'inizializzazione della msgqueue", EKEYREJECTED);
    }
    else {
        debug("Msgqueque creata");
    }
    return msgqueue;
}
