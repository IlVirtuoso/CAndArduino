#include <stdio.h>
#include <stdlib.h>


FILE * csv;





int main(int argc, char * argv[]){
    csv = fopen("./PPAY_ListaMovimenti.CSV", "r");
    fseek(csv,0,0);
    char *** matrix = (char***)malloc(sizeof(entrySize(csv) * sizeof(char***)));
    int r = 0; //Row
    int c = 0; //Column
    int b = 0; //Buffer
    matrix[r] = (char*)malloc(sizeof(char*));
    matrix[r][c] = (char**)malloc(sizeof(char**));
    while((matrix[r][c][b] = getc(csv)) != EOF){
        
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