#include "recordio.h"
#include <sys/stat.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* rio_open
 * Open data file and index file. If create is requested, open both 
 * the data file and the index file with O_CREAT. If O_RDONLY or
 * O_RDWR is requested, make sure that the index file is present and 
 * return an error otherwise. On success the data file's descriptor
 * should be returned and the file descriptor for the index file
 * must be maintained within the abstraction.
 */
int rio_open(const char *pathname, int flags, mode_t mode){

	int fd, fd_index; // File descriptors
	int path_len = strlen(pathname); // Length of pathname
	char rec_pathname[6 + path_len]; // Index file name
	strcpy(rec_pathname, ".rinx.");

	strcat(rec_pathname, pathname);  // Concatenate the new index file name

	if((fd = open(pathname, flags, mode)) < 0){ // Open data file for use
		printf("ERROR: Failed to open datafile!\n");
		return -1;
	}

	if((fd_index = open(rec_pathname, flags, mode)) < 0){ // Open index file for use
		printf("ERROR: Failed to open index file!\n");
		return -1;
	}

	return fd; // Return integer file descriptor
}

/* rio_read
 * Allocate a buffer large enough to hold the requested record, read 
 * the record into the buffer and return the pointer to the allocated 
 * area. The I/O result should be returned through the return_value 
 * argument. On success this will be the number of bytes read into
 * the allocated buffer. If any system call returns an error, this
 * should be communicated to the caller through return_value.
 */
void *rio_read(int fd, int *return_value){

	struct record_descriptor r;

	// Read record from index file
	if(read(fd + 1, &r, sizeof(struct record_descriptor)) < 0){
		printf("ERROR: Failed to read from index file!\n");
		*return_value = -1;
		return 0;
	}

	void *ptr = malloc(r.length); // Create a buffer of record length

	// Read from datafile
	if((read(fd, ptr, r.length)) < 0){
		printf("ERROR: Failed to read from data file!\n");
		*return_value = -1;
		return 0;
	}

	*return_value = r.length; // Update *return_value to record length

	return ptr; // Return pointer to buffer created earlier
}

/* rio_write
 * Write a new record. If appending to the file, create a record_descriptor 
 * and fill in the values. Write the record_descriptor to the index file and 
 * the supplied data to the data file for the requested length. If updating 
 * an existing record, read the record_descriptor, check to see if the new 
 * record fits in the allocated area and rewrite. Return an error otherwise.
 */
int rio_write(int fd, const void*buf, int count){

	if(fd < 3){
		return write(fd, buf, count);
	}

	struct record_descriptor r;
	int n;
	
	// Read in the next file descriptor
	if((n = read(fd + 1, &r, sizeof(struct record_descriptor))) < 0){
		printf("ERROR: Failed to read from index file!\n");
		return 0;
	}


	if(n == 0){	// If end of file
		if(lseek(fd + 1, 0, SEEK_CUR) == 0){ // If the file is empty
			r.position = 0; // Set position to 0
			r.length = count; // Set length to count
			write(fd + 1, &r, sizeof(struct record_descriptor)); // Write new record to file
			return write(fd, buf, count); // Write to datafile and return write's return value
		} else{ // If the file has stuff in it already
			lseek(fd + 1, -1 * sizeof(struct record_descriptor), SEEK_SET); // Set currency pointer to previous record
			read(fd + 1, &r, sizeof(struct record_descriptor)); // Read previous record
			printf("Prev - r.pos: %d  |  r.len: %d\n", r.position, r.length);
			r.position += r.length; // Update position for new record
			r.length = count; // Update length to new record
			write(fd + 1, &r, sizeof(struct record_descriptor));  // Write new record to file
			return write(fd, buf, count); // Write to datafile and return write's return value
		}
	} else{
		if(count <= r.length){
			lseek(fd, r.position, SEEK_SET); // Seek to position in datafile based on record in index file
			r.length = count; // Update length of record
			write(fd + 1, &r, sizeof(struct record_descriptor)); // Rewrite record
			return write(fd, buf, count); // Rewrite in datafile and return write's return value
		} else{
			return 0; // Return 0 if nothing written
		}
		
	}
}

/* rio_lseek
 * Seek both files (data and index files) to the beginning of the requested 
 * record so that the next I/O is performed at the requested position. The
 * offset argument is in terms of records not bytes (relative to whence).
 * whence assumes the same values as lseek whence argument.
 *
 */
int rio_lseek(int fd, int offset, int whence){

	int pos;
	struct record_descriptor r;
	pos = lseek(fd + 1, offset * sizeof(struct record_descriptor), whence); // Seek to position of requested offset
	pos /= sizeof(struct record_descriptor); // Math for fixing offset from bytes to record offset

	read(fd + 1, &r, sizeof(struct record_descriptor)); // Read current record

	lseek(fd, r.position, SEEK_SET); // Seek to position in data file
	lseek(fd + 1, offset * sizeof(struct record_descriptor), whence); // Seek back to requested offset

	return pos; // Return record offset
}

/* rio_close
 * Close both files. Even though a single integer is passed as an argument, 
 * your abstraction must close the other file as well. It is suggested 
 * in rio_open that you return the descriptor obtained by opening the data file 
 * to the user and keep the index file descriptor number in the 
 * abstraction and associate them. You may also shift and pack them together 
 * into a single integer.
 */
int rio_close(int fd){

	if(close(fd) < 0){ // Close data file and return -1 on error
		return -1;
	}
	if(close(fd + 1) < 0){ // Close index file and return -1 on error
		return -1;
	}
	return 0; // Return 0 on success
}
