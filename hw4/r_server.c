/* Add any includes needed*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

void open_handler(int conn);
void close_handler(int conn);
void read_handler(int conn);
void write_handler(int conn);
void lseek_handler(int conn);
void pipe_handler(int conn);

/* In this project, we will develop a mini Remote Procedure Call (RPC) 
 * based system consisting of a em server and a client. Using the remote 
 * procedures supplied by the server our client program will be able to 
 * open files and perform computations on the server. 
 *
 * The server should open a socket and listen on an available port. The 
 * server program upon starting should print the port number it is using 
 * on the standard output 
 * (Print only the port number with no additional formatting. You may use printf \%d). 
 * This port number is then manually passed as a command line argument to 
 * the client to establish the connection. In order to implement the RPC, 
 * the server and the client should communicate through a TCP socket. It 
 * is allowed to fork a child for each open connection and delagate the 
 * handling to the child.
 */

/**
 * Server side handler for open call
 */
void open_handler(int conn){
	char buf[4];

	// decode pathname length
	if(read(conn, buf, 2) < 0){
		perror("ERROR DECODING");
	}
	int length = (buf[0] << 8) | buf[1];

	// decode pathname
	char* path = malloc(length);
	if(read(conn, path, length) < 0){
		perror("ERROR DECODING");
	}

	memset(buf, 0, 4);

	// Decode flags
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int flags = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	memset(buf, 0, 4);

	// decode mode
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int mode = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// perform open
	int fd = open(path, flags, mode);
	if(fd < 0){
		perror("ERROR ON OPEN");
	}

	int err = errno;

	int L = 0;
	char msg[8];

	msg[L++] = (fd >> 24) & 0xff; // put fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >> 8) & 0xff;
	msg[L++] = (fd) & 0xff;

	msg[L++] = (err >> 24) & 0xff; // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8) & 0xff;
	msg[L++] = (err ) & 0xff;

	write(conn, msg, 8);

	free(path);
}

/**
 * Server side handler for close
 */
void close_handler(int conn){
	char buf[4];
	
	// decode fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int fd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// perform close
	int close_ret = close(fd);
	if(close_ret < 0){
		perror("ERROR ON CLOSE");
	}

	int err = errno;

	int L = 0;
	char msg[8];

	msg[L++] = (close_ret >> 24) & 0xff;  // Put close return val
	msg[L++] = (close_ret >> 16) & 0xff;
	msg[L++] = (close_ret >> 8 ) & 0xff;
	msg[L++] = (close_ret		 ) & 0xff;

	msg[L++] = (err >> 24) & 0xff; // put errno val
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err 	 ) & 0xff;

	write(conn, msg, 8);

}

/**
 * Server side handler for read
 */
void read_handler(int conn){
	char buf[4];

	// Decode fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int fd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode count
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int count = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	char* buf2 = malloc(count);

	// perform read
	int read_ret = read(fd, buf2, count);
	if(read_ret < 0){
		perror("ERROR ON READ");
	}

	int err = errno;

	int L = 0;
	char *msg = malloc(read_ret + 8);

	msg[L++] = (read_ret >> 24) & 0xff;  // put read return val
	msg[L++] = (read_ret >> 16) & 0xff;
	msg[L++] = (read_ret >> 8 ) & 0xff;
	msg[L++] = (read_ret ) & 0xff;

	msg[L++] = (err >> 24) & 0xff;  // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err 	 ) & 0xff;

	int i;
	for(i = 0; i < read_ret; i++){
		msg[L++] = buf2[i];
	}

	write(conn, msg, read_ret + 8);

	free(buf2); free(msg);

}

/**
 * Server side handler for write
 */
void write_handler(int conn){
	char buf[4];
	// Decode fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int fd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode count
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int count = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode data
	char *data = malloc(count);
	if(read(conn, data, count) < 0){
		perror("ERROR DECODING");
	}

	// perform write
	int write_ret = write(fd, data, count);
	if(write_ret < 0){
		perror("ERROR ON WRITE");
	}

	int err = errno;

	int L = 0;
	char msg[8];

	msg[L++] = (write_ret >> 24) & 0xff;  // put write return val
	msg[L++] = (write_ret >> 16) & 0xff;
	msg[L++] = (write_ret >> 8 ) & 0xff;
	msg[L++] = (write_ret ) & 0xff;

	msg[L++] = (err >> 24) & 0xff; // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err ) & 0xff;

	write(conn, msg, 8);
}

/**
 * Server side handler for lseek
 */
