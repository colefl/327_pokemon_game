/*
 * pathMaker.c
 *
 *  Created on: Mar 15, 2026
 *      Author: colef
 */

#include "pathMaker.h"

#include "entities/Entity.h"

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

int rand_num;

pathMaker makePaths(pathMaker *pm){
	if(pm->map->entrances[North] == OutOfBounds && pm->map->entrances[West] == OutOfBounds){
			connect_northwest_corner(pm);

		//Top Right Corner
		} else if (pm->map->entrances[North] == OutOfBounds && pm->map->entrances[East] == OutOfBounds){
			connect_northeast_corner(pm);

		//Bottom Right Corner
		} else if (pm->map->entrances[South] == OutOfBounds && pm->map->entrances[West] == OutOfBounds){
			connect_southwest_corner(pm);

		//Bottom Left Corner
		} else if (pm->map->entrances[South] == OutOfBounds && pm->map->entrances[East] == OutOfBounds){
			connect_southeast_corner(pm);

		//North Edge
		} else if(pm->map->entrances[North] == OutOfBounds){
		    connectE_W(WORLDX - 1, pm->map->entrances[East], 0, pm->map->entrances[West], pm);
		    connect_upwards(pm->map->entrances[South], WORLDY - 1, pm);

		//South Edge
		} else if(pm->map->entrances[South] == OutOfBounds){
		    connectE_W(WORLDX - 1, pm->map->entrances[East], 0, pm->map->entrances[West], pm);
		    connect_down(pm->map->entrances[North], 0, pm);

		//East Edge
		} else if(pm->map->entrances[East] == OutOfBounds){
		    connectN_S(pm->map->entrances[North], 0, pm->map->entrances[South], WORLDY - 1, pm);
		    connect_right(0, pm->map->entrances[West], pm);

		//West Edge
		} else if(pm->map->entrances[West] == OutOfBounds){
		    connectN_S(pm->map->entrances[North], 0, pm->map->entrances[South], WORLDY - 1, pm);
		    connect_left(WORLDX - 1, pm->map->entrances[East], pm);

		//South Edge
		} else {
		    connectN_S(pm->map->entrances[North], 0, pm->map->entrances[South], WORLDY - 1, pm);
		    connectE_W(WORLDX - 1, pm->map->entrances[East], 0, pm->map->entrances[West], pm);
		}

	return *pm;
}

pathMaker makePathMaker(Map *m, entity *player){
	pathMaker tmp;

	tmp.map = m;
	tmp.player = *player;

	return tmp;
}

/*
Connects the east and west paths
*/
int connectE_W(int ex, int ey, int wx, int wy, pathMaker *pm){
	int rand_num;
	int count = 0;
	int pSpawn;
	bool pSpawned = false;
	bool spawned = false;

	//printf("hello I make it here to connect E_W\n");
	while(wx != ex || wy != ey){
		rand_num = rand();
		pSpawn = rand_num % 400 + 1;
		if(rand_num % 2 == 0 && wx != ex){
			if(wx + 1 < WORLDX && pm->map->arr[wx + 1][wy] != '%' && pm->map->arr[wx + 1][wy] != 'C' && pm->map->arr[wx + 1][wy] != 'M'){ //Checking to the right
				wx++;
				pm->map->arr[wx][wy] = '#';
				count++;
			}
			else if(wy > 0 && pm->map->arr[wx][wy - 1] != '%' && pm->map->arr[wx][wy - 1] != 'C' && pm->map->arr[wx][wy - 1] != 'M'){ //Checking to go down if blocked
				wy--;
				pm->map->arr[wx][wy] = '#';
			}
			else if(wy + 1 < WORLDY && pm->map->arr[wx][wy + 1] != '%' && pm->map->arr[wx][wy + 1] != 'C' && pm->map->arr[wx][wy + 1] != 'M'){ //Checking to go up
				wy++;
				pm->map->arr[wx][wy] = '#';
			}
		} else {
			if (ey < wy && wy > 0 && pm->map->arr[wx][wy - 1] != '%' && pm->map->arr[wx][wy - 1] != 'C' && pm->map->arr[wx][wy - 1] != 'M'){ //Checking to go down
				wy--;
				pm->map->arr[wx][wy] = '#';
			} else if (ey > wy && wy + 1 < WORLDY && pm->map->arr[wx][wy + 1] != '%' && pm->map->arr[wx][wy + 1] != 'C' && pm->map->arr[wx][wy + 1] != 'M'){ //Checking to go up if stuck
				wy++;
				pm->map->arr[wx][wy] = '#';
			}
			else if(wx + 1 < WORLDX && pm->map->arr[wx + 1][wy] != '%' && pm->map->arr[wx + 1][wy] != 'C' && pm->map->arr[wx + 1][wy] != 'M'){ //Checking to go right
				wx++;
				pm->map->arr[wx][wy] = '#';
				count++;
			}
		}

		if(count > 5 && !spawned){
			spawned = checkForCenter(wx, wy, pm);
		}
		if(count >= pSpawn && !pSpawned){
			pm->player = CreateEntity(PLAYER, wx, wy);
			pm->player.prev_tile = '#'; //Spawns on a path for now
			pm->map->arr[wx][wy] = pm->player.marker;
			pSpawned = true;
			rand_num = rand();
		}
	}
	return 0;
}

