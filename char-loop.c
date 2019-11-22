#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void sig_handler(int signum);

unsigned char c;
int main(int argc, char * argv[]){
    struct sigaction sa;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = sig_handler;
    c = argv[1];
    sigaction(SIGINT,&sa,NULL);
    while(1){
        if(c <= 126){
            c++;
        }
        else{
            c = 33;
        }
    }
}

void sig_handler(int signum){
    printf("handle signal #%d, char c = %c \n",signum,c);
    exit(c);
}