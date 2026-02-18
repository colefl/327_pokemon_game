/*
 * world_gen.c
 *
 *  Created on: Jan 29, 2026
 *      Author: colef
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "heap.h"
//#include "stack.c"
#include "point_queue.c"
#include "entities/Entity.c"
//#include "Maps.c"

enum Entrances {
	North,
	West,
	South,
	East,
	OutOfBounds = 99
};

/*
 * floor is 80 by 21, terminal is 80 by 24
 * Some x and y as well as dx and dy and you write a couple of rectangles on top and generate them on the map
 * It's perfectly fine if things get overwritten
 * Now we run a path through it.
 * We need to choose some places on the edges and connect those points. Next, we can draw a path towards each other
 * until they are at the same place, then draw the distance between them.
 *
 * To check whether the direction is going east west versus north south, we can check whether the west_y - east_y
 * divided by the absolute value of this to get the direction of the path.
 *
 * Place seeds(points) around the map, then all of the c puts all of these into a queue. Then going through the queue,
 * all neighbors are checked to see if they're not occupied.
 * Can use dijkstras algorithm with weights added to the edges of the terrain types for pathing.
 *
 *For the pokemart, just search for a straight place along the path that is 2 wide.
 */

/*
 * Should create a struct for the maps.
 * Has thegate locations
 *
 * Should create another struct for maps for containing all of the maps.
 * This is going to be pointers so we can have null pointers(all maps are initially null)
 * When you go to a new map, check to see if it's null, if it's null, malloc a map
 *
 * Iterate over the whole map and deconstruct them before freeing the pointer
 *
 * World coords; we start in the center
 * The center is 200,200. But for the user's case, we subtract 200, 200.
 */

struct Map{
	char arr[80][21];
	bool visited[80][21];
	int entrances[4];
	bool omitDir[4];
};

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

//WORLD EDGE
int init_world_edge(struct Map *current_map);

int connectE_W(int ex, int ey, int wx, int wy, struct Map *m);
int connectN_S(int nx, int ny, int sx, int sy, struct Map *m);

//Edge Cases
int connect_upwards(int sx, int sy, struct Map *m);
int connect_down(int nx, int ny, struct Map *m);
int connect_right(int wx, int wy, struct Map *m);
int connect_left(int ex, int ey, struct Map *m);

//Corners
int connect_southeast_corner(struct Map *m);
int connect_southwest_corner(struct Map *m);
int connect_northwest_corner(struct Map *m);
int connect_northeast_corner(struct Map *m);


bool checkForCenter(int wx, int wy, struct Map *m);
bool checkForMart(int nx, int ny, struct Map *m);

//WORLD SPREAD
int get_num(int count, struct Map *m);
void DFS(struct point_queue *pq, struct Map *m);
bool canGrow(struct queue_item p, struct Map *m);

//SPAWNING ENTITIES
int spawnEntities(entity entities[5], int id, struct Map *m);
int32_t cell_compare(const void *key, const void *with);
int dijkstrasAlgo(struct Map *m, entity *player, entity *npc, int dist[80][21]);
static int getTerrainCost(char tile, entity *npc);

//Helper
int print_board(struct Map *m);
int print_costs(int arr[80][21], entity *player);

//Variables
int rand_num;
int count;
entity hikers[5];
entity player;
int hiker_dist[80][21];
//int x, y;
//struct Map m;
//int north_p, south_p, east_p, west_p;