void lseek_handler(int conn){
	char buf[4];
	// Decode fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int fd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode offset
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int offset = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode whence
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int whence = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// perform lseek
	int lseek_ret = lseek(fd, offset, whence);
	if(lseek_ret < 0){
		perror("ERROR ON SEEK");
	}

	int err = errno;

	int L = 0;
	char msg[8];

	msg[L++] = (lseek_ret >> 24) & 0xff;  // put lseek return val
	msg[L++] = (lseek_ret >> 16) & 0xff;
	msg[L++] = (lseek_ret >> 8 ) & 0xff;
	msg[L++] = (lseek_ret		 ) & 0xff;

	msg[L++] = (err >> 24) & 0xff; // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err 	 ) & 0xff;

	write(conn, msg, 8);

}

/**
 * Server side handler for pipe
 */
void pipe_handler(int conn){
	int pipefiledes[2];
	// perform pipe
	int pipe_ret = pipe(pipefiledes);
	if(pipe_ret < 0){
		perror("ERROR ON PIPE");
	}

	int err = errno;

	int L = 0;
	char msg[16];

	msg[L++] = (pipe_ret >> 24) & 0xff;  // put pipe return val
	msg[L++] = (pipe_ret >> 16) & 0xff;
	msg[L++] = (pipe_ret >> 8 ) & 0xff;
	msg[L++] = (pipe_ret		 ) & 0xff;

	msg[L++] = (err >> 24) & 0xff;  // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err 	 ) & 0xff;

	msg[L++] = (pipefiledes[0] >> 24) & 0xff; // put pipe file des 1
	msg[L++] = (pipefiledes[0] >> 16) & 0xff;
	msg[L++] = (pipefiledes[0] >> 8 ) & 0xff;
	msg[L++] = (pipefiledes[0]		) & 0xff;

	msg[L++] = (pipefiledes[1] >> 24) & 0xff; // put pipe file des 2
	msg[L++] = (pipefiledes[1] >> 16) & 0xff;
	msg[L++] = (pipefiledes[1] >> 8 ) & 0xff;
	msg[L++] = (pipefiledes[1] 	 	) & 0xff;

	write(conn, msg, 16);
}

/**
 * Server side handler for dup2
 */
void dup2_handler(int conn){
	char buf[4];

	// decode old fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int oldfd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// decode new fd
	if(read(conn, buf, 4) < 0){
		perror("ERROR DECODING");
	}
	int newfd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// perform dup2
	int dup2_ret = dup2(oldfd, newfd);
	if(dup2_ret < 0){
		perror("ERROR ON DUP2");
	}

	int err = errno;

	int L = 0;
	char msg[8];

	msg[L++] = (dup2_ret >> 24) & 0xff; // put dup2 return val
	msg[L++] = (dup2_ret >> 16) & 0xff;
	msg[L++] = (dup2_ret >> 8 ) & 0xff;
	msg[L++] = (dup2_ret	  ) & 0xff;

	msg[L++] = (err >> 24) & 0xff;  // put errno
	msg[L++] = (err >> 16) & 0xff;
	msg[L++] = (err >> 8 ) & 0xff;
	msg[L++] = (err 	 ) & 0xff;

	write(conn, msg, 8);

}

/* main - server implementation
 */
int main(int argc, char *argv[]){

	int listener, conn, length;
	struct sockaddr_in s1, s2;
	listener = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char*) &s1, sizeof(s1));
	s1.sin_family = AF_INET;
	s1.sin_addr.s_addr = htonl(INADDR_ANY);
	s1.sin_port = htons(0);
	bind(listener, (struct sockaddr *)&s1, sizeof(s1));
	length = sizeof(s1);
	getsockname(listener, (struct sockaddr *)&s1, (socklen_t* restrict)&length);
	// char hostname[128];
	// hostname[127] = '\0';
	// gethostname(hostname, 127);
	// printf("Server host: %s\n", hostname);
	printf("Port number %d\n", ntohs(s1.sin_port));

	listen(listener,1);
	length = sizeof(s2);

	while(1){
		conn = accept(listener, (struct sockaddr *)&s2, (socklen_t* restrict)&length);  // Accept new connection
		if(fork() == 0){
			close(listener);
			char call;
			do{
				read(conn, &call, 1); // read RPC call
				switch(call){
					case open_call:
						// printf("Received open call\n");
						open_handler(conn);
						break;
					case close_call:
						// printf("Received close call\n");
						close_handler(conn);
						break;
					case read_call:
						// printf("Received read call\n");
						read_handler(conn);
						break;
					case write_call:
						// printf("Received write call\n");
						write_handler(conn);
						break;
					case seek_call:
						// printf("Received lseek call\n");
						lseek_handler(conn);
						break;
					case pipe_call:
						// printf("Received pipe call\n");
						pipe_handler(conn);
						break;
					case dup2_call:
						// printf("Received dup2 call\n");
						dup2_handler(conn);
						break;
					default:
						break;
				}
			}while (call != EOF);
			exit(1);
		}
		close(conn);
	}
	return 0; // placeholder
}
