#ifndef DEBUG_H
#include "debug.h"
#endif


char errore[24];
void error(char message[], int err){
    sprintf(errore,"%s",strerror(err));
    printf("[ERROR]:%s,message:%s\n",errore,message);
    fprintf(logger,"[ERROR]:%s,message:%s\n",errore,message);
    cleaner();
    exit(err);
    
}

int debug(const char *__restrict__ message, ...){
    if(isDebug){
        printf("[Debug]: %s\n", message);
        fprintf(logger,"[DEBUG]: %s\n",message);
        return 1;
    }
    else{
        return 0;
    }
}

void logg(const char  *__restrict__ message, ...){
    double time = (double)clock()/1000;
    printf("[LOG: %f]%s\n",(double)time,message);
    fprintf(logger,"[LOG : %f] %s\n",(double)time,message);

}