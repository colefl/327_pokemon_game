/*
 * point_queue.c
 *
 *  Created on: Feb 2, 2026
 *      Author: colef
 *      Hello, this is me attempting to make a queue from the limited knowledge I have
 *      I found this resource to help: https://www.digitalocean.com/community/tutorials/queue-in-c
 */

#include <new>
#include <stdlib.h>
#include <stdio.h>
#include "point_queue.h"


void initialize_pq(point_queue *pq){
	pq->front = NULL;
	pq->rear = NULL;
	pq->size = 0;
}

int queue_size(point_queue *pq, int *size){
	*size = pq->size;
	return 0;
}

int enqueue(point_queue* q, int x, int y, char val) {
	queue_item *tmp = new(std::nothrow) queue_item; //Double check
    if(!tmp){
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

int dequeue(point_queue* q, int *x, int *y, char *val) { //Need to change this to return a queue_item instead
    if (q->front == NULL) {
        //printf("Queue is empty\n");
        return -1;
    }
    queue_item* temp = q->front;
    *x = temp->x;
    *y = temp-> y;
    *val = temp->value;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->size--;
    return 0;
}
