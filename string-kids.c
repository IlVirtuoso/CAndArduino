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

int main(int argc, char * argv[]){
    int status;
    char args[]={"a"};
    int processes = atoi(argv[1]);
    char buffer[processes];
    for(int i = 0; i < processes; i++){
        int n = fork();
        if(n == 0){
            if(execve("./char-loop",NULL,NULL) == -1){
                printf("error on launch\n");
                break;
            }
            
        }
        else{
            
            sleep(1);
            kill(n,SIGINT);
            printf("launch SIGKILL for process: %d\n",n);
            wait(&status);
            printf("exited with status: 0x%04X or %d\n",status,WEXITSTATUS(status));
            buffer[i] = WEXITSTATUS(status);
            
        }
    }
    printf("%s",buffer);
}