all: czy dzy bitsy
bitsy: bitsy.h bitsy.c
	gcc -c -Wall bitsy.c
czy: bitsy czy.c
	gcc -Wall -o czy bitsy.o czy.c
dzy: bitsy dzy.c
	gcc -Wall -o dzy bitsy.o dzy.c
clean:
	rm czy dzy bitsy.o
submission:
	gtar czvf prog3.tgz Makefile bitsy.h bitsy.c czy.c dzy.c README
