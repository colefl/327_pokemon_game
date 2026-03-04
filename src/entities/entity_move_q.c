/*
 * entity_move_q.c
 *
 *  Created on: Mar 3, 2026
 *      Author: colef
 */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "../heap.h"

#include "Entity.c"


typedef struct entity_move {
	entity *npc;
	int next_move;
} entity_move;

static int32_t compare_events(const void *a, const void *b)
{
    const entity_move *ea = (const entity_move *) a;
    const entity_move *eb = (const entity_move *) b;

    if (ea->next_move < eb->next_move) return -1;
    if (ea->next_move > eb->next_move) return  1;
    return 0;
}

void init_game_queue(heap_t *pq)
{
    heap_init(pq, compare_events, free);
}

void enqueue_entity(heap_t *pq, entity *ent, int time)
{
    entity_move *ev = malloc(sizeof(entity_move));
    if (!ev) {
    	printf("This isn't working in the enqueue");
    	return;
    }

    ev->npc = ent;
    ev->next_move = time;

    heap_insert(pq, ev);
}

entity_move *dequeue_next(heap_t *pq)
{
    return (entity_move *) heap_remove_min(pq);
}

void destroy_game_queue(heap_t *pq)
{
    heap_delete(pq);
}



