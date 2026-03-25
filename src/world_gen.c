/*
 * world_gen.c
 *
 *  Created on: Jan 29, 2026
 *      Author: colef
 */

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "heap.h"
//#include "stack.c"
#include "point_queue.c"
#include "pathMaker.h"
//#include "entities/Entity.c"
//#include "entities/entity_d_array.c"
//#include "Maps.c"

/*
 * floor is 80 by 21, terminal is 80 by 24
 *
 * To check whether the direction is going east west versus north south, we can check whether the west_y - east_y
 * divided by the absolute value of this to get the direction of the path.
 *
 *World Gen:
 * Place seeds(points) around the map, then all of the c puts all of these into a queue. Then going through the queue,
 * all neighbors are checked to see if they're not occupied using depth first search
 *
 *For the pokemart, just search for a straight place along the path that is 2 wide.
 */

//Methods

//WORLD EDGE
int init_world_edge(struct Map *current_map);

//WORLD SPREAD
int get_num(int count, struct Map *m);
void DFS(struct point_queue *pq, struct Map *m);
bool canGrow(struct queue_item p, struct Map *m);
int pepperInTrees(struct Map *m);

//SPAWNING ENTITIES
int spawnEntities(heap_t *eq, entity* entities[], int rand_num, struct Map *m);
int spawnEntity(entity *npc, int id, struct Map *m);
int32_t cell_compare(const void *key, const void *with);
int dijkstrasAlgo(struct Map *m, int x, int y, entity *npc, int dist[80][21]);
static int getTerrainCost(char tile, entity *npc);
int check_if_spawns_on(char tile, char spawnables[4]);

//GAMELOOP
void runGameLoop(heap_t *eq, entity* entities[], struct Map *m, int num_of_npcs);
int handle_npc_movement(entity *npc, int dist[80][21], struct Map *m, entity* entities[]);
int handle_wanderer_movement(entity *npc, struct Map *m);
int handle_pacer_movement(entity *npc, struct Map *m);
int handle_explorer_movement(entity *npc, struct Map *m);
static int is_border(int x, int y, struct Map *m);
static int is_occupied(int x, int y, struct Map *m, entity *self);

//Helper
int print_board(struct Map *m);
int print_costs(int arr[80][21], entity *player);
int paint_board(struct Map *m);
int run_battle_sequence();
entity* start_battle_state(Map *m, entity *npc);
static entity* player_entity_collision(entity* entities[], int target_x, int target_y, int n);
void toggle_npc_window(entity* entities[], int num_of_npcs, bool *window_open);

//Variables

int rand_num;
int count;
uint32_t player_time = 0;

entity *player;
heap_t eq; //Entity queue

int hiker_dist[80][21];
int rival_dist[80][21];
int center_dist[80][21];

int centerX;
int centerY;

struct point g1, g2, tg1, tg2, w;


int init_map(struct Map *m){
	int i, j;
    for(i = 0; i < WORLDX; i++){
        for(j = 0; j < WORLDY; j++){
            m->arr[i][j] = ' ';  // default empty space
            m->visited[i][j] = false;
        }
    }
    //printf("Hello I think I know what the issue is\n");

	srand(time(NULL));

	rand_num = rand();

	get_num(count, m);

	struct point_queue pq;

	initialize_pq(&pq);

	init_game_queue(&eq);

	enqueue(&pq, g1.x, g1.y, '.');
	enqueue(&pq, g2.x, g2.y, '.');
	enqueue(&pq, tg1.x, tg1.y, ':');
	enqueue(&pq, tg2.x, tg2.y, ':');
	enqueue(&pq, w.x, w.y, '~');

	DFS(&pq, m);

	pepperInTrees(m);

	init_world_edge(m);

	player = malloc(sizeof (entity)); //Need to free

	//printf("I make it past init_world_edge\n");
	pathMaker pm = makePathMaker(m, player);
	makePaths(&pm); //This may break when used along other maps...
	m = pm.map;
	centerX = pm.centerLocationX;
	centerY = pm.centerLocationY;
	//printf("Hello I make it to makePaths\n");
	player = pm.player;
	//printf("Hello the error is here lol\n");

	int num_of_npcs;
	num_of_npcs = rand() % 4 + 9;

	entity* entities[num_of_npcs];

	//printf("hello I make it before spawnEntities\n");

	spawnEntities(&eq, entities, num_of_npcs,  m);

	//printf("hello I make it here to spawnEntities\n");

	entity* hiker_template = CreateEntity(HIKER, 0, 0);
	entity* rival_template = CreateEntity(RIVAL, 0, 0);
	dijkstrasAlgo(m, player->x, player->y, hiker_template, hiker_dist);
	dijkstrasAlgo(m, player->x, player->y, rival_template, rival_dist);
	dijkstrasAlgo(m, centerX, centerY, player, center_dist);

	enqueue_entity(&eq, player, 0);

	runGameLoop(&eq, entities, m, num_of_npcs);

	return 0;
}

