/*
 * Entity.c
 *
 *  Created on: Feb 13, 2026
 *      Author: colef
 */

//Figuring the assignment
/*
 * Okay so effectively, I need to implement dijkstra's algorithm into map 0,0 whereas the mobs and such try to find the player
 * Thoughts: Okay so it appears to require that for dijkstra's algorithm, everything needs a weight.
 */

#include <limits.h>
#include <stdbool.h>

enum CharType{
	PLAYER,
	HIKER,
	RIVAL,
	SWIMMER
};

typedef struct {
	int x;
	int y;
	char type;
	int id;
	int weights[8];
	bool isSpawned;
} entity;

int copyArrs(int arr1[9], int arr2[9]);
int printArr(int arr[9]);

entity CreateEntity(int id, int x, int y){
	int player_weights[9] = {INT_MAX, INT_MAX, 10, 10, 20, 10, 10, INT_MAX, 10};
	int hiker_weights[9] = {INT_MAX, INT_MAX, 10, 50, 50, 15, 10, INT_MAX, INT_MAX};
	entity *tmp;
	if(!(tmp = malloc(sizeof (*tmp)))){ //MAKE SURE WE FREE THIS DATA TYPE
		return *tmp;
	}
	switch (id){
	case PLAYER:
		tmp->id = PLAYER;
		tmp->type='@';
		copyArrs(tmp->weights, player_weights);
		//printArr(tmp->weights);
		tmp->x = x;
		tmp->y = y;
		tmp->isSpawned = true;
		break;

	case HIKER:
		tmp->id = HIKER;
		tmp->type='h';
		copyArrs(tmp->weights, hiker_weights);
		printArr(tmp->weights);
		tmp->x = x;
		tmp->y = y;
		tmp->isSpawned = true;
	}

	return *tmp;
}

int copyArrs(int arr1[9], int arr2[9]){
	int i;
	for(i = 0; i < 8; i++){
		arr1[i] = arr2[i];
	}
	return 0;
}

int printArr(int arr[9]){
	int i;
	printf("Weights: ");
	for(i = 0; i < 8; i++){
		printf("%d, ", arr[i]);
	}
	printf("done \n");

	return 0;
}