int init_map(struct Map *m){
	int i, j;

    //initialize everything to avoid garbage values
    for(i = 0; i < WORLDX; i++){
        for(j = 0; j < WORLDY; j++){
            m->arr[i][j] = ' ';  // default empty space
            m->visited[i][j] = false;
        }
    }

	srand(time(NULL));

	rand_num = rand();

	get_num(count, m);

	struct point_queue pq;

	initialize_pq(&pq);

	enqueue(&pq, g1.x, g1.y, '.');
	enqueue(&pq, g2.x, g2.y, '.');
	enqueue(&pq, tg1.x, tg1.y, ':');
	enqueue(&pq, tg2.x, tg2.y, ':');
	enqueue(&pq, w.x, w.y, '~');

	DFS(&pq, m);

	init_world_edge(m);

	//west is at (0, m.entrances[1])
	//east is at (WORLDX-1, m.entrances[3])

	//Okay this might be the beginning of the end for me lol
	/*
	 * Edge possibilities
	 * North edge = connect east and west, have south connect upwards until it hits '#'
	 * South edge = connect east and west, have north connect downwards until it hits '#'
	 * East edge = connect north and south, have west connect to the right until it hits '#'
	 * West edge = connect north and south, have east connect to the right until it hits '#'
	 */
	//Need to add for the edges too now
	//Top left corner
	if(m->entrances[North] == OutOfBounds && m->entrances[West] == OutOfBounds){
		connect_northwest_corner(m);

	//Top Right Corner
	} else if (m->entrances[North] == OutOfBounds && m->entrances[East] == OutOfBounds){
		connect_northeast_corner(m);

	//Bottom Right Corner
	} else if (m->entrances[South] == OutOfBounds && m->entrances[West] == OutOfBounds){
		connect_southwest_corner(m);

	//Bottom Left Corner
	} else if (m->entrances[South] == OutOfBounds && m->entrances[East] == OutOfBounds){
		connect_southeast_corner(m);

	//North Edge
	} else if(m->entrances[North] == OutOfBounds){
	    connectE_W(WORLDX - 1, m->entrances[East], 0, m->entrances[West], m);
	    connect_upwards(m->entrances[South], WORLDY - 1, m);

	//South Edge
	} else if(m->entrances[South] == OutOfBounds){
	    connectE_W(WORLDX - 1, m->entrances[East], 0, m->entrances[West], m);
	    connect_down(m->entrances[North], 0, m);

	//East Edge
	} else if(m->entrances[East] == OutOfBounds){
	    connectN_S(m->entrances[North], 0, m->entrances[South], WORLDY - 1, m);
	    connect_right(0, m->entrances[West], m);

	//West Edge
	} else if(m->entrances[West] == OutOfBounds){
	    connectN_S(m->entrances[North], 0, m->entrances[South], WORLDY - 1, m);
	    connect_left(WORLDX - 1, m->entrances[East], m);

	//South Edge
	} else {
	    connectN_S(m->entrances[North], 0, m->entrances[South], WORLDY - 1, m);
	    connectE_W(WORLDX - 1, m->entrances[East], 0, m->entrances[West], m);
	}
	//north is at (m.entrances[0], 0)
	//south is at (m.entrances[2], WORLDY-1)
//	connectN_S(m->entrances[North], 0, m->entrances[South], WORLDY - 1, m);

	spawnEntities(hikers, HIKER,  m);

	dijkstrasAlgo(m, &player, &hikers[0], hiker_dist);

	print_costs(hiker_dist, &player);

	return 0;
}

int init_world_edge(struct Map *current_map){
	int i,j;
	if(current_map->entrances[North] == OutOfBounds){
		current_map->omitDir[North] = true;
	} else if(current_map->entrances[North] == 0){
		current_map->entrances[North] = (rand_num % 70) + 9;
		rand_num = rand();
	}

	if(current_map->entrances[West] == OutOfBounds){
		current_map->omitDir[West] = true;
	} else if(current_map->entrances[West] == 0){
		current_map->entrances[West] = (rand_num % 12) + 3;
		rand_num = rand();
	}

	if(current_map->entrances[South] == OutOfBounds){
		current_map->omitDir[South] = true;
	} else if(current_map->entrances[South] == 0){
		current_map->entrances[South] = (rand_num % 70) + 9;
		rand_num = rand();
	}

	if(current_map->entrances[East] == OutOfBounds){
		current_map->omitDir[East] = true;
	} else if(current_map->entrances[East] == 0){
		current_map->entrances[East] = (rand_num % 12) + 3;
		rand_num = rand();
	}

	//top and bottom edges
	for(i = 0; i < WORLDX; i++){
		current_map->arr[i][0] = '%';
		current_map->arr[i][WORLDY - 1] = '%';
	}
	if(!current_map->omitDir[North]){
		current_map->arr[current_map->entrances[North]][0] = '#'; //north
		//This also means that after the program has ran, the entrance may have the entrance value as a 99 still, meaning it's capable of checking
	}
	if(!current_map->omitDir[South]){
		current_map->arr[current_map->entrances[South]][WORLDY - 1] = '#'; //south
	}

	printf("north path: %d || south path: %d\n", current_map->entrances[North], current_map->entrances[South]);

	//left and right edge
	for(j = 0; j < WORLDY; j++){
		current_map->arr[0][j] = '%';
		current_map->arr[WORLDX - 1][j] = '%';
	}
	if(!current_map->omitDir[West]){
		current_map->arr[0][current_map->entrances[West]] = '#'; //west
	}
	if(!current_map->omitDir[East]){
		current_map->arr[WORLDX - 1][current_map->entrances[East]] = '#'; //east
	}

	printf("west path: %d || east path: %d\n", current_map->entrances[West], current_map->entrances[East]);

	return 0;
}

