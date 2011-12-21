#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ants.h"

// initializes the game_info structure on the very first turn
// function is not called after the game has started

void _init_ants(char *data) {
    char *replace_data = data;

    while (*replace_data != '\0') {
        if (*replace_data == '\n')
            *replace_data = '\0';
        ++replace_data;
    }

    while (1) {
        char *value = data;

        while (*++value != ' ');
        ++value;

        int num_value = atoi(value);

	    switch (*data) {
            case 'l':
                Info->loadtime = num_value;
                break;

            case 't':
                if (*(data + 4) == 't')
                    Info->turntime = num_value;
                else
                    Info->turns = num_value;
                break;

            case 'r':
                Info->rows = num_value;
                break;

            case 'c':
                Info->cols = num_value;
                break;
                        
            case 'v':
                Info->viewradius_sq = num_value;
                break;

            case 'a':
                Info->attackradius_sq = num_value;
                break;
                    
            case 's':
                if (*(data + 1) == 'p')
                    Info->spawnradius_sq = num_value;
                else
                    Info->seed = num_value;
                break;

        }

        data = value;
        
        while (*++data != '\0');
        ++data;
        
        if (strcmp(data, "ready") == 0)
            break;
    }

	/* Save a dangerradius that is one greater than the attack radius since enemy ants moves as well */
    float f = sqrtf(Info->attackradius_sq);
    f += 1;
    f = powf(f, 2);
    Info->dangerradius_sq = ceilf(f);

    Log("Attackradius_sq  %d\n", Info->attackradius_sq);
    Log("Dangerradius_sq  %d\n", Info->dangerradius_sq);
}

void init_lists() {
		Game->my_count_last = Game->my_count;
		Game->my_count = 0;
		Game->enemy_count = 0;
		Game->food_count = 0;
		Game->dead_count = 0;
		Game->hill_count = 0;
		Game->target_count = 0;
}

int add_own_ant(int row, int col) {
		int antn;

		antn = Game->my_count++;

		Game->my_ants[antn].id = antn;
		Game->my_ants[antn].row = row;
		Game->my_ants[antn].col = col;

		Game->my_ants[antn].targets = NULL;

		return antn;
}

int add_enemy_ant(int row, int col, char player) {
		int antn;

		antn = Game->enemy_count++;

		Game->enemy_ants[antn].player = player;
		Game->enemy_ants[antn].row = row;
		Game->enemy_ants[antn].col = col;

		return antn;
}

int add_hill(int row, int col, char player) {
		int hilln;

		hilln = Game->hill_count++;

		Game->hills[hilln].player = player;
		Game->hills[hilln].row = row;
		Game->hills[hilln].col = col;

		return hilln;
}

int add_food(int row, int col) {
		int foodn;

		foodn = Game->food_count++;

		Game->food[foodn].row = row;
		Game->food[foodn].col = col;

		return foodn;
}

void _init_map(char *data) {

    while (*data != 0) {
        char *tmp_data = data;
        int arg = 0;

        while (*tmp_data != '\n') {
            if (*tmp_data == ' ') {
                *tmp_data = '\0';
                ++arg;
            }

            ++tmp_data;
        }

        char *tmp_ptr = tmp_data;
        tmp_data = data;

        tmp_data += 2;
        int jump = strlen(tmp_data) + 1;

        int row = atoi(tmp_data);
        int col = atoi(tmp_data + jump);
        char var3 = -1;

        if (arg > 2) {
            jump += strlen(tmp_data + jump) + 1;
            var3 = *(tmp_data + jump);
        }

        switch (*data) {
            case 'w':
                Game->map[row][col].type = TYPE_WATER;
                break;
            case 'a':
				if(atoi(&var3) == 0) {
					add_own_ant(row, col);
					Log( "Friendly ant at %d:%d\n", row, col);
				} else {
					Log( "Enemy ant from player %c at %d:%d\n", var3, row, col);
					add_enemy_ant(row, col, var3);
				}
                break;
            case 'h':
				if(atoi(&var3) == 0) {
					Log( "Friendly hill at %d:%d\n", row, col);
                	Game->map[row][col].type = TYPE_FRIENDLYHILL;
				} else {
					Log( "Enemy hill from player %c at %d:%d\n", var3, row, col);
                	Game->map[row][col].type = TYPE_ENEMYHILL;
				}
				add_hill(row, col, var3);
                break;
            case 'd':
					Log( "Corpse from player %c at %d:%d\n", var3, row, col);
                break;
            case 'f':
				add_food(row, col);
				Log( "Food at %d:%d\n", row, col);
                break;
			case 'g':
				break;
			default:
				Log("Read strange input data: %s %d %d\n", data, row, col);
				break;
        }

		data = tmp_ptr + 1;
	}
}

/*
 * Unsofisticated check to see if our population is in decline
 */
int check_decline() {
		if (Game->my_count < 10) {
				return 1;
		}
		if (Game->my_count_last > Game->my_count) {
				return 1;
		}

		return 0;

}

void update_map() {
		int c, r;
		int antn;
		int foodn;
		int hilln;

		/* Reset everything but water tiles and enemy ant hills */
		for(r=0; r < Info->rows; r++) {
			for(c=0; c < Info->cols; c++) {
					int seen=0;

					/* Skip water nodes */
					if ( Game->map[r][c].type == TYPE_WATER ) {
							continue;
					}

					/* Clear the target lists from last turn if we have them */
					if ( Game->map[r][c].targets != NULL ) {
						clear_targetlist(Game->map[r][c].targets);
						Game->map[r][c].targets = NULL;
					}

					/* Note the danger rating of the node */
					Game->map[r][c].danger = 0;
					for (antn=0; antn < Game->enemy_count; antn++) {
						if (getdistance(Game->enemy_ants[antn].row, Game->enemy_ants[antn].col, r, c) <= Info->dangerradius_sq) {
							Game->map[r][c].danger++;
						}
					}

					/* Overwrite everything we see as we'll have list info on them */
					/* Also make a note of which turn the node wasn't covered by fow */
					for (antn=0; antn < Game->my_count; antn++) {
						if (getdistance(Game->my_ants[antn].row, Game->my_ants[antn].col, r, c) <= Info->viewradius_sq) {
							Game->map[r][c].lastseen = Game->turn;
							Game->map[r][c].type = TYPE_GROUND;
							seen = 1;
							break;
						}
					}
					/* Not in fow */
					if (seen == 1) {
						continue;
					}

					/* Don't overwrite enemy hill locations when they're out in the fow */
					if ( Game->map[r][c].type == TYPE_ENEMYHILL ) {
							continue;
					}
					/* Same for food */
					if ( Game->map[r][c].type == TYPE_FOOD ) {
							continue;
					}

					Game->map[r][c].type = TYPE_FOW;

			}
		}

		/* Mark our ants */
		for (antn=0; antn < Game->my_count; antn++) {
				Game->map[Game->my_ants[antn].row][Game->my_ants[antn].col].type = TYPE_ANT;
		}

		/* Mark food */
		for(foodn=0; foodn < Game->food_count; foodn++) {
				Game->map[Game->food[foodn].row][Game->food[foodn].col].type = TYPE_FOOD;
		}

		/* Mark hills */
		for(hilln=0; hilln < Game->hill_count; hilln++) {
				if(Game->hills[hilln].player == '0') {
					Game->map[Game->hills[hilln].row][Game->hills[hilln].col].type = TYPE_FRIENDLYHILL;
					continue;
				}
				Game->map[Game->hills[hilln].row][Game->hills[hilln].col].type = TYPE_ENEMYHILL;
		}

}
