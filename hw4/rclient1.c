#include "r_client.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* Add any includes needed*/

// int socket;

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * rclient1 remotely opens an output file, locally opens an input file, 
 * copies the input file to the output file and closes both files.
 */
int entry(int argc, char* argv[]){
	// open local file for input
	int local = open(argv[1], O_RDONLY | O_CREAT, 0666);
	// open remote file for output
	int remote = r_open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
	
	char buf[64];
	int read_ret = read(local, buf, 64);
	r_write(remote, buf, read_ret);
	while(read_ret > 0){
		read_ret = read(local, buf, 64);
		r_write(remote, buf, read_ret);
	}

	r_close(remote);
	close(local);
	
	return 0; //placeholder
}
