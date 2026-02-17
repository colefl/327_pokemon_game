/*
 * Maps.c
 *
 *  Created on: Feb 8, 2026
 *      Author: colef
 */

#include "world_gen.c"

typedef struct Maps{
	struct Map* m[401][401];
} Maps;

int checkEntrances(Maps *maps, int x, int y);

int initialize_all_maps(Maps *maps){
    int i, j;
    for(i = 0; i < 401; i++){
        for(j = 0; j < 401; j++){
            maps->m[i][j] = NULL;
        }
    }
    return 0;
}

int initialize_maps(struct Maps *maps, int x, int y){
	struct Map *tmp;
	if(!(tmp = malloc(sizeof (*tmp)))){ //Even if we change the type of tmp or the data struct this would still work
		//malloc failed
		return -1;
	}

	int i,j;

	//I kept getting garbage values so I ended up just initializing these arrays as null prior to
	for(i = 0; i < WORLDX; i++){
		for(j = 0; j < WORLDY; j++){
			tmp->arr[i][j] = ' ';
			tmp->visited[i][j] = false;
		}
	}

	for(i = 0; i < 4; i++){
		tmp->entrances[i] = 0;
		tmp->omitDir[i] = false;
	}

	init_map(tmp);
	maps->m[x][y] = tmp;
	//print_board(maps->m[x][y]);
	print_board(maps->m[x][y]);
	printf("Currently at: %d, %d\n", x - 200, y - 200);
	//printf("I think this is the west entrance: %d \n", tmp->entrances[0]); //debugging
	return 0;
}

int add_maps(Maps *maps, int x, int y){
	if(maps->m[x][y] == NULL){ //Here I check to see if the map is a null pointer
		struct Map *tmp;
		if(!(tmp = malloc(sizeof (*tmp)))){ //Even if we change the type of tmp or the data struct this would still work
			//malloc failed
			return -1;
		}

		int i, j;

		//I kept getting garbage values so I ended up just initializing these arrays as null prior to
		for(i = 0; i < WORLDX; i++){
			for(j = 0; j < WORLDY; j++){
				tmp->arr[i][j] = ' ';
				tmp->visited[i][j] = false;
			}
		}

		for(i = 0; i < 4; i++){
			tmp->entrances[i] = 0;
			tmp->omitDir[i] = false;
		}

		maps->m[x][y] = tmp; //Set m to point to the new tmp map.
		checkEntrances(maps, x, y);
		init_map(tmp); //Initialize the map if there's space on the stack
		print_board(maps->m[x][y]);
		//printf("I think this is the west entrance: %d \n", tmp->entrances[1]); //Idk basic check I guess
		printf("This is the x and y literal: %d, %d\n", x - 200, y - 200);

		return 0;


	} else {
		print_board(maps->m[x][y]);
		//printf("I think this is the west entrance: %d \n", maps->m[x][y]->entrances[1]); //Idk basic check I guess to see if the map data is saved.
		printf("This is the x and y literal: %d, %d\n", x - 200, y - 200);
		return 0;
	}

}

/*
 * Ref sheet for entrances array:
 * North: 0
 * West: 1
 * South: 2
 * East: 3
 */
int checkEntrances(Maps *maps, int x, int y){

	//Northwest corner
	if(x == 0 && y == 400){
		maps->m[x][y]->entrances[North] = OutOfBounds;
		maps->m[x][y]->entrances[West] = OutOfBounds;
		printf("We've hit the northwest corner\n");
	}
	//Northeast corner
	else if(x == 400 && y == 400){
		maps->m[x][y]->entrances[North] = OutOfBounds;
		maps->m[x][y]->entrances[East] = OutOfBounds;
		printf("We've hit the northeast corner\n");
	}
	//Southwest corner
	else if(x == 0 && y == 0){
		maps->m[x][y]->entrances[South] = OutOfBounds;
		maps->m[x][y]->entrances[West] = OutOfBounds;
		printf("We've hit the southwest corner\n");
	}
	//Southeast corner
	else if(x == 400 && y == 0){
		maps->m[x][y]->entrances[South] = OutOfBounds;
		maps->m[x][y]->entrances[East] = OutOfBounds;
		printf("We've hit the southeast corner\n");
	}

	//East check
	if(x + 1 <= 400 && maps->m[x + 1][y] != NULL){
		maps->m[x][y]->entrances[East] = maps->m[x+1][y]->entrances[West];
		printf("Matched east entrance to existing map: %d\n", maps->m[x][y]->entrances[East]);
	} else if(x >= 400){
		maps->m[x][y]->entrances[East] = OutOfBounds;
		printf("We've hit the far right edge\n");
	}

	//West check
	if(x - 1 >= 0 && maps->m[x-1][y] != NULL){
		maps->m[x][y]->entrances[West] = maps->m[x-1][y]->entrances[East];
		printf("Matched west entrance to existing map: %d\n", maps->m[x][y]->entrances[West]);
	} else if(x <= 0){
		maps->m[x][y]->entrances[West] = OutOfBounds;
		printf("We've hit the far left edge\n");
	}

	//North check
	if(y + 1 <= 400 && maps->m[x][y+1] != NULL){
		maps->m[x][y]->entrances[North] = maps->m[x][y+1]->entrances[South];
		printf("Matched north entrance to existing map: %d\n", maps->m[x][y]->entrances[North]);
	} else if(y >= 400){
		maps->m[x][y]->entrances[North] = OutOfBounds;
		printf("We've hit the top edge\n");
	}

	//South check
	if(y - 1 >= 0 && maps->m[x][y-1] != NULL){
		maps->m[x][y]->entrances[South] = maps->m[x][y-1]->entrances[North];
		printf("Matched south entrance to existing map: %d\n", maps->m[x][y]->entrances[South]);
	} else if(y <= 0){
		maps->m[x][y]->entrances[South] = OutOfBounds;
		printf("We've hit the bottom edge\n");
	}

	return 0;
}

int cleanup(Maps *maps){
	int i, j;
	for(i = 0; i < 401; i++){
		for(j = 0; j < 401; j++){
			if(maps->m[i][j] == NULL){
				continue;
			} else {
				free(maps->m[i][j]);
				maps->m[i][j] = NULL;
				//printf("Hey Alexa play freedom by George Michael\n");
			}
		}
	}
	return 0;
}
