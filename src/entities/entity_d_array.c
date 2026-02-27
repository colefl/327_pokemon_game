/*
 * entity_d_array.c
 *
 *  Created on: Feb 24, 2026
 *      Author: colef
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "Entity.c"

#define DEFAULT_ENTITY_ARR_CAPACITY 12

typedef struct{
	char *a; //Idrk why we need this pointer to the data
	int elements; //I think this is the number of elements
	int capacity;
} entity_arr;

int eq_init(entity_arr *eq){//Don't need the element size, I know this will take in entities
	if(!(eq = malloc( sizeof (entity_arr)))){
		return -1;
	}

	eq->a = malloc(DEFAULT_ENTITY_ARR_CAPACITY * sizeof(entity));
	if ((eq)->a == NULL) {
	    free(*eq);
	    return -1;
	}

	(eq)->capacity = DEFAULT_ENTITY_ARR_CAPACITY; //initial capacity of array
	(eq)->elements = 0; //Num of elements

	return 0;
}

int eq_destroy(entity_arr *eq){
	return 0;
}

int eq_add(entity_arr *eq, int index, entity npc){
	if(index >= (eq)->elements || index < 0){
		return -1;
	}

	memcpy(((eq)->a) + ((sizeof(entity)) * (eq)->elements), npc, sizeof (entity));
	(eq)->elements++;

	return 0;
}

//Need to replace a lot of the npc and such with void pointers as memcpy and memmove are not applicable for any type so they must be the only type that's applicable to any type
int eq_remove(entity_arr *eq, int index, entity npc){
	if(index >= (eq)->elements || index < 0){
		return -1;
	}

	if(npc){ //I think this is kind of saying if this is real since in C 0 is false and everything else is true which is kind of really cool
		memcpy(npc, ((eq)->a) + ((eq)->elements * (sizeof(entity))), sizeof(entity));
	}

	memmove((eq)->a + (index * (sizeof(entity)),
			(eq)->a + ((index + 1)* (eq)->elements),
			sizeof(entity)));

	(eq)->elements--;

	return 0;
}



