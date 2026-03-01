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

int eq_init(entity_arr **eq){
    if(!(*eq = malloc(sizeof(entity_arr)))){
        return -1;
    }

    (*eq)->a = malloc(DEFAULT_ENTITY_ARR_CAPACITY * sizeof(entity));
    if((*eq)->a == NULL){
        free(*eq);
        return -1;
    }

    (*eq)->capacity = DEFAULT_ENTITY_ARR_CAPACITY;
    (*eq)->elements = 0;

    return 0;
}

int eq_destroy(entity_arr *eq){
	return 0;
}

static int grow(entity_arr *eq){ //Means the function can only be called inside of this file
	void *v; //We need to have a temp value in order to account for the fact that if alloc fails, it will set to null
	if(!(v = realloc((eq) -> a, (eq) -> capacity * 2 * (sizeof (entity))))){
		return -1;
	}

	(eq) -> a = v; //If it doesn't fail then grow

	return 0;
}

int eq_add(entity_arr *eq, int index, entity* npc){
	if ((eq)->capacity == (eq)->elements && grow(eq)){ //If our capacity equals our number of elements we need to resize
		printf("This is failing at eq_add\n");
		return -1;
	}

	memcpy(((eq)->a) + ((sizeof(entity)) * (eq)->elements), npc, sizeof (entity));
	(eq)->elements++;

	printf("Here is an entity: %c\n", (*npc).marker);
	printf("Here is the number of elements: %d\n", eq->elements);

	return 0;
}


//Need to replace a lot of the npc and such with void pointers as memcpy and memmove are not applicable for any type so they must be the only type that's applicable to any type
int eq_remove(entity_arr *eq, int index, void* npc){
	if(index >= (eq)->elements || index < 0){
		return -1;
	}

	if(npc){ //I think this is kind of saying if this is real since in C 0 is false and everything else is true which is kind of really cool
		memcpy(npc, ((eq)->a) + ((eq)->elements * (sizeof(entity))), sizeof(entity));
	}

	memmove((eq)->a + (index * (sizeof(entity))),
			(eq)->a + ((index + 1)* (eq)->elements),
			sizeof(entity));

	(eq)->elements--;

	return 0;
}


int eq_visit_all(entity_arr *eq, void (*visitor)(void *)){
	int i;
	printf("hello I am running\n");
	for(i = 0; i < ((eq)->elements); i++){
		visitor((eq)->a + (i * (sizeof (entity))));
	}

	return 0;
}

void print_entity(void *e){
    entity *npc = (entity *)e;
    printf("Marker: %c, X: %d, Y: %d\n", npc->marker, npc->x, npc->y);
}



