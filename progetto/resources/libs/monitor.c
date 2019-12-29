#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif


int x;
int y;
void display(){
    for(x = 0 ; x < SO_BASE; x++){
        for(y = 0; y < SO_ALTEZZA; y++){
            printf("|%c|", getid(board,x,y));
        }
        printf("\n");
    }
}

void show(){
    while(1){
        display();
    }
}