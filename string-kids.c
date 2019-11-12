#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char * argv[]){
    int processes = atoi(argv[1]);
    char buffer[processes];
    for(int i = 0; i < processes; i++){
        int n = fork();
        if(n == 0){
            execve("./char-loop",'a',"/bin/sh");
        }
        else{
            kill(n,SIGINT);
            buffer[i] = wait(&n);
        }
    }
    printf("%s",buffer);
}