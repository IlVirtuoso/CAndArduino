#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif


int x;
int y;
void display(){
    for(x = 0 ; x < SO_ALTEZZA; x++){
        for(y = 0; y < SO_BASE; y++){
            printf("|%c|", getid(board,x,y));
        }
        printf("\n");
    }
}

void show(){
    while(1){
        sleep(1);
        display();
    }
}

void display_sem(){
    int x,y;
    for(x = 0; x < SO_ALTEZZA; x++){
        for (y = 0; y < SO_BASE; y++){
            printf("|%d|", semctl(sem_table,x*y + y, GETVAL));
        }
        printf("\n");
    }
}
