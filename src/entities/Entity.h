/*
 * Entity.h
 *
 *  Created on: Mar 15, 2026
 *      Author: colef
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
	char prev_tile;
	int direction;
	bool isDefeated;
} entity;

int copyArrs(int arr1[NUM_OF_TILES], int arr2[NUM_OF_TILES]);
int printArr(int arr[NUM_OF_TILES]);

entity* CreateEntity(int id, int x, int y);

int deleteEntity(entity *npc);



#endif /* ENTITY_H_ */
