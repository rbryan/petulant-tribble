CC=gcc
CFLAGS=-Wall -g -O0 -Iinclude -I/usr/include/SDL -Llib
LDFLAGS= -lm -lSDL -lSDL_mixer -lSDL_image -lsge

all: main.o 
	$(CC) $(CFLAGS) $(LDFLAGS) main.o -o space-terraria

%.o:%.c *.h
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm ./*.o space-terraria
