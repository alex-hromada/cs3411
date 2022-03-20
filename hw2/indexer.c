#include <sys/stat.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* main - indexer
 * Develop a single standalone program called indexer which creates an 
 * index file for a text file whose title is supplied as an argument to 
 * the program. This program should read the text file from beginning 
 * till end, find the beginning and ending of each line (ending with 
 * the newline character), create a descriptor and write the descriptor 
 * to the created index file. The program should not modify the text 
 * file which is supplied as an argument.
 * 
 * "make indexer" will compile this source into indexer
 * To run your program: ./indexer <some_text_file>
 */
int main(int argc, char *argv[]){
	/* The index file for record i/o is named by following the 
	 * convention .rinx.<data file name> (i.e., if the data file
	 * is named myfile.txt, its index will be .rinx.myfile.txt). 
	 * This convention conveniently hides the index files while
	 * permitting easy access to them based on the data file name. 
	 */

	if(argc < 2) {
		write(1, "ERROR: Please supply a file name\n", 34);
		exit(1);
	}

	struct record_descriptor r;

	int file_name_len = strlen(argv[1]);

	char file_name[file_name_len];
	strcpy(file_name, argv[1]);

	int fd;
	if((fd = open(file_name, O_RDWR, 0600)) < 0)
		exit(1);
	
	char rec_file_name[6 + file_name_len];
	strcpy(rec_file_name, ".rinx.");

	strcat(rec_file_name, file_name);

	int record_fd;
	if((record_fd = open(rec_file_name, O_RDWR | O_CREAT, 0600)) < 0){
		exit(1);
	}
	

	char buf[1] = "0";
	int record_position = 0;
	int len = 0;

	while (read(fd, buf, 1) != 0)
	{
		len += 1;
		printf("%s\n", buf);
		if(buf[0] == '\n'){
			r.position = record_position;
			r.length = len;
			record_position = record_position + len;
			len = 0;
			printf("%d %d\n", r.position, r.length);
			write(record_fd, &r, sizeof(struct record_descriptor));
		}
	}

	r.position = record_position;
	r.length = len;
	record_position = record_position + len;
	len = 0;
	printf("%d %d\n", r.position, r.length);
	write(record_fd, &r, sizeof(struct record_descriptor));


	return 0;
}