void runGameLoop(heap_t *eq, entity* entities[], struct Map *m, int num_of_npcs) {

    int current_time = 0;
    entity_move *event;
    bool quit_game = false;
    bool npc_window_open = false;
    char key = 'k';
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);



    //printf("hello I make it inside the game loop\n");

    //Idea for tomorrow: have a battle sequence boolean or int or something and that way if there's a battle sequence going on we can switch from the game state
    //ADD A GAME STATE INTEGER AND A THINGY UP TOP AN ENUM OH YEA
    //ALSO add a boolean for defeated into the entity class

    while (!quit_game) {
        event = dequeue_next(eq);

        //Okay so here's the general idea: I need to have a mvgetch that will be able to receive the various movements. I likely need to relook at the windows and such for curses because this is super weird so far but overall this feels doable.
        //Notably: The game pauses and waits for our input now!
        //The top line needs to be used for displaying messages
        //pressing t will display the list of all entities not including the trainer, if there isn't enough space, then there needs to be scrolling capabilities.
        //Need to add a quit_game bool for ending the game
        if (!event) break;

        current_time = event->next_move;

        int terrain_cost = 10;

        entity *tmp;

        switch (event->npc->id) {

        case PLAYER: {
        	player_time = current_time;
        	bool should_paint = true;
            key = getch();
            switch(key){
                case 'k': // Moving upwards
                    if(is_occupied(player->x, player->y - 1, m, player)){
                        tmp = player_entity_collision(entities, player->x, player->y - 1, num_of_npcs);
                        if(tmp != NULL && !(tmp->isDefeated)){
                            run_battle_sequence();
                            start_battle_state(m, tmp);
                        }
                        break;
                    }
                    if(is_border(player->x, player->y - 1, m)) break;
                    m->arr[player->x][player->y] = player->prev_tile;
                    player->prev_tile = m->arr[player->x][player->y - 1];
                    player->y = player->y - 1;
                    m->arr[player->x][player->y] = player->marker;
                    break;

                case 'j': // Moving downwards
                    if(is_occupied(player->x, player->y + 1, m, player)){
                        tmp = player_entity_collision(entities, player->x, player->y + 1, num_of_npcs);
                        if(tmp != NULL && !(tmp->isDefeated)){
                            run_battle_sequence();
                            start_battle_state(m, tmp);
                        }
                        break;
                    }
                    if(is_border(player->x, player->y + 1, m)) break;
                    m->arr[player->x][player->y] = player->prev_tile;
                    player->prev_tile = m->arr[player->x][player->y + 1];
                    player->y = player->y + 1;
                    m->arr[player->x][player->y] = player->marker;
                    break;

                case 'h': // Moving left
                    if(is_occupied(player->x - 1, player->y, m, player)){
                        tmp = player_entity_collision(entities, player->x - 1, player->y, num_of_npcs);
                        if(tmp != NULL && !(tmp->isDefeated)){
                            run_battle_sequence();
                            start_battle_state(m, tmp);
                        }
                        break;
                    }
                    if(is_border(player->x - 1, player->y, m)) break;
                    m->arr[player->x][player->y] = player->prev_tile;
                    player->prev_tile = m->arr[player->x - 1][player->y];
                    player->x = player->x - 1;
                    m->arr[player->x][player->y] = player->marker;
                    break;

                case 'l': // Moving right
                    if(is_occupied(player->x + 1, player->y, m, player)){
                        tmp = player_entity_collision(entities, player->x + 1, player->y, num_of_npcs);
                        if(tmp != NULL && !(tmp->isDefeated)){
                            run_battle_sequence();
                            start_battle_state(m, tmp);
                        }
                        break;
                    }
                    if(is_border(player->x + 1, player->y, m)) break;
                    m->arr[player->x][player->y] = player->prev_tile;
                    player->prev_tile = m->arr[player->x + 1][player->y];
                    player->x = player->x + 1;
                    m->arr[player->x][player->y] = player->marker;
                    break;

                case 'q':
                    quit_game = true;
                    break;

                case '.':
                	break;

                case 't':
                	toggle_npc_window(entities, num_of_npcs, &npc_window_open);
                	if(!npc_window_open){
                	    paint_board(m);
                	}
                	should_paint = false;
                	enqueue_entity(eq, player, current_time); //I have to skip the enqueue at the bottom otherwise when I press t again it will advance
                	continue;
                    break;

                default:
                    break;
            }
        	//printf("Player pos: x=%d y=%d\n", player.x, player.y);
        	entity* hiker_template = CreateEntity(HIKER, 0, 0);
        	entity* rival_template = CreateEntity(RIVAL, 0, 0);
        	dijkstrasAlgo(m, player->x, player->y, hiker_template, hiker_dist); //POSSIBLE UNCAUGHT ERROR SINCE SWITCHING TO
        	dijkstrasAlgo(m, player->x, player->y, rival_template, rival_dist);
        	dijkstrasAlgo(m, centerX, centerY, player, center_dist);
        	if(should_paint){
        	        paint_board(m);
        	    }
            //paint_board(m);
        	//printf("Center location: %d, %d\n", centerX, centerY);
            //usleep(500000);
            terrain_cost = 10;
            break;
        }
        case HIKER:
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = handle_npc_movement(event->npc, hiker_dist, m, entities);
        	}

//        	if(tmp->isDefeated){
//        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
//        	} else {
//        		terrain_cost = handle_npc_movement(event->npc, hiker_dist, m, entities);
//        	}
            break;

        case RIVAL:
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = handle_npc_movement(event->npc, rival_dist, m, entities);
        	}
            break;

        case PACER:
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = handle_pacer_movement(event->npc, m);
        	}
            break;

        case WANDERER:
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = handle_wanderer_movement(event->npc, m);
        	}
        	break;

        case EXPLORERS: //Oops did not mean to make that plural
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = handle_explorer_movement(event->npc, m);
        	}
        	break;

        case SENTRY:
        	if(event->npc->isDefeated){
        		terrain_cost = handle_npc_movement(event->npc, center_dist, m, entities);
        	} else {
        		terrain_cost = 10;
        	}
            break;

        default:
        	printf("DEFAULT HIT: npc id=%d\n", event->npc->id);
            free(event);
            continue;
        }

        entity *npc = event->npc;
        free(event);

        if(npc->id == PLAYER){
            enqueue_entity(eq, npc, current_time + (uint32_t)terrain_cost - 1);
            continue;
        }

        if(npc->isDefeated && center_dist[npc->x][npc->y] == 0){
            m->arr[npc->x][npc->y] = npc->prev_tile;
            continue;
        }

        if(npc->isDefeated){
            enqueue_entity(eq, npc, player_time + (uint32_t) terrain_cost);
        } else {
            enqueue_entity(eq, npc, current_time + (uint32_t) terrain_cost);
        }
