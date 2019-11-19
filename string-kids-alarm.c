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

pid_t * childs;
int status;
int process;
int sum;
char * generated;
int gen;
char * args[]= {{"char-loop"},{"a"}};

static sigset_t mask;
int main(int argc, char * argv[]){
    struct sigaction sa;
    bzero(&sa,sizeof(sa));
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    
    sa.sa_mask = mask;
    sa.sa_handler = handle_signal;
    sigaction(SIGALRM,&sa,NULL);
    sigset(SIGALRM,handle_signal);
    process = atoi(argv[1]);
    childs = (pid_t*)malloc(sizeof(childs));
    gen = 0;
    generated = (char*)malloc(sizeof(generated));
    pid_t pid;
    for(int i = 0; i < process; i++){
        if(pid = fork()){
            //parent
            childs[i] = pid;
            printf("Started Process : %d\n",childs[i]);

        }
        else{
            //child
            execve("./char-loop",args,NULL);
            printf("Cannot Start Program");
            exit(-1);
        }
    }
    
    alarm(1);
    sum = 0;
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
    generated[gen] = WEXITSTATUS(status);
    gen++;
    printf("killing child: %d\n", childs[choiced]);
    printf("exited with status: %d\n",WEXITSTATUS(status));
    sum = sum + (int)WEXITSTATUS(status);
    sum = sum % 256;
    printf("Result: %d\n",sum);
    if(sum == 0){
        for(int i = 0; i < process; i++){
            if(i == choiced){
                //skip
            }
            else{
                kill(childs[i],SIGINT);
                wait(&status);
               generated[gen] = WEXITSTATUS(status);
            }
        }
    }
    else{
        pid_t n = fork();
        if(n){
            //parent
            childs[choiced] = n;
            printf("replaced old process with: %d\n",childs[choiced]);
            alarm(1);
        }
        else{
            //child
            execve("./char-loop",args,NULL);
            printf("error cannot start program\n");
            exit(-1);
        }
        
        alarm(1);
        wait(&status);
    }
    printf("%s",generated);
}