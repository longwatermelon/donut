CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall
LDFLAGS=-lm -lncurses

all: main.c
	$(CC) $(CFLAGS) $< $(LDFLAGS)

clean:
	-rm a.out

