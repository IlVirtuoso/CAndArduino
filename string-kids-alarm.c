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
void killall();

pid_t * childs;
int status;
int process;
int i;
int sum;
char * generated;
int gen;
char ** args;
struct sigaction sa;
pid_t pid;

static sigset_t mask;
int main(int argc, char * argv[]){
    args = (char**)malloc(sizeof(args));
    args[0] = "char-loop";
    args[1] = "a";
    
    bzero(&sa,sizeof(sa));
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    sa.sa_mask = mask;
    sa.sa_handler = handle_signal;
    sa.sa_flags = SA_RESTART;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGALRM,&sa,NULL);
    sigset(SIGALRM,handle_signal);
    process = atoi(argv[1]);
    childs = (pid_t*)malloc(sizeof(childs));
    gen = 0;
    generated = (char*)malloc(sizeof(generated));
    for(i = 0; i < process; i++){
        pid = fork();
        if(pid){
            
            childs[i] = pid;
            printf("Started Process : %d\n",childs[i]);

        }
        else{
            
            execve("./char-loop",args,NULL);
            printf("Cannot Start Program");
            exit(-1);
        }
    }
    
    while (1)
    {
    printf("Schedule alarm\n");
    alarm(1);
    wait(&status);
    sum = (sum + WEXITSTATUS(status)%2);
    printf("Result: %d\n",sum);
    generated[gen] = WEXITSTATUS(status);
    gen++;
    }
    
    return 0;
}

int choice;


void handle_signal(int signum){
    if(signum == SIGALRM){
        if(sum == 0){
            choice = rand() % process;
            printf("Killing: %d\n",childs[choice]);
            kill(childs[choice],SIGINT);
        }
        else{
            killall();
        }
    }
}

int q;
void killall(){
    for(q = 0; q < process; q++){
        kill(childs[q],SIGINT);
        wait(&status);
        generated[gen] = WEXITSTATUS(status);
        gen++;
    }
    printf("Generated: %s",generated);
    exit(0);
}
