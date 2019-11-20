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
    
    while(1){
        alarm(1);
        wait(&status);
    }
}


void handle_signal(int signum){
    if(signum == SIGALRM){
        kill_one();
        printf("Schedule alarm1\n");
    }
}

int q;
int k;
void kill_one(){

    int choiced = rand() % process;
    kill(childs[choiced],SIGINT);
    wait(&status);
    generated[gen] = WEXITSTATUS(status);
    gen++;
    printf("killing child: %d\n", childs[choiced]);
    printf("exited with status: %d\n",WEXITSTATUS(status));
    sum = (sum + (int)WEXITSTATUS(status))%8;
    printf("Result: %d\n",sum);
    if(sum == 0){
        for(k = 0; k < process; k++){
                kill(childs[k],SIGINT);
                wait(&status);
               generated[gen] = WEXITSTATUS(status);
               gen++;
        }
            printf("%s",generated);
            exit(0);
    }
    else{
        pid_t n = fork();
        if(n){
            printf("Actual process: ");
            for(q = 0; q < process; q++){
                printf("|%d|",childs[q]);
            }
            printf("\n");
            childs[choiced] = n;
            printf("replaced old process with: %d\n",childs[choiced]);
        }
        else{
            
            execve("./char-loop",args,NULL);
            printf("error cannot start program\n");
            exit(-1); 
        }
    }
}