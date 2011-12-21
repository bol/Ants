#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ants.h"

// returns the distance squared  between two items on the grid accounting for map wrapping
int getdistance(int row1, int col1, int row2, int col2) {
    int dr, dc;
    int abs1, abs2;

    abs1 = abs(row1 - row2);
    abs2 = Info->rows - abs1;

    if (abs1 > abs2)
        dr = abs2;
    else
        dr = abs1;

    dr *= dr;

    abs1 = abs(col1 - col2);
    abs2 = Info->cols - abs1;

    if (abs1 > abs2)
        dc = abs2;
    else
        dc = abs1;

    dc *= dc;

    return (dr + dc);
}

// just a function that returns the string on a given line for i/o
// you don't need to worry about this

char *get_line(char *text) {
    char *tmp_ptr = text;
    int len = 0;

    while (*tmp_ptr != '\n') {
        ++tmp_ptr;
        ++len;
    }

    char *return_str = malloc(len + 1);
    memset(return_str, 0, len + 1);

    int i = 0;
    for (; i < len; ++i) {
        return_str[i] = text[i];
    }

    return return_str;
}

// main, communicates with tournament engine

int main() {
    int action = -1;

    Info = calloc(1, sizeof(struct game_info));
    Game = calloc(1, sizeof(struct game_state));
    Game->map = 0;

    Game->my_ants = 0;
    Game->enemy_ants = 0;
    Game->food = 0;
    Game->dead_ants = 0;

#ifdef DEBUG
	init_log();
#endif

    while (42) {
        int initial_buffer = 100000;

        char *data = malloc(initial_buffer);
        memset(data, 0, initial_buffer);

        *data = '\n';

        char *ins_data = data + 1;

        int i = 0;

        while (1 > 0) {
            ++i;

            if (i > initial_buffer) {
                initial_buffer *= 2;
                data = realloc(data, initial_buffer);
                memset(ins_data, 0, initial_buffer/2);
            }

            *ins_data = getchar();

            if (*ins_data == '\n') {
                char *backup = ins_data;

                while (*(backup - 1) != '\n') {
                    --backup;
                }

                char *test_cmd = get_line(backup);

                if (strcmp(test_cmd, "go") == 0) {
                    action = 0; 
                    free(test_cmd);
                    break;
                }
                else if (strcmp(test_cmd, "ready") == 0) {
                    action = 1;
                    free(test_cmd);
                    break;
                }
                free(test_cmd);
            }
            
            ++ins_data;
        }

        if (action == 0) {
			Log("Playing turn %d\n", Game->turn);
            char *skip_line = data + 1;
            while (*++skip_line != '\n');
            ++skip_line;

			init_lists();
            _init_map(skip_line);
            do_turn();
            fprintf(stdout, "go\n");
            fflush(stdout);
        }
        else if (action == 1) {
			Log("Read game start data\n");
            _init_ants(data + 1);
            Game->my_ant_index = -1;
			Log("Initializing memory\n");
			allocate_map();
			allocate_lists();
			Game->turn = 1;
			Log("Ready to play, signaling engine\n");
            fprintf(stdout, "go\n");
            fflush(stdout);
        }

        free(data);
    }
}
