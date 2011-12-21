#include <sys/time.h>

#include "ants.h"

void do_turn() {
	int i;
	struct timeval before, after;

	Game->turn++;

	gettimeofday(&before, NULL);

	/* Update the map with fresh turn information from the engine */
	update_map();

	/* Pathfind and set up targets */
    for (i = 0; i < Game->my_count; ++i) {

        scan(i);

		continue;
	}

	/* Weight targets */
    for (i = 0; i < Game->my_count; ++i) {

		weight_targets(i);

		continue;
	}
	/* Select targets */
    for (i = 0; i < Game->my_count; ++i) {

		select_target(i);

		continue;
	}
	/* Move  */
	int j;
	for(j=0;j<15;j++) {
		int retry=0;

		for (i = 0; i < Game->my_count; ++i) {

			if( do_move(i) ){
				retry = 1;
			}

			continue;
		}
		if(retry == 0) {
			break;
		}
	}
	Log("Did %d retries when moving the ants\n", j);

	/* Clear the targets */
    for (i = 0; i < Game->my_count; ++i) {

		clear_targetlist(Game->my_ants[i].targets);
		Game->my_ants[i].targets = NULL;

		continue;
	}

	gettimeofday(&after, NULL);
	Log( "Processed %d ants in %ld us\n", i, ((after.tv_usec - before.tv_usec )));
}
