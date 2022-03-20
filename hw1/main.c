#include "hmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*You may include any other relevant headers here.*/

#define malloc hmalloc
#define free hfree

/*	main()
 *	Use this function to develop tests for hmalloc. You should not 
 *	implement any of the hmalloc functionality here. That should be
 *	done in hmalloc.c
 *	This file will not be graded. When grading I will replace main 
 *	with my own implementation for testing.*/
int main(int argc, char *argv[]){

	int *a = (int *) malloc(sizeof(int) * 8);
	char *b = (char *) malloc(sizeof(char) * 8);
	int *c = (int *) malloc(sizeof(int) * 8);
	int *d = (int *) malloc(sizeof(int) * 8);
	int *zzz = (int *) hcalloc(sizeof(int) * 8);

	strcpy(b, "hello\n");


	// some calls to hmalloc
	// ...
	// some calls to hfree
	free(a);
	free(b);
	free(c);
	free(d);
	
	int *e = (int *) malloc(sizeof(int) * 12);
	int *f = (int *) malloc(sizeof(int) * 4);

	traverse();


	free(e);
	free(f);
	free(zzz);

	traverse();

	int *y = (int *) malloc(sizeof(int) * 4);
	int *t = (int *) malloc(sizeof(int) * 8);

	traverse();

	free(t);
	free(y);

	traverse();

	
	return 1;
}
