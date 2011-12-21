#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ants.h"

/*
 * Logging functions to use with -DDEBUG
 */
void init_log() {
	stdlog = fopen("/tmp/MyBot.log", "w");
	Log( "MyBot booting up\n");
	flush_log();
}

void write_log(const char *file, const int line, const char *function, const char *format, ...) {
		va_list varlist;

		fprintf(stdlog, "%s:%d\t%-25s\t", file, line, function);

		va_start(varlist, format);
		vfprintf(stdlog, format, varlist);
		va_end(varlist);
		flush_log();
}

void flush_log() {
	fflush(stdlog);
}

void dump_map() {
		int c, r;
		FILE * mapfile;
		char * path;

		mkdir("/tmp/map", S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
		asprintf(&path, "/tmp/map/%d.map", Game->turn);
		mapfile = fopen(path, "w" );
		free(path);

		for(r=0; r < Info->rows; r++) {
			for(c=0; c < Info->cols; c++) {
					fprintf(mapfile, "%c", Game->map[r][c].type);
			}
			fprintf(mapfile, "\n");
		}

		fclose(mapfile);
}

#endif
