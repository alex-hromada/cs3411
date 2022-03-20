all: hmalloc main
	gcc -o hmalloc hmalloc.o main.o
hmalloc: hmalloc.c hmalloc.h
	gcc -Wall -c hmalloc.c
main:
	gcc -Wall -c main.c
clean:
	rm hmalloc hmalloc.o main.o
submission:
	gtar czvf prog1.tgz Makefile main.c hmalloc.c hmalloc.h
debug: main.c hmalloc.c hmalloc.h
	gcc -Wall -g -o hmalloc hmalloc.c main.c
