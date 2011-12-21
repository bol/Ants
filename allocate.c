#include <limits.h>
#include <stdlib.h>

#include "ants.h"

void allocate_map() {
	int r, c;

	Log( "Initializing map\n");
	Game->map = calloc(Info->rows, sizeof(struct node *));

	for (r=0; r < Info->rows;r++) {
		Game->map[r] = calloc(Info->cols, sizeof(struct node));
	}

	for (r=0; r < Info->rows;r++) {
		for (c=0; c < Info->cols;c++) {
				Game->map[r][c].type = '.';
				Game->map[r][c].col = c;
				Game->map[r][c].row = r;
				Game->map[r][c].lastseen = INT_MIN;;

				if (r == 0) {
					Game->map[r][c].dir[DIR_NORTH] = &Game->map[Info->rows - 1][c];
				} else {
					Game->map[r][c].dir[DIR_NORTH] = &Game->map[r-1][c];
				}

				if (c == Info->cols -1 ) {
					Game->map[r][c].dir[DIR_EAST] = &Game->map[r][0];
				} else {
					Game->map[r][c].dir[DIR_EAST] = &Game->map[r][c+1];
				}

				if (r == Info->rows -1 ) {
					Game->map[r][c].dir[DIR_SOUTH] = &Game->map[0][c];
				} else {
					Game->map[r][c].dir[DIR_SOUTH] = &Game->map[r+1][c];
				}

				if (c == 0) {
					Game->map[r][c].dir[DIR_WEST] = &Game->map[r][Info->cols - 1];
				} else {
					Game->map[r][c].dir[DIR_WEST] = &Game->map[r][c-1];
				}
		}
	}

	/* Allocate the color map as well */
	Game->colormap = malloc(Info->rows *Info->cols * sizeof(unsigned int));
}

void allocate_lists(){
		Log( "Initializing lists\n");

		/* Can't be more than one any per node on the map */
		Game->enemy_ants = calloc(Info->cols * Info->rows, sizeof(struct basic_ant));
		Game->my_ants = calloc(Info->cols * Info->rows, sizeof(struct my_ant));
		Game->food = calloc(Info->cols * Info->rows, sizeof(struct food));
		Game->hills = calloc(Info->cols * Info->rows, sizeof(struct food));
		Game->dead_ants = calloc(Info->cols * Info->rows, sizeof(struct basic_ant));

		Game->targets = NULL;
		Game->target_max = 0;
}
