#include "r_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Add any includes needed*/

/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

int entry(int argc, char *argv[]);
int sockfd;

/* main - entry point for client applications.
 *
 * You are expected to develop a client program which will provide an 
 * environment into which we can plug an application and execute it 
 * using the remote versions of the supported calls. The client program 
 * therefore should expect a <hostname> <portnumber> pair as its first 
 * two arguments and attempt to connect the server. Once it connects, it
 * should call the user program which has a function called entry 
 * analogous to the main program in an ordinary C program. The entry 
 * routine should have the argv and argc arguments and return an integer 
 * value, just like the ordinary main. The client program should strip 
 * off the first two arguments, create a new argv array and call the entry procedure. 
 * Finally, when entry exits, the return value should be returned from the 
 * main procedure of the client.
 */
int main(int argc, char *argv[]){
	if(argc < 5){
		printf("./rclient <hostname> <portnumber> <input_file> <output_file>\n");
		return 0;
	}

	//you will need to call entry()
	char *remhost; u_short remport;
	struct sockaddr_in remote;
	struct hostent *h;

	remhost = argv[1]; 
	remport = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *) &remote, sizeof(remote));
	remote.sin_family = AF_INET;
	h = gethostbyname(remhost);
	bcopy((char *)h->h_addr, (char *) &remote.sin_addr, h->h_length);
	remote.sin_port = htons(remport);

	connect(sockfd, (struct sockaddr *) &remote, sizeof(remote));

	int eargc = argc - 2;
	char *eargv[] = {argv[0], argv[3], argv[4]};

	int res = entry(eargc, eargv);
	char eof = EOF;
	write(sockfd, &eof, 1);
	return res; // placeholder
}

/* r_open
 * remote open
 */
int r_open(const char *pathname, int flags, int mode){

	int L;
	char * msg;
	const char *p;
	int in_msg;
	int in_err;
	int u_l;

	p = pathname;

	while(*p) p++;
	u_l = p - pathname;
	L = u_l + 11;// int bytes for mode.

	msg = malloc(L);
	L = 0;

	msg[L++] = (open_call) & 0xff; // this is the code for open.
	msg[L++] = (u_l >> 8) & 0xff; // this is the length.
	msg[L++] = (u_l) & 0xff;

	int i;
	for (i = 0; i < u_l; i++)
		msg[L++] = (pathname[i]) & 0xff; // put the pathname.

	msg[L++] = (flags >> 24) & 0xff; // put the flags.
	msg[L++] = (flags >> 16) & 0xff;
	msg[L++] = (flags >> 8) & 0xff;
	msg[L++] = (flags ) & 0xff;

	msg[L++] = (mode >> 24) & 0xff; // put the mode.
	msg[L++] = (mode >> 16) & 0xff;
	msg[L++] = (mode >> 8) & 0xff;
	msg[L++] = (mode ) & 0xff;


	// This is where you can split (A).
	write(sockfd, msg, L);

	read(sockfd, msg, 8);

	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	free(msg);

	errno = in_err;
	return in_msg;

}

/* r_close
 * remote close
 */
int r_close(int fd){
	int L;
	char * msg;
	int in_msg;
	int in_err;

	L = 0;
	msg = malloc(5);

	msg[L++] = (close_call) & 0xff; // put code for close

	msg[L++] = (fd >> 24) & 0xff; // put the fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >> 8) & 0xff;
	msg[L++] = (fd ) & 0xff;

	write(sockfd, msg, L); // Send the packet

	read(sockfd, msg, 8); // Receive a packet

	msg = malloc(8);
	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	free(msg); // free

	errno = in_err;
	return in_msg;

	return in_msg; // placeholder
}

/* r_read
 * remote read
 */
int r_read(int fd, void *buf, int count){
	int L;
	char * msg;
	int in_msg;
	int in_err;

	L = 0;
	msg = malloc(9);

	msg[L++] = (read_call) & 0xff; // Put read code

	msg[L++] = (fd >> 24) & 0xff; // put the fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >> 8 ) & 0xff;
	msg[L++] = (fd 		) & 0xff;

	msg[L++] = (count >> 24) & 0xff; // put the count
	msg[L++] = (count >> 16) & 0xff;
	msg[L++] = (count >> 8 ) & 0xff;
	msg[L++] = (count	   ) & 0xff;

	write(sockfd, msg, L); // Send packet

	msg = malloc(8 + count);
	read(sockfd, msg, count + 8); // Receive packet
	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	int i;
	for(i = 0; i < in_msg; i++){
		((char *)buf)[i] = msg[i+8];
	}

	free(msg);

	errno = in_err;
	return in_msg;
}

