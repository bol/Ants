#include <stdio.h>

// this header is basically self-documenting

/*
 * Global variables
 */
struct game_info * Info;
struct game_state * Game;

#ifdef DEBUG
FILE * stdlog;
void init_log( void );
void write_log(const char *, const int , const char *, const char *, ... );
void flush_log( void );
#define Log(format, ...) write_log(__FILE__, __LINE__, __func__, format,## __VA_ARGS__ )
#else
#define Log(...)
#endif

struct game_info {
	int loadtime;
	int turntime;
	int rows;
	int cols;
	int turns;
	int viewradius_sq;
	int attackradius_sq;
	int dangerradius_sq;
	int spawnradius_sq;
    int seed;
};

struct basic_ant {
    int row;
    int col;
    char player;
};

struct hill {
    int row;
    int col;
    char player;
};

struct my_ant {
    int id;
    int row;
    int col;
    unsigned char move;
    struct target_list * targets;
};

struct food {
    int row;
    int col;
};

struct game_state {
    struct my_ant *my_ants;
    struct basic_ant *enemy_ants;
    struct food *food;
    struct basic_ant *dead_ants;
	struct hill *hills;
	struct target *targets;
    
    int my_count;
    int my_count_last;
    int enemy_count;
    int food_count;
    int dead_count;
	int hill_count;
	int target_count;
	int target_max;

    int my_ant_index;
	int turn;
	int decline;

	struct node ** map;
	unsigned int * colormap;
};

#define TARGET_ANT	1
#define TARGET_FOOD	2
#define TARGET_HILL	3
#define TARGET_FOW	4

#define DIR_NORTH	0
#define DIR_EAST	1
#define DIR_SOUTH	2
#define DIR_WEST	3
#define DIR_ILLEGAL	4

#define  TYPE_WATER			'#'
#define  TYPE_FOOD			'f'
#define  TYPE_ANT			'a'
#define  TYPE_ENEMY			'e'
#define  TYPE_CORPSE		'd'
#define  TYPE_FRIENDLYHILL	'b'
#define  TYPE_ENEMYHILL		'h'
#define  TYPE_FOW			'*'
#define  TYPE_GROUND		'.'

struct node {
	char type;
	int col;
	int row;
	int willbemovedto;
	int danger;
	int lastseen;;

	struct node * dir[4];
	struct target_list * targets;
};

struct target_list {
		int targetn;
		struct target_list * next;
};

struct target {
		int targetn;
		int antn;
		int type;
		int weight;
		int row;
		int col;
		int distance;
		unsigned char path;
};


/* ants.c */
void _init_ants(char *data);
void _init_game(void);
void test_map(struct game_info *game_info);
void allocate_map(void);
void allocate_lists(void);
void _init_map(char *data);
int check_decline(void);
void update_map( void );
/* MyBot.c */
int getdistance(int row1, int col1, int row2, int col2) __attribute__ ((pure));
void move(int index, int dir);
char *get_line(char *text);
int main();
/* scan.c */
void scan(int index)									__attribute__ ((hot));
struct target * add_target (int, struct node *, int)	__attribute__ ((hot));
/* YourCode.c */
void do_turn();
/* target.c */
void select_target(int)									__attribute__ ((hot));
void clear_targetlist(struct target_list *)				__attribute__ ((hot));
int get_nr_targetters(struct target *)					__attribute__ ((pure));
void weight_targets(int)								__attribute__ ((hot));
/* allocate.c */
void init_lists( void );

