/*
 * Entity.cpp
 *
 *  Created on: Feb 13, 2026
 *      Author: colef
 */

#include "Entity.h"

#include <new>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

entity* CreateEntity(int id, int x, int y){

    // Weights go: Bldr, Tree, Path, Pmart, Pcenter, TGras, SGras, Water, Gate, Other NPCS
    int player_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 10, 20, 10, 10, INT_MAX, 10, INT_MAX};
    int hiker_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 50, 50, 15, 10, INT_MAX, INT_MAX, INT_MAX};
    int rival_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX};
    int default_weights[NUM_OF_TILES] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 10, 10, INT_MAX, INT_MAX, INT_MAX};


    entity *tmp = new(std::nothrow) entity; //Double check
    if(!tmp){
        return NULL;
    }

    switch (id){
    case PLAYER:
        tmp->id = PLAYER;
        tmp->marker = '@';
        tmp->spawnsOn[0] = '#';
        copyArrs(tmp->weights, player_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case HIKER:
        tmp->id = HIKER;
        tmp->marker = 'h';
        tmp->spawnsOn[0] = ':';
        copyArrs(tmp->weights, hiker_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case RIVAL:
        tmp->id = RIVAL;
        tmp->marker = 'r';
        tmp->spawnsOn[0] = '.';
        copyArrs(tmp->weights, rival_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case PACER:
        tmp->id = PACER;
        tmp->marker = 'p';
        tmp->spawnsOn[0] = '.';
        tmp->spawnsOn[1] = ':';
        copyArrs(tmp->weights, default_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case WANDERER:
        tmp->id = WANDERER;
        tmp->marker = 'w';
        tmp->spawnsOn[0] = '.';
        tmp->spawnsOn[1] = ':';
        copyArrs(tmp->weights, default_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case SENTRY:
        tmp->id = SENTRY;
        tmp->marker = 's';
        tmp->spawnsOn[0] = '.';
        tmp->spawnsOn[1] = ':';
        copyArrs(tmp->weights, default_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;

    case EXPLORERS:
        tmp->id = EXPLORERS;
        tmp->marker = 'e';
        tmp->spawnsOn[0] = '.';
        tmp->spawnsOn[1] = ':';
        copyArrs(tmp->weights, default_weights);
        tmp->x = x;
        tmp->y = y;
        tmp->isSpawned = false;
        tmp->isDefeated = false;
        break;
    }

    return tmp;
}

int deleteEntity(entity *npc){
    delete npc;
    return 0;
}

int copyArrs(int arr1[NUM_OF_TILES], int arr2[NUM_OF_TILES]){
    int i;
    for(i = 0; i < NUM_OF_TILES; i++){
        arr1[i] = arr2[i];
    }
    return 0;
}

int printArr(int arr[NUM_OF_TILES]){
    int i;
    printf("Weights: ");
    for(i = 0; i < NUM_OF_TILES; i++){
        printf("%d, ", arr[i]);
    }
    printf("done \n");

    return 0;
}
