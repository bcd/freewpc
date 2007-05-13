CC = gcc
CFLAGS = -g -I../../include

all : pgmlib pgmlib2.o

pgmlib : pgmlib.c

pgmlib2.o : pgmlib.c
	$(CC) $(CFLAGS) -DNO_MAIN -o $@ -c $<
