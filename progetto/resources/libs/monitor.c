#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif

struct timespec * crono;
int x;
int y;
void display(){

    for(x = 0 ; x < SO_BASE; x++){
        for(y = 0; y < SO_ALTEZZA; y++){
            printf("|%c|", tab(board,x,y)->id);
        }
        printf("\n");
    }
}

void show(){
    crono->tv_nsec = 10;
    while(1){
        display();
    }
}