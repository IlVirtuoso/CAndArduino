#include <stdio.h>
#include <stdlib.h>


typedef struct node {
	int value;
	struct node * next;   /* pointer to the same structure */
} node;

typedef node * list;

typedef struct _Tree{
    int elem;
   struct Tree * left;
   struct Tree * right;
}Tree;


Tree * t;
int main(){
    t = (Tree*) malloc(sizeof(Tree));
    t->elem = NULL;
}

void insert(int x){
    if(t->elem == NULL){
        t->elem = x;
    }
}
