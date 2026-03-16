/*
 * world_gen.h
 *
 *  Created on: Mar 15, 2026
 *      Author: colef
 */

#ifndef WORLD_GEN_H_
#define WORLD_GEN_H_

//#include "entities/entity_move_q.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "heap.h"
//#include "stack.c"
#include "point_queue.h"
#include "entities/Entity.h"
//#include "entities/entity_d_array.c"
#include "entities/entity_move_q.h"

enum Entrances {
	North,
	West,
	South,
	East,
	OutOfBounds = 99
};


typedef struct Map{
	char arr[80][21];
	bool visited[80][21];
	int entrances[4];
	bool omitDir[4];
} Map;

#include "pathMaker.h"

struct point{
	int x;
	int y;
	//bool hasGone;
	//bool growing;
	//struct point *next;
} g1, g2, tg1, tg2, w; //Got from Brian W Kernighan and Dennis M. Ritchie's book

typedef struct {
    int x;
    int y;
    int cost;
} map_cell_t;

//Variables
#define WORLDX 80
#define WORLDY 21

//Methods

int init_map(struct Map *m);

//WORLD EDGE
int init_world_edge(Map *current_map);

//WORLD SPREAD
int get_num(int count, Map *m);
void DFS(struct point_queue *pq, Map *m);
bool canGrow(struct queue_item p, Map *m);
int pepperInTrees(Map *m);

//SPAWNING ENTITIES
int spawnEntities(heap_t *eq, int id, Map *m);
int spawnEntity(entity *npc, int id, Map *m);
int32_t cell_compare(const void *key, const void *with);
int dijkstrasAlgo(Map *m, entity *player, entity *npc, int dist[80][21]);
int check_if_spawns_on(char tile, char spawnables[4]);

//GAMELOOP
void runGameLoop(heap_t *eq, Map *m);
int handle_npc_movement(entity *npc, int dist[80][21] , Map *m);
int handle_wanderer_movement(entity *npc, Map *m);
int handle_pacer_movement(entity *npc, Map *m);
int handle_explorer_movement(entity *npc, Map *m);

//Helper
int print_board(Map *m);
int print_costs(int arr[80][21], entity *player);


#endif /* WORLD_GEN_H_ */
