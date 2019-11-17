#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <signal.h>

void handle_signal(int signum);
void kill_one();

int * childs;
int status;
int process;
int sum;
char args[]= {"char-loop a"};

static sigset_t mask;
int main(int argc, char * argv[]){
    struct sigaction sa;
    bzero(&sa,sizeof(sa));

    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    
    sa.sa_mask = mask;
    sa.sa_handler = handle_signal;

    process = atoi(argv[1]);
    childs = (int*)malloc(sizeof(int)*process);
    for(int i = 0; i < process; i++){
        int n = fork();
        if(n){
            //parent
            childs[i] = n;

        }
        else{
            //child
            execve("./char-loop",args,NULL);
            printf("Cannot Start Program");
            exit(-1);
        }
    }

    alarm(1);
    wait(&status);
}


void handle_signal(int signum){
    if(signum == SIGALRM){
        kill_one();
    }
}

void kill_one(){
    int choiced = rand() % process;
    kill(childs[choiced],SIGINT);
    printf("%c",status);
    sum = sum + status;
    if(sum == 0){
        for(int i = 0; i < process; i++){
            if(i == choiced){

            }
            else{
                kill(childs[i],SIGINT);
                wait(&status);
                printf("%c",status);
            }
        }
    }
    else{
        int n = fork();
        if(n){
            //parent
            childs[choiced] = n;
            alarm(1);
            
        }
        else{
            //child
            execve("./char-loop",args,NULL);
            printf("error cannot start program");
            exit(-1);
        }
    }
}