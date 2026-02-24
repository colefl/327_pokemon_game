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
	PACER,
	WANDERER,
	SENTRY,
	EXPLORERS
};

enum entity_readbility{
	NUM_OF_TILES = 10
};

typedef struct {
	int x;
	int y;
	char marker;
	int id;
	char spawnsOn[4];
	int weights[8];
	bool isSpawned;
} entity;

int copyArrs(int arr1[NUM_OF_TILES], int arr2[NUM_OF_TILES]);
int printArr(int arr[NUM_OF_TILES]);

entity CreateEntity(int id, int x, int y){

	//Weights go Bldr, Tree, Path, Pmart, Pcenter, TGras, SGras, Water, Gate, Other NPCS
	int player_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 10, 20, 10, 10, INT_MAX, 10, INT_MAX};
	int hiker_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 50, 50, 15, 10, INT_MAX, INT_MAX, INT_MAX};
	int rival_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX};
	entity *tmp;
	if(!(tmp = malloc(sizeof (*tmp)))){ //MAKE SURE WE FREE THIS DATA TYPE
		return *tmp;
	}
	switch (id){
	case PLAYER:
		tmp->id = PLAYER;
		tmp->marker='@';
		tmp->spawnsOn[0]='#';
		copyArrs(tmp->weights, player_weights);
		//printArr(tmp->weights);
		tmp->x = x;
		tmp->y = y;
		tmp->isSpawned = false;
		break;

	case HIKER:
		tmp->id = HIKER;
		tmp->marker='h';
		tmp->spawnsOn[0]=':';
		copyArrs(tmp->weights, hiker_weights);
		//printArr(tmp->weights);
		tmp->x = x;
		tmp->y = y;
		tmp->isSpawned = false;
		break;

	case RIVAL:
		tmp->id = RIVAL;
		tmp->marker='r';
		tmp->spawnsOn[0]='.';
		copyArrs(tmp->weights, rival_weights);
		tmp->x= x;
		tmp->y = y;
		tmp->isSpawned = false;
		break;
	case PACER:
		tmp->id = PACER;
		tmp->marker='p';
		tmp->spawnsOn[0] = '.';
		tmp->spawnsOn[1]=':';
		//We don't need this to be the case
		tmp->x = x;
		tmp->y = y;
		tmp->isSpawned = false;
		break;
	}

	entity result = *tmp;
	free(tmp);
	return result;
}

int deleteEntity(entity *npc){
	free(npc); //?
	return 0;
}

int copyArrs(int arr1[NUM_OF_TILES], int arr2[NUM_OF_TILES]){
	int i;
	for(i = 0; i < 8; i++){
		arr1[i] = arr2[i];
	}
	return 0;
}

int printArr(int arr[NUM_OF_TILES]){
	int i;
	printf("Weights: ");
	for(i = 0; i < 8; i++){
		printf("%d, ", arr[i]);
	}
	printf("done \n");

	return 0;
}