/*
Connects the east and west paths
*/
int connectE_W(int ex, int ey, int wx, int wy, struct Map *m){
	int rand_num;
	int count = 0;
	int pSpawn;
	bool pSpawned = false;
	bool spawned = false;

	while(wx != ex || wy != ey){
		rand_num = rand();
		pSpawn = rand_num % 400 + 1;
		if(rand_num % 2 == 0 && wx != ex){
			if(wx + 1 < WORLDX && m->arr[wx + 1][wy] != '%' && m->arr[wx + 1][wy] != 'C' && m->arr[wx + 1][wy] != 'M'){ //Checking to the right
				wx++;
				m->arr[wx][wy] = '#';
				count++;
			}
			else if(wy > 0 && m->arr[wx][wy - 1] != '%' && m->arr[wx][wy - 1] != 'C' && m->arr[wx][wy - 1] != 'M'){ //Checking to go down if blocked
				wy--;
				m->arr[wx][wy] = '#';
			}
			else if(wy + 1 < WORLDY && m->arr[wx][wy + 1] != '%' && m->arr[wx][wy + 1] != 'C' && m->arr[wx][wy + 1] != 'M'){ //Checking to go up
				wy++;
				m->arr[wx][wy] = '#';
			}
		} else {
			if (ey < wy && wy > 0 && m->arr[wx][wy - 1] != '%' && m->arr[wx][wy - 1] != 'C' && m->arr[wx][wy - 1] != 'M'){ //Checking to go down
				wy--;
				m->arr[wx][wy] = '#';
			} else if (ey > wy && wy + 1 < WORLDY && m->arr[wx][wy + 1] != '%' && m->arr[wx][wy + 1] != 'C' && m->arr[wx][wy + 1] != 'M'){ //Checking to go up if stuck
				wy++;
				m->arr[wx][wy] = '#';
			}
			else if(wx + 1 < WORLDX && m->arr[wx + 1][wy] != '%' && m->arr[wx + 1][wy] != 'C' && m->arr[wx + 1][wy] != 'M'){ //Checking to go right
				wx++;
				m->arr[wx][wy] = '#';
				count++;
			}
		}

		if(count > 5 && !spawned){
			spawned = checkForCenter(wx, wy, m);
		}
		if(count >= pSpawn && !pSpawned){
			player = CreateEntity(PLAYER, wx, wy);
			m->arr[wx][wy] =  player.type; //Need to swap for enemy init
			pSpawned = true;
			rand_num = rand();
		}
	}
	return 0;
}

/*
Connects the north and south paths
*/
int connectN_S(int nx, int ny, int sx, int sy, struct Map *m){
	bool spawned = false;
	int count = 0;
	int rand_num;

	while(nx != sx || ny != sy){
		rand_num = rand();
		if(rand_num % 2 == 0 && ny != sy){
			if(ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
				ny++;
				m->arr[nx][ny] = '#';
				count++;
			}
			else if(nx > 0 && m->arr[nx - 1][ny] != '%' && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M'){
				nx--;
				m->arr[nx][ny] = '#';
			}
			else if(nx + 1 < WORLDX && m->arr[nx + 1][ny] != '%' && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M'){
				nx++;
				m->arr[nx][ny] = '#';
			}
		} else {
			if (nx < sx && nx + 1 < WORLDX && m->arr[nx + 1][ny] != '%' && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M'){
				nx++;
				m->arr[nx][ny] = '#';
			} else if (nx > sx && nx > 0 && m->arr[nx - 1][ny] != '%' && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M'){
				nx--;
				m->arr[nx][ny] = '#';
			}
			else if(ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
				ny++;
				m->arr[nx][ny] = '#';
				count++;
			}
		}

		if(count > 6 && !spawned){
			spawned = checkForMart(nx, ny, m);
		}
	}

	return 0;
}

