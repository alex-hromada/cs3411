#include "recordio.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


/* Develop a test program called testio which includes recordio.h and 
 * is linked against recordio.o. This program should expect a single 
 * argument which supplies a file name. The program should rio_open 
 * the supplied argument and report any errors to the user. Upon a 
 * successful open it should execute a series of rio_read statements, 
 * read the file one record at a time and write each record to the 
 * standard output as shown below.
 *
 * Data File                          Index file
 * ---------------------------------------------------
 *  Systems                            0, 8
 *  programming is cool.               8, 20
 *
 * "make testio" will compile this souce into testio
 * To execute your program: ./testio <some_record_file_name>
 */
int main(int argc, char *argv[]){

	int n, fd;
	char *record;

	// Open file for use
	if((fd = rio_open("myfile.txt", O_RDWR | O_CREAT, 0644)) < 0 ){
		printf("ERROR: rio_open failed!\n");
		exit(-1);
	}

	// Write new record
	if (( n = rio_write(fd,"Systems\n",8)) != 8){
		printf("ERROR: rio_write failed!\n");
		exit(-1);
	}

	// Write another new record
	if (( n = rio_write(fd,"programming is cool\n",20)) != 20){
		printf("ERROR: rio_write failed!\n");
		exit(-1);
	}
	
	// Seek to a record
	if((n = rio_lseek(fd, 1, SEEK_SET)) != 1){
		printf("ERROR: rio_lseek encountered a problem!\n");
		exit(-1);
	}

	// Read record at offset
	record = rio_read(fd, &n);
	if(n == 0){ 
		fprintf(stderr,"Unexpected EOF on record i/o\n"); 
		exit(-1);
	}
	if(n < 0){
		printf("ERROR: rio_read encountered a problem!\n");
		exit(-1);
	}
	write(1, record, n); // Write record to standard out
	free(record); // Free record buffer
	
	if(rio_close(fd) < 0){ // Close file
		printf("ERROR: rio_close encountered a problem!\n");
		exit(-1);
	}
	
	return 0;
}
