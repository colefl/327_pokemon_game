/*
 * pathmaker.h
 *
 *  Created on: Mar 16, 2026
 *      Author: colef
 */

#ifndef PATHMAKER_H_
#define PATHMAKER_H_


#include "world_gen.h"

typedef struct {
    Map *map;
    entity* player;
} pathMaker;


pathMaker makePaths(pathMaker *pm);

pathMaker makePathMaker(Map *m, entity *player);

int connectE_W(int ex, int ey, int wx, int wy, pathMaker *pm);
int connectN_S(int nx, int ny, int sx, int sy, pathMaker *pm);

//Edge Cases
int connect_upwards(int sx, int sy, pathMaker *pm);
int connect_down(int nx, int ny, pathMaker *pm);
int connect_right(int wx, int wy, pathMaker *pm);
int connect_left(int ex, int ey, pathMaker *pm);

//Corners
int connect_southeast_corner(pathMaker *pm);
int connect_southwest_corner(pathMaker *pm);
int connect_northwest_corner(pathMaker *pm);
int connect_northeast_corner(pathMaker *pm);


bool checkForCenter(int wx, int wy, pathMaker *pm);
bool checkForMart(int nx, int ny, pathMaker *pm);


#endif /* PATHMAKER_H_ */