int connect_upwards(int sx, int sy, struct Map *m){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", sy, sx);

	//Continue until we hit a path '#'
	while(sy > 0 && m->arr[sx][sy - 1] != '#'){
		if(rand_num % 2 == 0){
			if(sy > 0 && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
				sy--;
				m->arr[sx][sy] = '#';
			} else if(sx + 1 < WORLDX && m->arr[sx + 1][sy] != 'C' && m->arr[sx + 1][sy] != 'M' && m->arr[sx+1][sy] != '%'){
				sx++;
				m->arr[sx][sy] = '#';
			} else if(sx > 0 && m->arr[sx - 1][sy] != 'C' && m->arr[sx - 1][sy] != 'M' && m->arr[sx - 1][sy] != '%'){
				sx--;
				m->arr[sx][sy] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && sx + 1 < WORLDX && m->arr[sx + 1][sy] != 'C' && m->arr[sx + 1][sy] != 'M' && m->arr[sx - 1][sy] != '%'){
				sx++;
				m->arr[sx][sy] = '#';
			} else if(sx > 0 && m->arr[sx - 1][sy] != 'C' && m->arr[sx - 1][sy] != 'M' && m->arr[sx - 1][sy] != '%'){
				sx--;
				m->arr[sx][sy] = '#';
			} else if(sy > 0 && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
				sy--;
				m->arr[sx][sy] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_down(int nx, int ny, struct Map *m){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", ny, nx);

	//Continue until we hit a path '#' or reach the bottom
	while(ny < WORLDY - 1 && m->arr[nx][ny + 1] != '#'){
		if(rand_num % 2 == 0){
			if(ny + 1 < WORLDY && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
				ny++;
				m->arr[nx][ny] = '#';
			} else if(nx + 1 < WORLDX && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M' && m->arr[nx + 1][ny] != '%'){
				nx++;
				m->arr[nx][ny] = '#';
			} else if(nx > 0 && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M' && m->arr[nx - 1][ny] != '%'){
				nx--;
				m->arr[nx][ny] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && nx + 1 < WORLDX && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M' && m->arr[nx + 1][ny] != '%'){
				nx++;
				m->arr[nx][ny] = '#';
			} else if(nx > 0 && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M' && m->arr[nx - 1][ny] != '%'){
				nx--;
				m->arr[nx][ny] = '#';
			} else if(ny + 1 < WORLDY && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
				ny++;
				m->arr[nx][ny] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_right(int wx, int wy, struct Map *m){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", wy, wx);

	//Continue until it hits a path '#' or reach the left edge
	while(wx < WORLDX - 1 && m->arr[wx + 1][wy] != '#'){
		if(rand_num % 2 == 0){
			// Move right (increase x)
			if(wx + 1 < WORLDX && m->arr[wx + 1][wy] != 'C' && m->arr[wx + 1][wy] != 'M'){
				wx++;
				m->arr[wx][wy] = '#';
			} else if(wy > 0 && m->arr[wx][wy - 1] != 'C' && m->arr[wx][wy - 1] != 'M' && m->arr[wx][wy - 1] != '%'){
				wy--;
				m->arr[wx][wy] = '#';
			} else if(wy + 1 < WORLDY && m->arr[wx][wy + 1] != 'C' && m->arr[wx][wy + 1] != 'M' && m->arr[wx][wy + 1] != '%'){
				wy++;
				m->arr[wx][wy] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && wy + 1 < WORLDY && m->arr[wx][wy + 1] != 'C' && m->arr[wx][wy + 1] != 'M' && m->arr[wx][wy + 1] != '%'){
				wy++;
				m->arr[wx][wy] = '#';
			} else if(wy > 0 && m->arr[wx][wy - 1] != 'C' && m->arr[wx][wy - 1] != 'M' && m->arr[wx][wy - 1] != '%'){
				wy--;
				m->arr[wx][wy] = '#';
			} else if(wx + 1 < WORLDX && m->arr[wx + 1][wy] != 'C' && m->arr[wx + 1][wy] != 'M'){
				wx++;
				m->arr[wx][wy] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_left(int ex, int ey, struct Map *m){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", ey, ex);

	//Continue until it hits a path '#' or reach the left edge
	while(ex > 0 && m->arr[ex - 1][ey] != '#'){
		if(rand_num % 2 == 0){
			if(ex > 0 && m->arr[ex - 1][ey] != 'C' && m->arr[ex - 1][ey] != 'M'){
				ex--;
				m->arr[ex][ey] = '#';
			} else if(ey > 0 && m->arr[ex][ey - 1] != 'C' && m->arr[ex][ey - 1] != 'M' && m->arr[ex][ey - 1] != '%'){
				ey--;
				m->arr[ex][ey] = '#';
			} else if(ey + 1 < WORLDY && m->arr[ex][ey + 1] != 'C' && m->arr[ex][ey + 1] != 'M' && m->arr[ex][ey + 1] != '%'){
				ey++;
				m->arr[ex][ey] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && ey + 1 < WORLDY && m->arr[ex][ey + 1] != 'C' && m->arr[ex][ey + 1] != 'M' && m->arr[ex][ey + 1] != '%'){
				ey++;
				m->arr[ex][ey] = '#';
			} else if(ey > 0 && m->arr[ex][ey - 1] != 'C' && m->arr[ex][ey - 1] != 'M' && m->arr[ex][ey - 1] != '%'){
				ey--;
				m->arr[ex][ey] = '#';
			} else if(ex > 0 && m->arr[ex - 1][ey] != 'C' && m->arr[ex - 1][ey] != 'M'){
				ex--;
				m->arr[ex][ey] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

//only North and West entrances
int connect_southeast_corner(struct Map *m){
	if(m->entrances[North] != OutOfBounds && m->entrances[West] != OutOfBounds){
		int nx = m->entrances[North];
		int ny = 0;
		int wx = 0;
		int wy = m->entrances[West];

		while(nx != wx || ny != wy){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(nx > wx && nx > 0 && m->arr[nx - 1][ny] != '%' && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M'){
					nx--;
					m->arr[nx][ny] = '#';
				} else if(ny < wy && ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
					ny++;
					m->arr[nx][ny] = '#';
				}
			} else {
				if(ny < wy && ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
					ny++;
					m->arr[nx][ny] = '#';
				} else if(nx > wx && nx > 0 && m->arr[nx - 1][ny] != '%' && m->arr[nx - 1][ny] != 'C' && m->arr[nx - 1][ny] != 'M'){
					nx--;
					m->arr[nx][ny] = '#';
				}
			}
		}
	}
	return 0;
}

//only North and East entrances
int connect_southwest_corner(struct Map *m){
	if(m->entrances[North] != OutOfBounds && m->entrances[East] != OutOfBounds){
		int nx = m->entrances[North];
		int ny = 0;
		int ex = WORLDX - 1;
		int ey = m->entrances[East];

		while(nx != ex || ny != ey){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(nx < ex && nx + 1 < WORLDX && m->arr[nx + 1][ny] != '%' && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M'){
					nx++;
					m->arr[nx][ny] = '#';
				} else if(ny < ey && ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
					ny++;
					m->arr[nx][ny] = '#';
				}
			} else {
				if(ny < ey && ny + 1 < WORLDY && m->arr[nx][ny + 1] != '%' && m->arr[nx][ny + 1] != 'C' && m->arr[nx][ny + 1] != 'M'){
					ny++;
					m->arr[nx][ny] = '#';
				} else if(nx < ex && nx + 1 < WORLDX && m->arr[nx + 1][ny] != '%' && m->arr[nx + 1][ny] != 'C' && m->arr[nx + 1][ny] != 'M'){
					nx++;
					m->arr[nx][ny] = '#';
				}
			}
		}
	}
	return 0;
}

//only South and West entrances
int connect_northeast_corner(struct Map *m){
	if(m->entrances[South] != OutOfBounds && m->entrances[West] != OutOfBounds){
		int sx = m->entrances[South];
		int sy = WORLDY - 1;
		int wx = 0;
		int wy = m->entrances[West];

		while(sx != wx || sy != wy){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(sx > wx && sx > 0 && m->arr[sx - 1][sy] != '%' && m->arr[sx - 1][sy] != 'C' && m->arr[sx - 1][sy] != 'M'){
					sx--;
					m->arr[sx][sy] = '#';
				} else if(sy > wy && sy > 0 && m->arr[sx][sy - 1] != '%' && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
					sy--;
					m->arr[sx][sy] = '#';
				}
			} else {
				if(sy > wy && sy > 0 && m->arr[sx][sy - 1] != '%' && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
					sy--;
					m->arr[sx][sy] = '#';
				} else if(sx > wx && sx > 0 && m->arr[sx - 1][sy] != '%' && m->arr[sx - 1][sy] != 'C' && m->arr[sx - 1][sy] != 'M'){
					sx--;
					m->arr[sx][sy] = '#';
				}
			}
		}
	}
	return 0;
}

//only South and East entrances
int connect_northwest_corner(struct Map *m){
	if(m->entrances[South] != OutOfBounds && m->entrances[East] != OutOfBounds){
		int sx = m->entrances[South];
		int sy = WORLDY - 1;
		int ex = WORLDX - 1;
		int ey = m->entrances[East];

		while(sx != ex || sy != ey){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(sx < ex && sx + 1 < WORLDX && m->arr[sx + 1][sy] != '%' && m->arr[sx + 1][sy] != 'C' && m->arr[sx + 1][sy] != 'M'){
					sx++;
					m->arr[sx][sy] = '#';
				} else if(sy > ey && sy > 0 && m->arr[sx][sy - 1] != '%' && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
					sy--;
					m->arr[sx][sy] = '#';
				}
			} else {
				if(sy > ey && sy > 0 && m->arr[sx][sy - 1] != '%' && m->arr[sx][sy - 1] != 'C' && m->arr[sx][sy - 1] != 'M'){
					sy--;
					m->arr[sx][sy] = '#';
				} else if(sx < ex && sx + 1 < WORLDX && m->arr[sx + 1][sy] != '%' && m->arr[sx + 1][sy] != 'C' && m->arr[sx + 1][sy] != 'M'){
					sx++;
					m->arr[sx][sy] = '#';
				}
			}
		}
	}
	return 0;
}

bool checkForCenter(int wx, int wy, struct Map *m){
	if (wx + 1 < WORLDX && wy + 2 < WORLDY) {
		if((m->arr[wx][wy + 1] != '#' && m->arr[wx][wy + 1] != '%')  && //bottom left
					(m->arr[wx + 1][wy + 1] != '#' && m->arr[wx + 1][wy + 1] != '%') && //bottom right
					(m->arr[wx][wy + 2] != '#' && m->arr[wx][wy + 2] != '%') && //top left
					(m->arr[wx + 1][wy + 2] != '#' && m->arr[wx + 1][wy + 2] != '%')){ //top right

				m->arr[wx][wy + 1] = 'C';
				m->arr[wx + 1][wy + 1] = 'C';
				m->arr[wx][wy + 2] = 'C';
				m->arr[wx + 1][wy + 2] = 'C';
				return true;
			}
	}
	return false;
}

bool checkForMart(int nx, int ny, struct Map *m){
	if (nx - 1 >= 0 && ny + 2 < WORLDY) {
		if((m->arr[nx - 1][ny + 1] != '#' && m->arr[nx - 1][ny + 1] != '%')  && //bottom left
					(m->arr[nx][ny + 1] != '#' && m->arr[nx][ny + 1] != '%') && //bottom right
					(m->arr[nx - 1][ny + 2] != '#' && m->arr[nx - 1][ny + 2] != '%') && //top left
					(m->arr[nx][ny + 2] != '#' && m->arr[nx][ny + 2] != '%')){ //top right

				m->arr[nx - 1][ny + 1] = 'M';
				m->arr[nx][ny + 1] = 'M';
				m->arr[nx - 1][ny + 2] = 'M';
				m->arr[nx][ny + 2] = 'M';
				return true;
			}
	}
	return false;
}

bool canGrow(struct queue_item p, struct Map *m){

	if(m->arr[p.x][p.y] == '#' || m->arr[p.x][p.y] == '%' || m->visited[p.x][p.y] || p.x >= WORLDX - 1 || p.y >= WORLDY - 1 || p.x <= 0 || p.y <= 0){
		return false;
	}
	m->arr[p.x][p.y] = p.value;
	return true;
}

void DFS(struct point_queue *pq, struct Map *m){
	//queue_size(pq, &size); //Need to figure out where this comes from
	//initialize_pq(*pq);
	 //I think I need to update this to take on points, then I can have it push on the four points. But I would need it to get pushed to the back instead of the front
	while(pq->front != NULL){
		struct queue_item tmp;
		dequeue(pq, &tmp.x, &tmp.y, &tmp.value);
		if(!(canGrow(tmp, m))){
			continue;
		}

		m->visited[tmp.x][tmp.y] = true;

		enqueue(pq, tmp.x + 1, tmp.y, tmp.value);
		enqueue(pq, tmp.x - 1, tmp.y, tmp.value);
		enqueue(pq, tmp.x, tmp.y + 1, tmp.value);
		enqueue(pq, tmp.x, tmp.y - 1, tmp.value);

		//queue_size(pq, &size);
		//First I'm going to check to see if the top on the stack is good
		//Next I'm going to set it's visited status to be true
		//Next I'm going to add all of it's adjacent tiles to the stack to be tested.
	}
}

int get_num(int count, struct Map *m){

	g1.x = (rand_num % 78) + 1;
	g1.y = (rand_num % 19) + 1;

	//printf("grass 1 x: %d | grass 1 y: %d\n", g1.x, g1.y);;

	m->arr[g1.x][g1.y] = '.';

	rand_num = rand();

	g2.x = (rand_num % 78) + 1;
	g2.y = (rand_num % 19) + 1;

	//printf("grass 2 x: %d | grass 2 y: %d\n", g2.x, g2.y);;

	m->arr[g2.x][g2.y] = '.';

	rand_num = rand();

	tg1.x = (rand_num % 78) + 1;
	tg1.y = (rand_num % 19) + 1;

	m->arr[tg1.x][tg1.y] = ':';

	rand_num = rand();

	tg2.x = (rand_num % 78) + 1;
	tg2.y = (rand_num % 19) + 1;

	m->arr[tg2.x][tg2.y] = ':';

	rand_num = rand();

	w.x = (rand_num % 78) + 1;
	w.y = (rand_num % 19) + 1;

	m->arr[w.x][w.y] = '~';

	rand_num = rand();


	return 0;
}

//NEED TO INITIALIZE FUNCTION UP TOP
int spawnEntities(entity entities[5], int id, struct Map *m){
	//Spawn Hiker
	int rand_x;
	int rand_y;
	int i;
	for(i = 0; i < 5; i++){
//		if(entities[i] == NULL){ //NEED TO ADD SOME CATCHES FOR  NULL VALUES
//			continue;
//		}
		while(!entities[i].isSpawned){
				rand_x = rand() % 79;
				rand_y = rand() % 19;
				if(m->arr[rand_x][rand_y] != '.'){
					continue;
				} else {
					m->arr[rand_x][rand_y] = 'h';
					entities[i] = CreateEntity(id, rand_x, rand_y);
					entities[i].isSpawned = true;
				}
			}
	}
	return 0;
}

int32_t cell_compare(const void *key, const void *with) {
    return ((map_cell_t *)key)->cost - ((map_cell_t *)with)->cost;
}

int dijkstrasAlgo(struct Map *m, entity *player, entity *npc, int dist[80][21]){
    heap_t h;
    heap_node_t *nodes[80][21];
    map_cell_t  *cells[80][21];

    int x, y;
    for (x = 0; x < 80; x++) {
        for (y = 0; y < 21; y++) {
            dist[x][y] = INT_MAX;
            nodes[x][y] = NULL;
            cells[x][y] = NULL;
        }
    }

    heap_init(&h, cell_compare, free);

    map_cell_t *start = malloc(sizeof(map_cell_t)); //Evil segmentation arror...
    start->x = player->x;
    start->y = player->y;
    start->cost = 0;
    dist[player->x][player->y] = 0;
    cells[player->x][player->y] = start;
    nodes[player->x][player->y] = heap_insert(&h, start); //Inserting the initial heap node

    //Same thing as what I used for knights tour
    int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (h.size > 0) {
        map_cell_t *cur = heap_remove_min(&h);
        int cx = cur->x, cy = cur->y;

        //Getting all of the surrounding nodes inside
        for (int d = 0; d < 8; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= 80 || ny < 0 || ny >= 21) { //Bounds checking
            	continue;
            }

            int weight = getTerrainCost(m->arr[nx][ny], npc);//Outsourced to a static method like we learned in class today
            if (weight == INT_MAX) continue;

            if((nx == 79 && m->arr[nx][ny] == '#')){ //This is just overwriting the gates with INT_MAX because I didn't set up that infrastructure in the code.
            	weight = INT_MAX;
            	dist[nx][ny] = weight;
            	printf("I'm overriding the pos");
            	continue;
            } else if((ny == 0 && m->arr[nx][ny] == '#')){
            	weight = INT_MAX;
            	dist[nx][ny] = weight;
            	continue;
            } else if((nx == 0 && m->arr[nx][ny] == '#')){
            	weight = INT_MAX;
            	dist[nx][ny] = weight;
            	continue;
            } else if (ny == 20 && m->arr[nx][ny] == '#'){
            	weight = INT_MAX;
            	dist[nx][ny] = weight;
            	continue;
            }

            int new_cost = dist[cx][cy] + weight;//[x][y]

            if (new_cost < dist[nx][ny]) {
                dist[nx][ny] = new_cost;

                if (nodes[nx][ny] == NULL) {
                    map_cell_t *cell = malloc(sizeof(map_cell_t)); //Evil segmentation error part 2 except I got it this time
                    cell->x = nx;
                    cell->y = ny;
                    cell->cost = new_cost;
                    cells[nx][ny] = cell;
                    nodes[nx][ny] = heap_insert(&h, cell);
                } else {
                    cells[nx][ny]->cost = new_cost;
                    heap_decrease_key_no_replace(&h, nodes[nx][ny]); //Replace the lowest number out of the 8
                }
            }
        }
    }

    heap_delete(&h); //Delete heap at the end
    return 0;
}
//Weights go Bldr, Tree, Path, Pmart, Pcenter, TGras, SGras, Water, Gate
/*
 * Key:
 * Bldr = 0
 * Tree = 1
 * Path = 2
 * PMart = 3
 * Pcenter = 4
 * TGras = 5
 * SGras = 6
 * Water = 7
 * Gate = 8;
 */
static int getTerrainCost(char tile, entity *npc){
	switch(tile){
	case '%':
		return npc->weights[0];
	case '^': //I know I haven't made the trees yet I want to but I need more time
		return npc->weights[1];
	case '#':
		return npc->weights[2];
	case 'M':
		return npc->weights[3];
	case 'C':
		return npc->weights[4];
	case ':':
		return npc->weights[5];
	case '.':
		return npc->weights[6];
	case '~':
		return npc->weights[7];
		//No distinction for gates yet... Hopefully will not come back to bite me.
	}
	return 0;
}

int print_board(struct Map *m){
	int i,j;
	//j is x and i is y in this case
	for(i = 0; i < WORLDY; i++){
		for(j = 0; j < WORLDX; j++){
			printf("%c", m->arr[j][i]);
		}
		printf("\n");
	}
	return 0;
}

int print_costs(int arr[80][21], entity *player){
    int x, y;
    for(y = 0; y < 21; y++){
        for(x = 0; x < 80; x++){
            if(x == player->x && y == player->y){
                printf(" @");
                continue;
            }
            if(arr[x][y] == INT_MAX){
                printf("  ");
                continue;
            }
            printf("%2d", arr[x][y] % 100);
        }
        printf("\n");
    }
    return 0;
}
