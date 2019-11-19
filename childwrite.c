#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


char * c;
int * p;
int main(){
    c = (char*)malloc(sizeof(c));
    printf("%s",c);
    p = (int*)malloc(sizeof(p));
    for(int i = 0; i < 100; i++){
        p[i] = i;
        printf("%d",p[i]);
    }
}