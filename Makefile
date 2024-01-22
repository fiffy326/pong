CC = cc
CFLAGS = -std=c99 -pedantic -Wall -O3

all: pong

pong: pong.c
	$(CC) -o $@ $(shell pkg-config --cflags --libs sdl2) $(CFLAGS) $<

clean:
	rm -f pong
