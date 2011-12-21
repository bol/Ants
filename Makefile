CC=gcc
WARNINGS=-Wall -Wextra -Wshadow -Wcast-qual -Wwrite-strings -Wuninitialized -Wstrict-overflow=3 -Wno-sign-compare -Wsuggest-attribute=pure
CFLAGS=-c -O3 -funroll-loops
#CFLAGS=-c -O3 -funroll-loops -g -DDEBUG
LDFLAGS=-O2
LIBRARIES=-lm
SOURCES=MyBot.c turn.c ants.c scan.c target.c log.c allocate.c move.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=MyBot

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

zip:
	zip nikbol.zip ${SOURCES} *.h

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d .depend

depend: .depend

.depend: $(SOURCES)
	@rm -f .depend
	@$(CC) $(CFLAGS) -MM $^ >> .depend;

-include .depend

.PHONY: all clean