//        if(npc->isDefeated){
//            printf("EXIT CHECK: x=%d y=%d tile=%c center_dist=%d\n",
//                   npc->x, npc->y, m->arr[npc->x][npc->y], center_dist[npc->x][npc->y]);
//            if(center_dist[npc->x][npc->y] == 0){
//                m->arr[npc->x][npc->y] = npc->prev_tile;
//                continue;
//            }
//        }
//        if(npc->isDefeated){
//            printf("EXIT CHECK: x=%d y=%d center_dist=%d\n",
//                   npc->x, npc->y, center_dist[npc->x][npc->y]);
//            if(center_dist[npc->x][npc->y] == 0){
//                m->arr[npc->x][npc->y] = npc->prev_tile;
//                continue;
//            }
//        }
//        if(npc->isDefeated){
//            printf("Defeated NPC at x=%d y=%d, center at x=%d y=%d, center_dist=%d\n",
//                   npc->x, npc->y, centerX, centerY, center_dist[npc->x][npc->y]);
//        }
    }
    endwin();
}

entity* start_battle_state(Map *m, entity *npc){ //Input the player and the npc in question. UPDATE ENTITY TO INCLUDE IS_DEFEATED
	int i,j;
		for(i = 0; i < WORLDX; i++){
			for(j = 0; j < WORLDY; j++){
				mvaddch(j,i, ' ');
				usleep(1000); //just a cool little thingy for now
			}
			refresh();
		}
	bool in_battle = true;
	char battle_debug_screen[] = "Press q to exit battle";
	int k;
	for(k = 0; k < strlen(battle_debug_screen); k++){
		mvaddch(13, k + (WORLDX /2), battle_debug_screen[k]);
		usleep(1000);
	}
	while(in_battle){
	        char key = 'h';
	        key = getch();
	        switch(key){
	        case 'q':
	            npc->isDefeated = true;
	            in_battle = false;
	            // Recompute center_dist immediately using the NPC's current position
	            entity* center_template = CreateEntity(HIKER, 0, 0);
	            dijkstrasAlgo(m, centerX, centerY, center_template, center_dist);
	            break;
	        }
	    }
	    paint_board(m);
	return npc; // perhaps this works?
}

