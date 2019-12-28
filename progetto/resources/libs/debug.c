#ifndef DEBUG_H
#include "debug.h"
#endif


char errore[24];
void error(char message[], int err){
    sprintf(errore,"[ERROR]:%s,message:%s\n",strerror(err),message);
    fprintf(logger,"%s",errore);
    cleaner();
    perror(errore);
    exit(err);
    
}



int debug(const char *__restrict__ message, ...){
    char formatted[128];
    va_list args;
    if(isDebug){
    va_start(args, message);
    vsnprintf(formatted,sizeof(formatted),message,args);
    fprintf(logger,"[DEBUG : %s]: %s\n",processSign,formatted);
    printf("[Debug %s]: %s\n",processSign, formatted);
    va_end(args);
    bzero(formatted,sizeof(formatted));
    return 1;

    }
    else{
        return 0;
    }
}

float actime;


void logg(const char  *__restrict__ message, ...){
    char logformatted[128];
    va_list args;
    actime = (double)clock()/1000;
    va_start(args, message);
    vsnprintf(logformatted,sizeof(logformatted),message,args);
    fprintf(logger,"[LOG : %s -> %f] %s\n",processSign,(float)actime,logformatted);
    printf("[LOG : %s -> %f]%s\n",processSign,(float)actime,logformatted);
    va_end(args);
    bzero(logformatted,sizeof(logformatted));
}