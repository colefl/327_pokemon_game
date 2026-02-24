/*
 * entity_d_array.c
 *
 *  Created on: Feb 24, 2026
 *      Author: colef
 */


#include <stdlib.h>
#include <stdio.h>
#include "Entity.c"

typedef struct{
	char *a;
	entity npc;
	int capacity;
} entity_arr;

int eq_init(entity_arr *eq, int element_size){
	if(!(*eq = malloc( sizeof (struct entity_arr)))){
		return -1;
	}


	return 0;
}

/*
struct queue_item{
	int x; //Will hold the point's x
	int y; //Will hold the point's y
	char value;
	struct queue_item *next;
};

struct point_queue{
	struct queue_item *front;
	struct queue_item *rear;
	int size;
};

void initialize_pq(struct point_queue *pq){
	pq->front = NULL;
	pq->rear = NULL;
	pq->size = 0;
}

int queue_size(struct point_queue *pq, int *size){
	*size = pq->size;
	return 0;
}

int enqueue(struct point_queue* q, int x, int y, char val) {
	struct queue_item *tmp;
	if(!(tmp = malloc(sizeof (*tmp)))){ //Even if we change the type of tmp or the data struct this would still work
		//malloc failed
		return -1;
	}
    tmp->x = x;
    tmp->y = y;
    tmp->value=val;
    tmp->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = tmp;
    } else {
        q->rear->next = tmp;
        q->rear = tmp;
    }
    q->size++;

    return 0;
}

int dequeue(struct point_queue* q, int *x, int *y, char *val) { //Need to change this to return a queue_item instead
    if (q->front == NULL) {
        //printf("Queue is empty\n");
        return -1;
    }
    struct queue_item* temp = q->front;
    *x = temp->x;
    *y = temp-> y;
    *val = temp->value;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->size--;
    return 0;
}
*/