static int is_border(int x, int y, struct Map *m)
{
    /* Map edges are always border */
    if (x <= 0 || x >= 79 || y <= 0 || y >= 20) return 1;
    if (m->arr[x][y] == '%') return 1;
    if(m->arr[x][y] == '^') return 1;
    if(m->arr[x][y] == '~') return 1;
    return 0;
}

//Lowkey I should start keeping a separate array for entities
static entity* player_entity_collision(entity* entities[], int target_x, int target_y, int n) {
    int i;
    for(i = 0; i < n; i++){
        if(entities[i]->x == target_x && entities[i]->y == target_y){
            return entities[i];
        }
    }
    return NULL;
}

static int is_occupied(int x, int y, struct Map *m, entity *self) //This is okay to just return an int since it's only being used for the npcs.
//Maybe in a further refactor, I could put the logic for the check inside of here but also that might bog the game down sinces thats an
//O(n) running everytime anything whatsoever runs into something
{
    char tile = m->arr[x][y];
    /* Player and NPC markers count as occupied */
    if (tile == '@') return 2;
    if (tile == 'h') return 1;
    if (tile == 'r') return 1;
    if (tile == 'p') return 1;
    if (tile == 'w') return 1;
    if (tile == 's') return 1;
    if (tile == 'e') return 1;
    return 0;
}

//static int getPlayerMovement(){ //Eventually I will put all of that logic into this helper function
//
//}

int handle_npc_movement(entity *npc, int dist[80][21], struct Map *m, entity* entities[]) {
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    int best_x = npc->x;
    int best_y = npc->y;
    int bestCost = INT_MAX;



    int i;

//    if(npc->isDefeated){
//            for(i = 0; i < 8; i++){
//                int new_x = npc->x + dx[i];
//                int new_y = npc->y + dy[i];
//                printf("neighbor x=%d y=%d tile=%c dist=%d occupied=%d border=%d\n",
//                       new_x, new_y, m->arr[new_x][new_y],
//                       dist[new_x][new_y],
//                       is_occupied(new_x, new_y, m, npc),
//                       is_border(new_x, new_y, m));
//            }
//        }

    for (i = 0; i < 8; i++) {
        int new_x = npc->x + dx[i];
        int new_y = npc->y + dy[i];
//        if (is_occupied(new_x, new_y, m, npc) == 1 || is_occupied(new_x, new_y, m, npc) == 0){
//        	continue;
//        } else if(is_occupied(new_x, new_y, m, npc) == 2){
//        	run_battle_sequence();
//        }
        if(m->arr[new_x][new_y] == '@') {
            if(!npc->isDefeated) {
                run_battle_sequence();
                start_battle_state(m, npc);
                return getTerrainCost(m->arr[npc->x][npc->y], npc);
            }
            continue; //skip the player tile and keep scanning
        }
        if(is_occupied(new_x, new_y, m, npc)) continue;
        if (is_border(new_x, new_y, m)) continue;
        if (dist[new_x][new_y] == INT_MAX) continue;

        if (dist[new_x][new_y] < bestCost) {
            bestCost = dist[new_x][new_y];
            best_x   = new_x;
            best_y   = new_y;
        }
    }

    if (best_x == npc->x && best_y == npc->y) {
        return getTerrainCost(m->arr[npc->x][npc->y], npc);
    }

    m->arr[npc->x][npc->y] = npc->prev_tile;
    npc->prev_tile          = m->arr[best_x][best_y];
    npc->x = best_x;
    npc->y = best_y;
    m->arr[npc->x][npc->y] = npc->marker;

    return getTerrainCost(npc->prev_tile, npc);
}

