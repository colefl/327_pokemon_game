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

typedef struct {
	int x;
	int y;
	char type;
	int weights[];
} entity;
