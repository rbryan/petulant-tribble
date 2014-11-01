CC=gcc
CFLAGS=-Wall -g -O0 -funroll-loops -Iinclude -I/usr/include/SDL 
LDFLAGS= -lm 


all: main.o 
	$(CC) $(CFLAGS) $(LDFLAGS) lib/libsge.a main.o -o space-terraria

%.o:%.c *.h
	$(CC) $(CFLAGS) -c $*.c
