#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * csv;

int main(int argc, char * argv[]){
    csv = fopen("./PPAY_ListaMovimenti.CSV", "r");
    fseek(csv,0,0);
    char *** matrix = (char***) malloc(sizeof(char**)*sizeof(char*)*sizeof(char***));
    char c;
    int i = 0;
    int j = 0;
    int buf = 0;
    char buffer[40];
    while((c = fgetc(csv)) != EOF){
        if(c == ';'){
            for(int b = 0; b < buf; b++){
                matrix[i][j][b] = buffer[b];
                printf("%c",matrix[i][j][b]);
            }
            buf = 0;
            j++;
        }
        else if(c == '\n'){
            i++;
            j = 0;
            
        }
        else{
            buffer[buf] = c;
            buf++;
            
        }
    }
    for(int i = 0; i < sizeof(matrix);i++){
        for(int j = 0; j < sizeof(matrix[i]);j++){
            for(int b = 0; b < sizeof(matrix[i][j]); b++){
                printf("%c",matrix[i][j][b]);
            }
        }
    }
    
}

int countLines(FILE * buffer){
    char buf;
    fseek(buffer,0,0);
    int lines = 0;
    while ((buf = fgetc(buffer)) != EOF)
    {
        if(buf == '\n'){
            lines++;
        }
    }
    
}

int entrySize(FILE * buffer){
    int size = 0;
    fseek(buffer,0,0);
    char c;
    while((c = fgetc(buffer)) != '\n'){
        if(c == ';'){
            size++;
        }
    }
    return size;
}