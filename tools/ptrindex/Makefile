
all : try

try : try.c 

deff.h: try.c ptrindex.pl Makefile
	perl ptrindex.pl -m deff -c try.c -o deff.h

clean:
	rm -f deff.h try.o try.exe