int handle_wanderer_movement(entity *npc, struct Map *m) {
    int dx[8] = { 0,  0,  1, -1,  1, -1,  1, -1};
    int dy[8] = {-1,  1,  0,  0, -1,  1,  1, -1};

    char home_terrain = npc->prev_tile;

    int nx = npc->x + dx[npc->direction];
    int ny = npc->y + dy[npc->direction];

    //try current direction first
    if (nx >= 0 && nx < 80 && ny >= 0 && ny < 21
        && !is_border(nx, ny, m)
        && !is_occupied(nx, ny, m, npc)
        && m->arr[nx][ny] == home_terrain) {

        m->arr[npc->x][npc->y] = npc->prev_tile;
        npc->x = nx;
        npc->y = ny;
        m->arr[npc->x][npc->y] = npc->marker;
        return getTerrainCost(npc->prev_tile, npc);
    }

    int i;
    for (i = 0; i < 8; i++) {
        if (i == npc->direction) continue;

        nx = npc->x + dx[i];
        ny = npc->y + dy[i];

        if (nx >= 0 && nx < 80 && ny >= 0 && ny < 21
            && !is_border(nx, ny, m)
            && !is_occupied(nx, ny, m, npc)
            && m->arr[nx][ny] == home_terrain) {

            npc->direction = i; //new dir
            m->arr[npc->x][npc->y] = npc->prev_tile;
            npc->x = nx;
            npc->y = ny;
            m->arr[npc->x][npc->y] = npc->marker;
            return getTerrainCost(npc->prev_tile, npc);
        }
    }

    return getTerrainCost(m->arr[npc->x][npc->y], npc);
}

int handle_pacer_movement(entity *npc, struct Map *m)
{

	int dx[8] = { 0,  0,  1, -1,  1, -1,  1, -1};
	int dy[8] = {-1,  1,  0,  0, -1,  1,  1, -1};

    /* Try current direction first, then the reverse */
    int dirs_to_try[2];
    dirs_to_try[0] = npc->direction;
    dirs_to_try[1] = npc->direction ^ 1;   /* opposite direction */

    char start_terrain = npc->prev_tile;     /* terrain pacer was spawned on */

    int attempt;
    for (attempt = 0; attempt < 2; attempt++) {
        int dir = dirs_to_try[attempt];
        int nx  = npc->x + dx[dir];
        int ny  = npc->y + dy[dir];

        if (nx < 0 || nx >= 80 || ny < 0 || ny >= 21) continue;

        if (is_border(nx, ny, m)) continue;

        if (is_occupied(nx, ny, m, npc)) continue;

        if (m->arr[nx][ny] != start_terrain) continue;

        /*update direction if we had to reverse */
        npc->direction = dir;

        m->arr[npc->x][npc->y] = npc->prev_tile;
        npc->x = nx;
        npc->y = ny;
        m->arr[npc->x][npc->y] = npc->marker;

        return getTerrainCost(npc->prev_tile, npc);
    }

    return getTerrainCost(m->arr[npc->x][npc->y], npc);
}

