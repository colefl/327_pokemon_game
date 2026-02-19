/*
 * main.c
 *
 *  Created on: Feb 7, 2026
 *      Author: colef
 */

#include "Maps.c"
#include <stdio.h>
#include <stdbool.h>

struct Map m;

Maps maps;

/*
 * Idea Board: So, for assignment 4, I need to add in a lot of different entity types which move in many different ways
 * I want to make sure spawning is okay so I'm thinking I may try to learn how to implement a hashmap in order to get the
 * contains as fast as possible.
 */

/*
 * For creating paths that connect depending on where you are in the world
 * we would need to check adjacent maps. If not null, based on where map is
 * populate the current map entrances with the other maps direction.
 */


int main(int argc, char *argv[]){
	bool isRunning = true;
	char input;
	int default_x = 200;
	int default_y = 200;
	//Need to create a character array, print on it, and then reset input to null.
	//Past this I need to figure initializing and mallocing the maps(?)
	//Likely will create a new file for the larger scale maps.

	initialize_all_maps(&maps);
	initialize_maps(&maps, default_x, default_y);
	while(isRunning){
		printf("Please choose a direction(n,s,e,w), to fly(f), or to quit(q): ");
		scanf("\r %c", &input);
		switch(input){
		case 'q':
			isRunning = false;
			cleanup(&maps);
			printf("\r quitting\n");
			fflush(stdout);
			break;

		case 'n':
			if(default_y++ < 400){
				//printf("%d",default_y);
				printf("\r north\n");
				add_maps(&maps, default_x, default_y);
			} else {
				printf("You've hit the northern world edge!\n");
				default_y--;
			}
			fflush(stdout);
			break;

		case 'e':
			if(default_x++ < 400){
				printf("\r east\n");
				add_maps(&maps, default_x, default_y);
			} else {
				printf("You've hit the eastern world edge!\n");
				default_x--;
			}
			fflush(stdout);
			break;

		case 's':
			if(default_y-- > 0){
				printf("\r south\n");
				add_maps(&maps, default_x, default_y);
			} else {
				printf("You've hit the southern world edge!\n");
				default_y++;
			}
			fflush(stdout);
			break;

		case 'w':
			if(default_x-- > 0){
				printf("\r west\n");
				add_maps(&maps, default_x, default_y);
			} else {
				printf("You've hit the western world edge!\n");
				default_x++;
			}
			fflush(stdout);
			break;

		case 'f':
			printf("Please choose integer coordinates you'd like to fly to here: \nx: ");
			scanf("%d", &default_x);
			printf("\ny: ");
			scanf("%d", &default_y);
			printf("\n");
			//default_x = exnwhy[0];
			//default_y = exnwhy[1];
			printf("default_x value: %d\n", default_x);
			printf("default_y value: %d\n", default_y);
			if((default_x + 200 > 401 || default_y + 200 > 401) || (default_x + 200 < 0 || default_y + 200 < 0)){
				printf("Please input a value within the ranges\n");
				while ((input = getchar()) != '\n' && input != EOF);
				break;
			}
			default_x += 200;
			default_y += 200;
			add_maps(&maps, default_x, default_y);
			break;

		default:
			printf("Wrong input!\n");
			while ((input = getchar()) != '\n' && input != EOF); //clears the buffer from bad inputs
			fflush(stdout);
			break;
		}
	}
}
