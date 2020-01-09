#ifndef PARAMETERS_H
#include "parameters.h"
#endif

typedef struct{
    int token;
    char id[30];
}configuration;

configuration * cfg;

void ParseFile(FILE * config){
char c;
int b;
int interr = 0;
SO_ALTEZZA = -1;
SO_BASE = -1;
SO_FLAG_MAX = -1;
SO_FLAG_MIN = -1;
SO_MAX_TIME = -1;
SO_MIN_HOLD_NSEC = -1;
SO_N_MOVES = -1;
SO_NUM_G = -1;
SO_NUM_P = -1;
SO_ROUND_SCORE = -1;
fseek(config,0,0);
cfg = (configuration *)malloc(sizeof(cfg));
while(interr != 1){
    while((c = fgetc(config)) != '\n'){
        
    }
}

}