/*
Connects the north and south paths
*/
int connectN_S(int nx, int ny, int sx, int sy, pathMaker *pm){
	bool spawned = false;
	int count = 0;
	int rand_num;

	//printf("hello I make it to connectN_S\n");
	while(nx != sx || ny != sy){
		rand_num = rand();
		if(rand_num % 2 == 0 && ny != sy){
			if(ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
				ny++;
				pm->map->arr[nx][ny] = '#';
				count++;
			}
			else if(nx > 0 && pm->map->arr[nx - 1][ny] != '%' && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M'){
				nx--;
				pm->map->arr[nx][ny] = '#';
			}
			else if(nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != '%' && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M'){
				nx++;
				pm->map->arr[nx][ny] = '#';
			}
		} else {
			if (nx < sx && nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != '%' && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M'){
				nx++;
				pm->map->arr[nx][ny] = '#';
			} else if (nx > sx && nx > 0 && pm->map->arr[nx - 1][ny] != '%' && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M'){
				nx--;
				pm->map->arr[nx][ny] = '#';
			}
			else if(ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
				ny++;
				pm->map->arr[nx][ny] = '#';
				count++;
			}
		}

		if(count > 6 && !spawned){
			spawned = checkForMart(nx, ny, pm);
		}
	}

	return 0;
}

