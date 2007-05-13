
CFLAGS = -I../../tools/pgmlib -I. -I../../include -DNO_MAIN
VPATH = ../../tools/pgmlib

fiftool : fiftool.c pgmlib.c

.PHONY : run
run: fiftool
	./fiftool -o test.fif -c test.pgm
