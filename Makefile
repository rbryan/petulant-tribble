CC=gcc
CFLAGS=-Wall -g -O0 -funroll-loops -Isge2d
LDFLAGS= -lm -lsge


all: main.o 
	$(CC) $(CFLAGS) $(LDFLAGS) main.o -o space-terraria

%.o:%.c *.h
	$(CC) $(CFLAGS) -c $*.c
