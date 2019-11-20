#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

FILE * csv;
char *** matrix;
countEntries(FILE * buffer);

int main(int argc, char * argv[]){
    matrix = (char***)malloc(sizeof(matrix));
    csv = fopen("./PPAY_ListaMovimenti.CSV", "r");
    int r = 0; //Row
    int c = 0; //Column
    int b = 0; //Buffer
    printf("Entries are: %d",countEntries(csv));
    char buf;
    fseek(csv,0,0);
    while ((buf = fgetc(csv)) != EOF)
    {
        switch (buf)
        {
        case ';':
        buf = ' ';
        c++;
        b = 0;
        break;

        case ' ':

        break;

        case '\n':
        buf = ' ';
        r++;
        c = 0;
        
        break;
        
        default:
        printf("Writing on: Row: %d, Column: %d, Buffer: %d, Content: %c \n",r,c,b,buf);
        matrix[r][c][b] = buf;
        b++;
            break;
        }
    }
    print(matrix,r,c);
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
    return lines;
    
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

int countEntries(FILE * buffer){
    int entries = 0;
    fseek(buffer,0,0);
    char c;
    while((c = fgetc(buffer)) != '\n'){
     if(c == ';'){
         entries++;
         c = ' ';
     } 
    }
    return entries;
}

void parse(FILE * buffer){
    fseek(buffer,0,0);
    char c;
    while((c = fgetc(buffer)) != EOF){
        switch (c)
        {

        case '\n':
        c = ' ';
            printf("  [return] \n");
            break;

        case ';':
        printf("  [Entry];");
        c = ' ';
        break;
        
        default:
        printf("%c",c);
        c = ' ';
            break;
        }
        
    }
}

void print(char * matrix[],int rows, int columns){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            printf("|%s|",matrix[i][j]);
        }
    }
}

