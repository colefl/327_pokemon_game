/*
 * entity_move_q.h
 *
 *  Created on: Mar 15, 2026
 *      Author: colef
 */

#ifndef ENTITY_MOVE_Q_H_
#define ENTITY_MOVE_Q_H_


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "../heap.h"

#include "Entity.h"


typedef struct entity_move {
	entity *npc;
	int next_move;
} entity_move;

void init_game_queue(heap_t *pq);
void enqueue_entity(heap_t *pq, entity *ent, int time);
entity_move *dequeue_next(heap_t *pq);
void destroy_game_queue(heap_t *pq);

#endif /* ENTITY_MOVE_Q_H_ */
