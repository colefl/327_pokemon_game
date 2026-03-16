/*
 * Maps.h
 *
 *  Created on: Mar 16, 2026
 *      Author: colef
 */

#ifndef MAPS_H_
#define MAPS_H_
#include "world_gen.h"

typedef struct Maps {
    struct Map* m[401][401];
} Maps;

int initialize_all_maps(Maps *maps);
int initialize_maps(Maps *maps, int x, int y);
int add_maps(Maps *maps, int x, int y);
int cleanup(Maps *maps);
#endif
