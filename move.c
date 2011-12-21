
#include "ants.h"

int do_move(int antn) {
	struct my_ant * ant;
	struct node * node;

	ant = &Game->my_ants[antn];
	if (ant->move == DIR_ILLEGAL) {
		return 0;
	}
	node = &Game->map[ant->row][ant->col];

	if (node->dir[ant->move]->type == TYPE_ANT) {
		return 1;
	} 

	node->dir[ant->move]->type = TYPE_ANT;
	node->type = TYPE_GROUND;

	move(antn, ant->move);
	ant->move = DIR_ILLEGAL;
	return 0;
}

// sends a move to the tournament engine and keeps track of ants new location
void move(int antn, int dir) {
	char dir_c;
	struct my_ant * ant;
	struct node * node;

	ant = &Game->my_ants[antn];
	node =  &Game->map[ant->row][ant->col];
	node->type = '.';

    switch (dir) {
        case DIR_NORTH:
			dir_c = 'N';
			break;
        case DIR_EAST:
			dir_c = 'E';
			break;
        case DIR_SOUTH:
			dir_c = 'S';
			break;
        case DIR_WEST:
			dir_c = 'W';
			break;
		default:
    		Log( "Trying to move ant %i:%i in non existing direction %d\n", ant->row, ant->col, dir);
			return;
	}

    fprintf(stdout, "O %i %i %c\n", ant->row, ant->col, dir_c);

	node->dir[dir]->type = 'a';
	node->dir[dir]->willbemovedto = Game->turn;
}