int connect_upwards(int sx, int sy, pathMaker *pm){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", sy, sx);

	//Continue until we hit a path '#'
	while(sy > 0 && pm->map->arr[sx][sy - 1] != '#'){
		if(rand_num % 2 == 0){
			if(sy > 0 && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
				sy--;
				pm->map->arr[sx][sy] = '#';
			} else if(sx + 1 < WORLDX && pm->map->arr[sx + 1][sy] != 'C' && pm->map->arr[sx + 1][sy] != 'M' && pm->map->arr[sx+1][sy] != '%'){
				sx++;
				pm->map->arr[sx][sy] = '#';
			} else if(sx > 0 && pm->map->arr[sx - 1][sy] != 'C' && pm->map->arr[sx - 1][sy] != 'M' && pm->map->arr[sx - 1][sy] != '%'){
				sx--;
				pm->map->arr[sx][sy] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && sx + 1 < WORLDX && pm->map->arr[sx + 1][sy] != 'C' && pm->map->arr[sx + 1][sy] != 'M' && pm->map->arr[sx - 1][sy] != '%'){
				sx++;
				pm->map->arr[sx][sy] = '#';
			} else if(sx > 0 && pm->map->arr[sx - 1][sy] != 'C' && pm->map->arr[sx - 1][sy] != 'M' && pm->map->arr[sx - 1][sy] != '%'){
				sx--;
				pm->map->arr[sx][sy] = '#';
			} else if(sy > 0 && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
				sy--;
				pm->map->arr[sx][sy] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_down(int nx, int ny, pathMaker *pm){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", ny, nx);

	//Continue until we hit a path '#' or reach the bottom
	while(ny < WORLDY - 1 && pm->map->arr[nx][ny + 1] != '#'){
		if(rand_num % 2 == 0){
			if(ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
				ny++;
				pm->map->arr[nx][ny] = '#';
			} else if(nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M' && pm->map->arr[nx + 1][ny] != '%'){
				nx++;
				pm->map->arr[nx][ny] = '#';
			} else if(nx > 0 && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M' && pm->map->arr[nx - 1][ny] != '%'){
				nx--;
				pm->map->arr[nx][ny] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M' && pm->map->arr[nx + 1][ny] != '%'){
				nx++;
				pm->map->arr[nx][ny] = '#';
			} else if(nx > 0 && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M' && pm->map->arr[nx - 1][ny] != '%'){
				nx--;
				pm->map->arr[nx][ny] = '#';
			} else if(ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
				ny++;
				pm->map->arr[nx][ny] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_right(int wx, int wy, pathMaker *pm){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", wy, wx);

	//Continue until it hits a path '#' or reach the left edge
	while(wx < WORLDX - 1 && pm->map->arr[wx + 1][wy] != '#'){
		if(rand_num % 2 == 0){
			// Move right (increase x)
			if(wx + 1 < WORLDX && pm->map->arr[wx + 1][wy] != 'C' && pm->map->arr[wx + 1][wy] != 'M'){
				wx++;
				pm->map->arr[wx][wy] = '#';
			} else if(wy > 0 && pm->map->arr[wx][wy - 1] != 'C' && pm->map->arr[wx][wy - 1] != 'M' && pm->map->arr[wx][wy - 1] != '%'){
				wy--;
				pm->map->arr[wx][wy] = '#';
			} else if(wy + 1 < WORLDY && pm->map->arr[wx][wy + 1] != 'C' && pm->map->arr[wx][wy + 1] != 'M' && pm->map->arr[wx][wy + 1] != '%'){
				wy++;
				pm->map->arr[wx][wy] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && wy + 1 < WORLDY && pm->map->arr[wx][wy + 1] != 'C' && pm->map->arr[wx][wy + 1] != 'M' && pm->map->arr[wx][wy + 1] != '%'){
				wy++;
				pm->map->arr[wx][wy] = '#';
			} else if(wy > 0 && pm->map->arr[wx][wy - 1] != 'C' && pm->map->arr[wx][wy - 1] != 'M' && pm->map->arr[wx][wy - 1] != '%'){
				wy--;
				pm->map->arr[wx][wy] = '#';
			} else if(wx + 1 < WORLDX && pm->map->arr[wx + 1][wy] != 'C' && pm->map->arr[wx + 1][wy] != 'M'){
				wx++;
				pm->map->arr[wx][wy] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

int connect_left(int ex, int ey, pathMaker *pm){
	rand_num = rand();
	printf("starting y: %d | starting x: %d\n", ey, ex);

	//Continue until it hits a path '#' or reach the left edge
	while(ex > 0 && pm->map->arr[ex - 1][ey] != '#'){
		if(rand_num % 2 == 0){
			if(ex > 0 && pm->map->arr[ex - 1][ey] != 'C' && pm->map->arr[ex - 1][ey] != 'M'){
				ex--;
				pm->map->arr[ex][ey] = '#';
			} else if(ey > 0 && pm->map->arr[ex][ey - 1] != 'C' && pm->map->arr[ex][ey - 1] != 'M' && pm->map->arr[ex][ey - 1] != '%'){
				ey--;
				pm->map->arr[ex][ey] = '#';
			} else if(ey + 1 < WORLDY && pm->map->arr[ex][ey + 1] != 'C' && pm->map->arr[ex][ey + 1] != 'M' && pm->map->arr[ex][ey + 1] != '%'){
				ey++;
				pm->map->arr[ex][ey] = '#';
			}
		} else {
			rand_num = rand();
			if(rand_num % 2 == 0 && ey + 1 < WORLDY && pm->map->arr[ex][ey + 1] != 'C' && pm->map->arr[ex][ey + 1] != 'M' && pm->map->arr[ex][ey + 1] != '%'){
				ey++;
				pm->map->arr[ex][ey] = '#';
			} else if(ey > 0 && pm->map->arr[ex][ey - 1] != 'C' && pm->map->arr[ex][ey - 1] != 'M' && pm->map->arr[ex][ey - 1] != '%'){
				ey--;
				pm->map->arr[ex][ey] = '#';
			} else if(ex > 0 && pm->map->arr[ex - 1][ey] != 'C' && pm->map->arr[ex - 1][ey] != 'M'){
				ex--;
				pm->map->arr[ex][ey] = '#';
			}
		}
		rand_num = rand();
	}
	return 0;
}

//only North and West entrances
int connect_southeast_corner(pathMaker *pm){
	if(pm->map->entrances[North] != OutOfBounds && pm->map->entrances[West] != OutOfBounds){
		int nx = pm->map->entrances[North];
		int ny = 0;
		int wx = 0;
		int wy = pm->map->entrances[West];

		while(nx != wx || ny != wy){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(nx > wx && nx > 0 && pm->map->arr[nx - 1][ny] != '%' && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M'){
					nx--;
					pm->map->arr[nx][ny] = '#';
				} else if(ny < wy && ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
					ny++;
					pm->map->arr[nx][ny] = '#';
				}
			} else {
				if(ny < wy && ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
					ny++;
					pm->map->arr[nx][ny] = '#';
				} else if(nx > wx && nx > 0 && pm->map->arr[nx - 1][ny] != '%' && pm->map->arr[nx - 1][ny] != 'C' && pm->map->arr[nx - 1][ny] != 'M'){
					nx--;
					pm->map->arr[nx][ny] = '#';
				}
			}
		}
	}
	return 0;
}

//only North and East entrances
int connect_southwest_corner(pathMaker *pm){
	if(pm->map->entrances[North] != OutOfBounds && pm->map->entrances[East] != OutOfBounds){
		int nx = pm->map->entrances[North];
		int ny = 0;
		int ex = WORLDX - 1;
		int ey = pm->map->entrances[East];

		while(nx != ex || ny != ey){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(nx < ex && nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != '%' && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M'){
					nx++;
					pm->map->arr[nx][ny] = '#';
				} else if(ny < ey && ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
					ny++;
					pm->map->arr[nx][ny] = '#';
				}
			} else {
				if(ny < ey && ny + 1 < WORLDY && pm->map->arr[nx][ny + 1] != '%' && pm->map->arr[nx][ny + 1] != 'C' && pm->map->arr[nx][ny + 1] != 'M'){
					ny++;
					pm->map->arr[nx][ny] = '#';
				} else if(nx < ex && nx + 1 < WORLDX && pm->map->arr[nx + 1][ny] != '%' && pm->map->arr[nx + 1][ny] != 'C' && pm->map->arr[nx + 1][ny] != 'M'){
					nx++;
					pm->map->arr[nx][ny] = '#';
				}
			}
		}
	}
	return 0;
}

//only South and West entrances
int connect_northeast_corner(pathMaker *pm){
	if(pm->map->entrances[South] != OutOfBounds && pm->map->entrances[West] != OutOfBounds){
		int sx = pm->map->entrances[South];
		int sy = WORLDY - 1;
		int wx = 0;
		int wy = pm->map->entrances[West];

		while(sx != wx || sy != wy){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(sx > wx && sx > 0 && pm->map->arr[sx - 1][sy] != '%' && pm->map->arr[sx - 1][sy] != 'C' && pm->map->arr[sx - 1][sy] != 'M'){
					sx--;
					pm->map->arr[sx][sy] = '#';
				} else if(sy > wy && sy > 0 && pm->map->arr[sx][sy - 1] != '%' && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
					sy--;
					pm->map->arr[sx][sy] = '#';
				}
			} else {
				if(sy > wy && sy > 0 && pm->map->arr[sx][sy - 1] != '%' && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
					sy--;
					pm->map->arr[sx][sy] = '#';
				} else if(sx > wx && sx > 0 && pm->map->arr[sx - 1][sy] != '%' && pm->map->arr[sx - 1][sy] != 'C' && pm->map->arr[sx - 1][sy] != 'M'){
					sx--;
					pm->map->arr[sx][sy] = '#';
				}
			}
		}
	}
	return 0;
}

//only South and East entrances
int connect_northwest_corner(pathMaker *pm){
	if(pm->map->entrances[South] != OutOfBounds && pm->map->entrances[East] != OutOfBounds){
		int sx = pm->map->entrances[South];
		int sy = WORLDY - 1;
		int ex = WORLDX - 1;
		int ey = pm->map->entrances[East];

		while(sx != ex || sy != ey){
			rand_num = rand();
			if(rand_num % 2 == 0){
				if(sx < ex && sx + 1 < WORLDX && pm->map->arr[sx + 1][sy] != '%' && pm->map->arr[sx + 1][sy] != 'C' && pm->map->arr[sx + 1][sy] != 'M'){
					sx++;
					pm->map->arr[sx][sy] = '#';
				} else if(sy > ey && sy > 0 && pm->map->arr[sx][sy - 1] != '%' && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
					sy--;
					pm->map->arr[sx][sy] = '#';
				}
			} else {
				if(sy > ey && sy > 0 && pm->map->arr[sx][sy - 1] != '%' && pm->map->arr[sx][sy - 1] != 'C' && pm->map->arr[sx][sy - 1] != 'M'){
					sy--;
					pm->map->arr[sx][sy] = '#';
				} else if(sx < ex && sx + 1 < WORLDX && pm->map->arr[sx + 1][sy] != '%' && pm->map->arr[sx + 1][sy] != 'C' && pm->map->arr[sx + 1][sy] != 'M'){
					sx++;
					pm->map->arr[sx][sy] = '#';
				}
			}
		}
	}
	return 0;
}

bool checkForCenter(int wx, int wy, pathMaker *pm){
	if (wx + 1 < WORLDX && wy + 2 < WORLDY) {
		if((pm->map->arr[wx][wy + 1] != '#' && pm->map->arr[wx][wy + 1] != '%')  && //bottom left
					(pm->map->arr[wx + 1][wy + 1] != '#' && pm->map->arr[wx + 1][wy + 1] != '%') && //bottom right
					(pm->map->arr[wx][wy + 2] != '#' && pm->map->arr[wx][wy + 2] != '%') && //top left
					(pm->map->arr[wx + 1][wy + 2] != '#' && pm->map->arr[wx + 1][wy + 2] != '%')){ //top right

				pm->map->arr[wx][wy + 1] = 'C';
				pm->map->arr[wx + 1][wy + 1] = 'C';
				pm->map->arr[wx][wy + 2] = 'C';
				pm->map->arr[wx + 1][wy + 2] = 'C';
				return true;
			}
	}
	return false;
}

bool checkForMart(int nx, int ny, pathMaker *pm){
	if (nx - 1 >= 0 && ny + 2 < WORLDY) {
		if((pm->map->arr[nx - 1][ny + 1] != '#' && pm->map->arr[nx - 1][ny + 1] != '%')  && //bottom left
					(pm->map->arr[nx][ny + 1] != '#' && pm->map->arr[nx][ny + 1] != '%') && //bottom right
					(pm->map->arr[nx - 1][ny + 2] != '#' && pm->map->arr[nx - 1][ny + 2] != '%') && //top left
					(pm->map->arr[nx][ny + 2] != '#' && pm->map->arr[nx][ny + 2] != '%')){ //top right

				pm->map->arr[nx - 1][ny + 1] = 'M';
				pm->map->arr[nx][ny + 1] = 'M';
				pm->map->arr[nx - 1][ny + 2] = 'M';
				pm->map->arr[nx][ny + 2] = 'M';
				return true;
			}
	}
	return false;
}
