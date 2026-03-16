/*
 * point_queue.h
 *
 *  Created on: Mar 15, 2026
 *      Author: colef
 */

#ifndef POINT_QUEUE_H_
#define POINT_QUEUE_H_

#include <stdlib.h>
#include <stdio.h>


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

void initialize_pq(struct point_queue *pq);
int queue_size(struct point_queue *pq, int *size);
int enqueue(struct point_queue* q, int x, int y, char val);
int dequeue(struct point_queue* q, int *x, int *y, char *val) ;


#endif /* POINT_QUEUE_H_ */
