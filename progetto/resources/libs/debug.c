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

char formatted[128];
int debug(const char *__restrict__ message, ...){
    if(isDebug){
    va_list args;
    va_start(args, message);
    vsnprintf(formatted,sizeof(formatted),message,args);
    fprintf(logger,"[DEBUG]: %s\n",formatted);
    printf("[Debug]: %s\n", formatted);
    va_end(args);
    bzero(formatted,sizeof(formatted));
    return 1;

    }
    else{
        return 0;
    }
}

char logformatted[128];
void logg(const char  *__restrict__ message, ...){
    double time = (double)clock()/1000;
    va_list args;
    va_start(args, message);
    vsnprintf(logformatted,sizeof(logformatted),message,args);
    fprintf(logger,"[LOG : %f] %s\n",(double)time,logformatted);
    printf("[LOG: %f]%s\n",(double)time,logformatted);
    va_end(args);
    bzero(logformatted,sizeof(logformatted));
}