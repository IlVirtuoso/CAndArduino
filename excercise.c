#include <stdio.h>
#include <stdlib.h>
#define SEPARATORI {0,10,13,',',';'}
#define CARATTERE 'K'

void print(char * String);
void ascii(char * String);
void intersect(char* q, char * p);
int length(char* String);
int count(char* string);
int isIn(char* string, char ch);
void tobin(int p);
int strcheck(const char * s, int n, char c);



char * buffer[1024];
int main(){
    fgets(buffer,1024,stdin);
    int x_init = 0;
    int x_fin = 0;
    int max_c = 0;
    int act_c = 0;
    char * longest;
    for(int i = 0; i < sizeof(buffer); i++){
        
    }
}



int strcheck(const char * s, int n, char c){
    int counter = 0;
    for(int i = 0; i < n; i++){
        if(*(s+i) == c){
            counter ++;
        }
    }
    return counter;
}


void print(char* String){
    for(int i = 0; i < length(String); i++){
        printf("%c", String[i]);
    }
}

void intersect(char * q, char * p){
    int i = 0;
    int p_controlled = 0;
    int q_controlled = 0;
    while(!q_controlled){
        if(!p_controlled){
            if(!(*(p+i) >= 32 && *(p+i) <= 126)){
                *(p + i) = '\0';
                p_controlled = 1;
            }
        }
        else{
            if(!(*(q+i) >= 32 && *(q+i) <= 126)){
                *(q + i) = '\0';
                q_controlled = 1;
            }
        }
        i++;
    }
    print(p);
    print(q);
    printf("controlled \n");
    int contained = 0;
    int j = 0;
    int k = 0;
    while(*(q+j) != '\0'){
            if(*(q+j) == *(p+k)){
                j++;
                k++;
                contained++;
            }
            else if(contained > 0){
                printf("%c",*(q+j));
                j++;
          }
          else{
              j++;
          }
    }
}
int length(char * string){
    int i = 0;
    while (string[i] != '\0')
    {
        i++;
    }
    return i;  
}


int count(char* string){
    char justcounted[99];
    for (int i = 0, j = 0; i < length(string); i++)
    {
        while (isIn(justcounted, string[i]))
        {
            i++;
        }
        char selected = string[i];
        justcounted[j] = selected;
        j++;
        int count = 0;
        for (int k = 0; k < length(string); k++)
        {
            if(selected == string[k]){
                count ++;
            }
        }
        printf("%c: %d \n", selected, count);
        
    }
    
}

int isIn(char* string, char ch){
    for (int i = 0; i < length(string); i++)
    {
        if(ch == string[i]){
            return 1;
        }
    }
    return 0;
    
}

void tobin(int p){
    int* bin = malloc(1000);
    int i = 0;
    while (p > 0)
    {
        int q = p >> 1;
        bin[i] = p - (q << 1);
        i++;
        p = q;
    }
    for (int k = i; k >= 0; k--)
    {
        printf("%d", bin[k]);
    }
    
}