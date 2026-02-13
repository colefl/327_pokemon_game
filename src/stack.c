/*
 * stack.c
 *
 *  Created on: Jan 27, 2026
 *      Author: colef
 */

struct stack_item {
	int value;
	int x;
	int y;
	struct stack_item *next;
};

struct stack {
	struct stack_item *top;
	int size;
};

#include "stdio.h"
#include "stdlib.h"
//#include <stack.h>


int stack_init(struct stack *s){
	s->top = NULL;
	s->size = 0;

	return 0;
};

int stack_destroy(struct stack *s){
	struct stack_item *tmp;

	while((tmp = s->top)){ //Why did we add two sets of parenthesis

		s->top = s->top->next;

		free(tmp);
	}

	s->top = NULL;
	s->size = 0;

	return 0;
}

int stack_push(struct stack *s, int val){
	struct stack_item *tmp;
	if(!(tmp = malloc(sizeof (*tmp)))){ //Even if we change the type of tmp or the data struct this would still work
		//malloc failed
		return -1;
	}

	tmp->value = val;
	tmp->next = s->top;
	s->top = tmp;
	s->size++;

	return 0;
}

int stack_pop(struct stack *s, int *val){

	struct stack_item *tmp;

	if(!s->top){
		return -1;
	}

	*val = s->top->value;
	s->top = s->top->next;

	free(tmp);

	s->size--;

	return 0;
}

int stack_top(struct stack *s, int *val){
	if(!s->top){
		return -1;
	}

	*val = s->top->value;

	return 0;

}
int stack_size(struct stack *s, int *size){

	*size = s->size;

	return 0;

}
