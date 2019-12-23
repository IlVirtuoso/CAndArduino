#ifndef DEBUG_H
#include "debug.h"
#endif


char errore[24];
void error(char message[], int err){
    sprintf(errore,"%s",strerror(err));
    fprintf(logger,"[ERROR]:%s,message:%s\n",errore,message);
    fprintf(stderr,"[ERROR]:%s,message:%s\n",errore,message);
    cleaner();
    exit(err);
    
}


va_list args;
char formatted[128];
int debug(const char *__restrict__ message, ...){
    if(isDebug){
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

float actime;

char logformatted[128];
void logg(const char  *__restrict__ message, ...){
    actime = (double)clock()/1000;
    va_start(args, message);
    vsnprintf(logformatted,sizeof(logformatted),message,args);
    fprintf(logger,"[LOG : %f] %s\n",(float)actime,logformatted);
    printf("[LOG: %f]%s\n",(float)actime,logformatted);
    va_end(args);
    bzero(logformatted,sizeof(logformatted));
}