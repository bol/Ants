#include <limits.h>
#include <stdlib.h>

#include "ants.h"

void select_target(int antn) {
	struct my_ant * ant;
	struct target_list * tl;
	struct target * selected_target = NULL;
	struct node * node;
	int min_weight = INT_MAX;

	ant = &Game->my_ants[antn];
	node = &Game->map[ant->row][ant->col];

	if (ant->targets == NULL) {
			Log( "Warning: Ant[%d,%d] No targets?\n", ant->row, ant->col);
	}

	for(tl = ant->targets; tl; tl=tl->next ) {
			struct target * current = &Game->targets[tl->targetn];
			struct node * next_node = node->dir[current->path];

			/* Will we die? */
			if (next_node->danger > 1) {
				Log( "Ant[%d,%d] skipping target [%d,%d] because of high danger rating.\n", ant->row, ant->col, current->row, current->col);
				continue;
			}
			/* Extra careful if we're low on ants */
			if ((next_node->danger == 1) && (Game->my_count < 10)) {
				Log( "Ant[%d,%d] skipping target [%d,%d] because of low danger rating.\n", ant->row, ant->col, current->row, current->col);
				continue;
			}

			/* Collision detection */
			if (next_node->willbemovedto == Game->turn) {
					Log( "Ant[%d,%d] skipping target [%d,%d] because of collision detection.\n", ant->row, ant->col, current->row, current->col);
					continue;
			}
			/* is it more important than what we've seen before? */
			if (current->weight < min_weight) {
					min_weight = current->weight;
					selected_target = current;; 
			}
	}
	if (!(selected_target)) {
		Log( "Warning: Ant[%d,%d] Has no viable targets.\n", ant->row, ant->col);
		if (node->willbemovedto == Game->turn) {
				/* No viable targets but we can't stay were we are */
				int i;
				for (i=1;i < 4; i++) {
						if ((node->dir[i]->willbemovedto != Game->turn) && (node->danger < 2)) {
								move(antn, i);
								return;
						}
				}
				for (i=1;i < 4; i++) {
						if (node->dir[i]->willbemovedto != Game->turn) {
								move(antn, i);
								return;
						}
				}
		}
		Log( "Warning: Ant[%d,%d] Staying put.\n", ant->row, ant->col);
		node->willbemovedto = Game->turn;

		return;
	}

	Log( "Ant[%d,%d] Moving to target [%d,%d] distance %d.\n", ant->row, ant->col, selected_target->row, selected_target->col, selected_target->distance);

	ant->move = selected_target->path;
}

void clear_targetlist(struct target_list * tl) {
	struct target_list * next_tl;

	while (tl) {
		next_tl = tl->next;

		free(tl);

		tl = next_tl;
	}
}

/* Check how many other ants are closer to the food */
int get_nr_targetters(struct target * target) {
	struct target_list * food_tl;
	struct target * current;
	int n = 0;

	for ( food_tl = Game->map[target->row][target->col].targets; food_tl; food_tl = food_tl->next) {
		current = &Game->targets[food_tl->targetn];

		if (current->distance < target->distance) {
			n++;
		}
	}

	return n;
}

void weight_targets(int antn) {
	struct my_ant * ant;
	struct target_list * tl;
	int n;

	ant = &Game->my_ants[antn];

	if (ant->targets == NULL) {
			Log( "Warning: Ant[%d,%d] No targets?\n", ant->row, ant->col);
			return;
	}

	for(tl = ant->targets; tl; tl=tl->next ) {
			struct target * target = &Game->targets[tl->targetn];
			switch(target->type) {
				case TYPE_FOOD: {
					/* Braces needed for declaration following case... sometimes I hate c */
					n = 20 * get_nr_targetters(target);

					target->weight = target->distance * (90 + n);
					break;
					}
				case TYPE_ENEMYHILL:
					target->weight = target->distance * 50;
					break;
				case TYPE_FOW:
					
					if (Game->map[target->row][target->col].lastseen == INT_MIN) {
							n = 90;
					} else {
						n = (Game->turn - Game->map[target->row][target->col].lastseen) - 1;
						n *= 5;
						n = 150 - n;
						if (n < 100) n = 100;
					}

					target->weight = target->distance * n;
					break;
				default:
					Log("Warning: Unhandled type\n");
					target->weight = INT_MAX;
					break;
			}

	Log("Weighting target %d for [%d,%d] objective: type %c pos [%d,%d] at a distance of %d and a weight of %d\n",
					target->targetn,
					Game->my_ants[antn].row,
					Game->my_ants[antn].col,
					target->type,
					target->row, target->col,
					target->distance, target->weight);
	}

}

struct target * add_target (int antn, struct node * node, int distance) {
	struct target * target;
	struct target_list * new_targetlist;

	/* Check if we got room in the buffer */
	if (Game->target_count == Game->target_max) {
			int new_size = Game->target_max + ( Game->my_count * 5);
			Game->targets = realloc(Game->targets, sizeof(struct target) * new_size );
			Game->target_max = new_size;
			Log("Enlarging target buffer to %d\n", new_size);
	}

	/* Set up the target */
	target = &Game->targets[Game->target_count];

	target->targetn = Game->target_count++;
	target->distance = distance;
	target->row = node->row;
	target->col = node->col;
	target->antn = antn;
	target->type = node->type;
	target->weight = INT_MAX;

	/* Insert it into the ants target list */
	new_targetlist = malloc(sizeof(struct target_list));
	new_targetlist->targetn = target->targetn;
	new_targetlist->next = NULL;

	if (Game->my_ants[antn].targets == NULL) {
			Game->my_ants[antn].targets = new_targetlist;
	} else {
		struct target_list * tl = Game->my_ants[antn].targets;

		while(tl->next != NULL) {
				tl = tl->next;
		}
		tl->next = new_targetlist;
	}

	/* Insert it into the nodes target list */
	new_targetlist = malloc(sizeof(struct target_list));
	new_targetlist->targetn = target->targetn;
	new_targetlist->next = NULL;

	if (node->targets == NULL) {
			node->targets = new_targetlist;
	} else {
		struct target_list * tl = node->targets;

		while(tl->next != NULL) {
				tl = tl->next;
		}
		tl->next = new_targetlist;
	}

	return target;
}
