CC=gcc
CFLAGS=-Wall -g -O0 -funroll-loops -Isge2d
LDFLAGS= -Lsge2d -lm -lsge -lSDL -lSDL_mixer -lSDL_image


all: main.o 
	$(CC) $(CFLAGS) $(LDFLAGS) main.o -o space-terraria

%.o:%.c *.h
	$(CC) $(CFLAGS) -c $*.c
