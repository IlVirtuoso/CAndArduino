#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif


int x;
int y;
void display(cell * shared_table){
    for(x = 0 ; x < SO_ALTEZZA; x++){
        for(y = 0; y < SO_BASE; y++){
            printf("|%c| ", getid(shared_table,x,y));
        }
        printf("\n");
    }
}


void display_sem(){
    int x,y;
    for(x = 0; x < SO_ALTEZZA; x++){
        for (y = 0; y < SO_BASE; y++){
            printf("|%d| ", semctl(sem_table,x*SO_BASE + y, GETVAL));
        }
        printf("\n");
    }
}