/* r_write
 * remote write
 */
int r_write(int fd, const void *buf, int count){

	int L;
	char * msg;
	int in_msg;
	int in_err;

	L = 0;
	msg = malloc(9 + count);

	msg[L++] = (write_call) & 0xff; // put write code

	msg[L++] = (fd >> 24) & 0xff; // put the fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >> 8 ) & 0xff;
	msg[L++] = (fd 		) & 0xff;

	msg[L++] = (count >> 24) & 0xff; // put the count
	msg[L++] = (count >> 16) & 0xff;
	msg[L++] = (count >> 8 ) & 0xff;
	msg[L++] = (count	   ) & 0xff;

	int i;
	for (i = 0; i < count; i++)
		msg[L++] = (((const char *)buf)[i]) & 0xff;

	write(sockfd, msg, L);  // send packet

	read(sockfd, msg, 8);  // receive packet

	msg = malloc(8);
	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	free(msg);

	errno = in_err;
	return in_msg;
}

/* r_lseek
 * remote seek
 */
int r_lseek(int fd, int offset, int whence){

	int L;
	char * msg;
	int in_msg;
	int in_err;

	L = 0;
	msg = malloc(13);

	msg[L++] = (seek_call) & 0xff; // put the lseek code

	msg[L++] = (fd >> 24) & 0xff; // put the fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >> 8 ) & 0xff;
	msg[L++] = (fd 		) & 0xff;

	msg[L++] = (offset >> 24) & 0xff; // put the offeset
	msg[L++] = (offset >> 16) & 0xff;
	msg[L++] = (offset >> 8 ) & 0xff;
	msg[L++] = (offset	   ) & 0xff;

	msg[L++] = (whence >> 24) & 0xff; // put whence
	msg[L++] = (whence >> 16) & 0xff;
	msg[L++] = (whence >> 8 ) & 0xff;
	msg[L++] = (whence	   ) & 0xff;

	write(sockfd, msg, L);  // send packet

	read(sockfd, msg, 8);  // send packet

	msg = malloc(8);
	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	free(msg);

	errno = in_err;
	return in_msg;
}

/* r_pipe
 * remote pipe
 */
int r_pipe(int pipefd[2]){
	char* msg;
	int in_msg;
	int in_err;
	char call = pipe_call;

	write(sockfd, &call, 1);  // send packet

	msg = malloc(16);
	read(sockfd, msg, 16); // receive packet

	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];
	pipefd[0] = (msg[8] << 24) | (msg[9] << 16) | (msg[10] << 8) | msg[11];
	pipefd[1] = (msg[12] << 24) | (msg[13] << 16) | (msg[14] << 8) | msg[15];

	free(msg);

	errno = in_err;
	return in_msg;
}

/* r_dup2
 * remote dup2
 */
int r_dup2(int oldfd, int newfd){

	int L;
	char * msg;
	int in_msg;
	int in_err;

	L = 0;
	msg = malloc(9);

	msg[L++] = (dup2_call) & 0xff; // put dup2 code

	msg[L++] = (oldfd >> 24) & 0xff; // put the oldfd
	msg[L++] = (oldfd >> 16) & 0xff;
	msg[L++] = (oldfd >> 8 ) & 0xff;
	msg[L++] = (oldfd 	   ) & 0xff;

	msg[L++] = (newfd >> 24) & 0xff; // put the newfd
	msg[L++] = (newfd >> 16) & 0xff;
	msg[L++] = (newfd >> 8 ) & 0xff;
	msg[L++] = (newfd	   ) & 0xff;

	write(sockfd, msg, L); // send packet

	msg = malloc(8);
	read(sockfd, msg, 8);  // receive packet

	in_msg = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
	in_err = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

	free(msg);

	errno = in_err;
	return in_msg;
}
