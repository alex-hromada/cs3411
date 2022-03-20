all: filter
filter: filter.c
	gcc -g -Wall -o filter filter.c
clean:
	rm filter
submission:
	gtar czvf prog5.tgz Makefile filter.c
