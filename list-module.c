#include <stdlib.h>
#include "list-module.h"




list list_delete_if(list head, int to_delete){
	if(head == NULL){
		return head;
	}
	int x = to_delete;
	if(head->value == x){
		head = head->next;
	}
	list q = head;
	while(q->next->next != NULL){
		if(q->next->value == x){
			q->next = q->next->next;
		}
	}
	if(q->next->value == x){
		q->next = NULL;
	}
	return head;
}

list list_delete_odd(list head){
	list q = head;
	while(q != NULL){
		if(q->value % 2 != 0){
			list_delete_if(head, q->value);
			q = q->next;
		}
		else{
			q = q->next;
		}
	}
	return head;
}

list cut_below(list head, int cut_value){
	for(int i = 0; i<= cut_value; i++){
		list_delete_if(head, i);
	}
}

list list_dup(list head){
	if(head == NULL){
		return head;
	}
	else{
		list newnode = (list *) malloc (sizeof(newnode));
		newnode->value = head->value;
		newnode->next = list_dup(head->next);
		return newnode;
	}
}