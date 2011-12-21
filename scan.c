#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "ants.h"

void backtrack(int, struct node *, unsigned int);

extern inline int offset(struct node *);
extern inline unsigned char reverse_direction(unsigned char);

/* Calculate offset for the colormap */
inline int offset(struct node *node)
{
	return ((node->row * Info->cols) + node->col);
}

/* Reverse a direction */
inline unsigned char reverse_direction(unsigned char d) {
				switch (d) {
				case DIR_NORTH:
					d = DIR_SOUTH;
					break;
				case DIR_SOUTH:
					d = DIR_NORTH;
					break;
				case DIR_EAST:
					d = DIR_WEST;
					break;
				case DIR_WEST:
					d = DIR_EAST;
					break;
				default:
					d = 100;
					break;
				}

				return d;
}

void scan(int antn)
{
	/* two buffers for storing nodes we need to check */
	struct node *buffer1[Info->rows * Info->cols];
	struct node *buffer2[Info->rows * Info->cols];
	/* pointers for the buffers */
	struct node **writeq;
	struct node **readq;
	struct node **tmpq;
	/* counters for the buffers */
	int read, write;
	/* distance from our ant */
	unsigned int distance;
	/* the nodes we're currently examining */
	struct node *node, *next_node;
	/* Check to find the first fow target */
	int fow_target = 0;
	int max_fow_targets = 20;
	unsigned char dir;
	/* Empty the color data */
	memset(Game->colormap, INT_MAX,
	       (Info->rows * Info->cols * sizeof(unsigned int)));

	/* Initialize the queue pointers */
	readq = buffer1;
	writeq = buffer2;

	/* Initialize the read queue with our ants location */
	readq[0] = &Game->map[Game->my_ants[antn].row][Game->my_ants[antn].col];
	read = 1;

	Game->colormap[offset(readq[0])] = 0;

	/* Search the map */
	for (distance = 1;
					distance < INT_MAX;
	     distance++) {
		write = 0;

		while (read) {
			node = readq[--read];

			/* Okay, we got a node. Lets check the neighbours */
			for (dir = 0; dir < 4; dir++) {
				next_node = node->dir[dir];

				/* Have we been here before? */
				if (Game->colormap[offset(next_node)] <
				    INT_MAX) {
					continue;
				}

				/* Is it a wall? */
				if (next_node->type == TYPE_WATER) {
					continue;
				}

				Game->colormap[offset(next_node)] = distance;

				if (next_node->type == TYPE_FOOD) {
					/* handle food */
					backtrack(antn, next_node, distance );
				} else if (next_node->type == TYPE_ENEMYHILL) {
					/* attack enemy bases */
					backtrack(antn, next_node, distance );
				} else if ((next_node->type == TYPE_FOW) && (fow_target < max_fow_targets)) {
					/* explore */
					fow_target++;
					backtrack(antn, next_node, distance );
				} 

				/* Write the distance from our source node and add it to the list of nodes we'll check on our next pass */
				writeq[write++] = next_node;
			}

		}

		/* We didn't add a single node to the write queue last run 
		   No point in continueing */
		if (write == 0) {
			break;
		}

		/* Switch the buffers and repeat */
		read = write;
		tmpq = readq;
		readq = writeq;
		writeq = tmpq;;
	}
}

void backtrack(int antn, struct node *from, unsigned int distance)
{
	unsigned char * path;
	unsigned int min_distance;

	struct node *next_node, *node;
	struct target * target;

	unsigned char i, d, direction;

	node = from;

	target = add_target(antn, from, distance);

	for (i = 0; i < distance; i++) {
		min_distance = INT_MAX;

		for (d = 0; d < 4; d++) {
			next_node = node->dir[d];

			/* Are we there yet? */
			if ((next_node->row == Game->my_ants[antn].row)
			    && (next_node->col == Game->my_ants[antn].col)) {

				d = reverse_direction(d);
				target->path = d;

				return;
			}

			/* Find the shortest path */
			if (min_distance > Game->colormap[offset(next_node)]) {
				min_distance = Game->colormap[offset(next_node)];
				direction = d;
			}
		}

		/* Take the next node */
		node = node->dir[direction];
	}

	Log( "Warning: Didn't find my way back.\n");
}