int handle_explorer_movement(entity *npc, struct Map *m) {
    int dx[8] = { 0,  0,  1, -1,  1, -1,  1, -1};
    int dy[8] = {-1,  1,  0,  0, -1,  1,  1, -1};

    int nx = npc->x + dx[npc->direction];
    int ny = npc->y + dy[npc->direction];

    if (nx >= 0 && nx < 80 && ny >= 0 && ny < 21
        && !is_border(nx, ny, m)
        && !is_occupied(nx, ny, m, npc)
        && getTerrainCost(m->arr[nx][ny], npc) != INT_MAX) {

        m->arr[npc->x][npc->y] = npc->prev_tile;
        npc->prev_tile          = m->arr[nx][ny];
        npc->x = nx;
        npc->y = ny;
        m->arr[npc->x][npc->y] = npc->marker;
        return getTerrainCost(npc->prev_tile, npc);
    }

    int i;
    for (i = 0; i < 8; i++) {
        if (i == npc->direction) continue;

        nx = npc->x + dx[i];
        ny = npc->y + dy[i];

        if (nx >= 0 && nx < 80 && ny >= 0 && ny < 21
            && !is_border(nx, ny, m)
            && !is_occupied(nx, ny, m, npc)
            && getTerrainCost(m->arr[nx][ny], npc) != INT_MAX) {

            npc->direction = i;
            m->arr[npc->x][npc->y] = npc->prev_tile;
            npc->prev_tile          = m->arr[nx][ny];
            npc->x = nx;
            npc->y = ny;
            m->arr[npc->x][npc->y] = npc->marker;
            return getTerrainCost(npc->prev_tile, npc);
        }
    }

    return getTerrainCost(m->arr[npc->x][npc->y], npc);
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

	//printf("north path: %d || south path: %d\n", current_map->entrances[North], current_map->entrances[South]);

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

	//printf("west path: %d || east path: %d\n", current_map->entrances[West], current_map->entrances[East]);

	return 0;
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

int pepperInTrees(struct Map *m){
	int count = 0;
	int rand_x, rand_y;
	while(count < 30){
		rand_x = rand() %79 + 1;
		rand_y = rand() % 19 + 1;
		if(m->arr[rand_x][rand_y] == '.' || m->arr[rand_x][rand_y] == ':'){
			m->arr[rand_x][rand_y] = '^';
		}
		count++;
	}
	return 0;
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

int check_if_spawns_on(char tile, char spawnables[4]){
	int i;
	printf("debug: ");
	for(i = 0; i < 4; i++){
		printf("%c, ", spawnables[i]);
		if(tile == spawnables[i]){
			return 1;
		}
		printf("\n");
	}
	return 0;
}

int spawnEntities(heap_t *eq, entity* entities[], int rand_num, struct Map *m){
	int i;
	printf("Hello I make it inside spawnEntities\n");
//	entity *explorer = malloc(sizeof(entity)); //For testing purposes
//	explorer = CreateEntity(EXPLORERS, 0, 0);
//	spawnEntity(explorer, EXPLORERS, m);
//	enqueue_entity(eq, explorer, 0);
	    for (i = 0; i < rand_num; i++) {
	    	//printf("I make it inside of here\n");
	    	entity *npc;
	        npc  = malloc(sizeof(entity));
	        int rand_entity = rand() % 6 + 1;
	        npc = CreateEntity(rand_entity, 0, 0);
	        printf("npc isSpawned = %b\n", npc->isSpawned);
	        //printf("Entity is created\n");
	        spawnEntity(npc, rand_entity, m); //NEED TO FIX
	        printf("Entity is initialized\n");
	        entities[i] = npc;
	        printf("Entity is in array\n");
	        enqueue_entity(eq, npc, 0);
	        printf("Enitity spawned: %d", npc->id);
	    }
	    return 0;
	}


//This is a not-dynamic spawning of entities on the world
/*
 * I think I could improve this algorithm by making it pass in a queue of entities/d_array
 * Currently, it requires for there to be a special magical awesome number of 5 entities for it to work.
 */
int spawnEntity(entity *npc, int id, struct Map *m){
	//Spawn stuff and things
	int rand_x;
	int rand_y;
		//(*npc) = CreateEntity(id, rand_x, rand_y); //Actual npc creation with garbage values set
		//printf("hello I am spawning: %c\n", entities[0].marker);
		while(!npc->isSpawned){
				rand_x = rand() % 78 + 1;
				rand_y = rand() % 19 + 1;
				if(check_if_spawns_on(m->arr[rand_x][rand_y], npc->spawnsOn)){ //So npc is full of garbage values rn
					//printf("Okay I'm getting put onto something\n");
					npc->x = rand_x;
					npc->y = rand_y;
					npc->prev_tile = m->arr[rand_x][rand_y]; /* save terrain BEFORE overwriting */
					npc->direction = rand() % 8;
					printf("Here is what's spawning: %c\n", npc->marker);
					npc->isSpawned = true;

				} else {
					//printf("I'm continuing\n");
				    continue;
				}
			}
	return 0;
}

int32_t cell_compare(const void *key, const void *with) {
    return ((map_cell_t *)key)->cost - ((map_cell_t *)with)->cost;
}

int dijkstrasAlgo(struct Map *m, int x, int y, entity *npc, int dist[80][21]){
    heap_t h;
    heap_node_t *nodes[80][21];
    map_cell_t  *cells[80][21];

    int da_x, da_y;
    for (da_x = 0; da_x < 80; da_x++) {
        for (da_y = 0; da_y < 21; da_y++) {
            dist[da_x][da_y] = INT_MAX;
            nodes[da_x][da_y] = NULL;
            cells[da_x][da_y] = NULL;
        }
    }

    heap_init(&h, cell_compare, free);

    map_cell_t *start = malloc(sizeof(map_cell_t)); //Evil segmentation arror...
    start->x = x;
    start->y = y;
    start->cost = 0;
    dist[x][y] = 0;
    cells[x][y] = start;
    nodes[x][y] = heap_insert(&h, start); //Inserting the initial heap node

    //Same thing as what I used for knights tour
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

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
            	//printf("I'm overriding the pos");
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
	return INT_MAX;
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

int paint_board(struct Map *m){ //Add message char* parameter that displays on the first line
	int i,j;
	for(i = 0; i < WORLDX; i++){
		for(j = 0; j < WORLDY; j++){
			mvaddch(j, i, m->arr[i][j]);
		}
	}
	refresh();
	return 0;
}

int run_battle_sequence(){
	int i,j;
	for(i = 0; i < WORLDX; i++){
		for(j = 0; j < WORLDY; j++){
			mvaddch(j,i, '/');
			usleep(1000); //just a cool little thingy for now
		}
		refresh();
	}
	return 0;
}

void toggle_npc_window(entity* entities[], int num_of_npcs, bool *window_open){
    if(*window_open){
        *window_open = false;
        return;
    }

    *window_open = true;

    int win_h = 15;
    int win_w = 40;
    int win_y = (WORLDY / 2) - (win_h / 2);
    int win_x = (WORLDX / 2) - (win_w / 2);
    int list_h = win_h - 2;

    WINDOW *npc_win = newwin(win_h, win_w, win_y, win_x); //Lowkey the hardest part for me
    int i;
    for(i = 0; i < win_w; i++){
        mvwaddch(npc_win, 0, i, '-');
        mvwaddch(npc_win, win_h - 1, i, '-');
    }
    for(i = 0; i < win_h; i++){
        mvwaddch(npc_win, i, 0, '|');
        mvwaddch(npc_win, i, win_w - 1, '|');
    }

    char *title = "[ NPC LIST ]";
    mvwprintw(npc_win, 0, (win_w / 2) - (strlen(title) / 2), "%s", title);
    mvwprintw(npc_win, win_h - 1, 2, "j/k to scroll, t to close");

    wrefresh(npc_win);

    //stolen from stackoverflow
    WINDOW *pad = newpad(num_of_npcs + 1, win_w - 2);

    //int i;
    for(i = 0; i < num_of_npcs; i++){
        char *type;
        switch(entities[i]->id){
            case HIKER:     type = "Hiker";    break;
            case RIVAL:     type = "Rival";    break;
            case PACER:     type = "Pacer";    break;
            case WANDERER:  type = "Wanderer"; break;
            case SENTRY:    type = "Sentry";   break;
            case EXPLORERS: type = "Explorer"; break;
            default:        type = "Unknown";  break;
        }
        char *status = entities[i]->isDefeated ? "Defeated" : "Active";
        mvwprintw(pad, i, 0, "%-10s x=%-3d y=%-3d %s",
                  type, entities[i]->x, entities[i]->y, status);
    }

    int scroll = 0;
    int max_scroll = num_of_npcs - list_h;
    if(max_scroll < 0) max_scroll = 0;

    char nav_key;
    bool browsing = true;
    while(browsing){
        //Render the visible portion of the pad onto the screen
        prefresh(pad, scroll, 0,
                 win_y + 1, win_x + 1,
                 win_y + win_h - 2, win_x + win_w - 2);

        nav_key = getch(); //Similar to the one in my player
        switch(nav_key){
            case 'k':
                if(scroll > 0) scroll--;
                break;
            case 'j':
                if(scroll < max_scroll) scroll++;
                break;
            case 't':
                browsing = false;
                *window_open = false;
                break;
            default:
                break;
        }
    }

    delwin(pad);
    delwin(npc_win);
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
