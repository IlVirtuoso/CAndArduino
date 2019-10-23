#ifndef _LIST_MODULE_H
#define _LIST_MODULE_H
/*this module offers duplication, conditional delete, delete if odd
*list removing below functions
*/
#endif

typedef struct node {
	int value;
	struct node * next;   /* pointer to the same structure */
} node;

typedef node * list;


list list_delete_if(list head, int to_delete);
list list_delete_odd(list head);
list list_cut_below(list head, int cut_value);
list list_dup(list head);