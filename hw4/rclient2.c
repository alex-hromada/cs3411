#include "r_client.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

/* Add any includes needed*/

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * rclient2 should open a local file as output and a remote file as input. 
 * It should seek the remote file to position 10 and copy the rest 
 * to the local file.
 */
int entry(int argc, char* argv[]){
	
	// open remote file for input
	int remote = r_open(argv[1], O_RDONLY | O_CREAT, 0666);
	// open local file for output
	int local = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
	
	char buf[64];
	r_lseek(remote, 10, SEEK_SET);
	int read_ret = r_read(remote, buf, 64);
	write(local, buf, read_ret);

	while(read_ret > 0){
		read_ret = r_read(remote, buf, 64);
		write(local, buf, read_ret);
	}


	r_close(remote);
	close(local);
	
	return 0; 
}
