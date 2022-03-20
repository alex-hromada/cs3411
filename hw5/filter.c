//Add any includes you require.
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>


/* main - implementation of filter
 * In this project, you will be developing a filter program that forks
 * a child and then executes a program passed as an argument and intercepts 
 * all output and input from that program. The syntax for the execution of 
 * the filter program is given by:
 * 
 * 	filter <program name> <arguments to program>
 *
 * The following commands must be implemented:
 * 	//           : Pass the character '/' as input to the program.
 * 	/i           : Go to input only mode.
 * 	/o           : Go to input/output mode.
 * 	/c           : Go to command mode.
 * 	/m <integer> : Set the max-text, maximum number of lines to be displayed.
 * 	/k <integer> : Send the child process indicated signal.
 *
 * See the spec for details.
 * 
 * After receiving each command, the program should output a prompt indicating 
 * the current mode and if there is more data to be displayed. 
 * The prompt syntax is :
 *
 * 	<pid> m <more> #
 *
 * where <pid> is the process id of the child process, m is the current mode (
 * i (input only), c (command), o(input/output)), optional <more> is the text "more" 
 * if there is data available to be displayed, and lastly the pound character.
 */

#define MODE_INPUT 'i'
#define MODE_INOUT 'o'
#define MODE_CMD 'c'

void process_stdin(struct timeval tv);
void process_stdout(struct timeval tv);
void process_stderr(struct timeval tv);
void process_cmd(char cmd);

unsigned char mode = MODE_CMD;
unsigned int maxText = 20;
unsigned int maxInput = 100;
int stdin_pipe[2];
int stdout_pipe[2];
int stderr_pipe[2];
pid_t cpid;
fd_set fdset;

int main(int argc, char *argv[]){
	//Hint: use select() to determine when reads may execute without blocking.

	// if(argc < 2){
	// 	printf("Usage: ./filter <program name> <arguments>");
	// 	exit(1);
	// }
	int wstatus;

	pipe(stdin_pipe);
	pipe(stdout_pipe);
	pipe(stderr_pipe);
	cpid = fork();

	if(cpid == 0){
		printf("\tIn child %d\n", getpid());
		printf("\tcpid = %d\n", cpid);

		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stderr_pipe[0]);

		dup2(stdin_pipe[0], 0);
		dup2(stdout_pipe[1], 1);
		dup2(stderr_pipe[1], 2);

		close(stdin_pipe[0]);
		close(stdout_pipe[1]);
		close(stderr_pipe[1]);

		int status = execvp(argv[1], &argv[1]);

		if(status == -1){
			printf("\tTerminated incorrectly\n");
			return 1;
		}


	} else{
		printf("In parent %d\n", getpid());
		printf("cpid = %d\n", cpid);

		sleep(1);

		close(stdin_pipe[0]);
		close(stdout_pipe[1]);
		close(stderr_pipe[1]);

		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		char cmd[2];
		while(1){

			FD_ZERO(&fdset);

			switch (mode)
			{
			case 'i':
			case 'c':
				FD_SET(0, &fdset);
				FD_SET(stderr_pipe[0], &fdset);
				break;
			case 'o':
				FD_SET(0, &fdset);
				FD_SET(stdout_pipe[0], &fdset);
				FD_SET(stderr_pipe[0], &fdset);
				break;
			default:
				break;
			}

			switch (mode)
			{
			case 'c':
				printf("Reading command\n");
				read(0, cmd, 2);
				if(cmd[0] == '/'){
					process_cmd(cmd[1]);
				}
				else{
					printf("Command format -> /<i,o,c,m,k\n");
					fflush(stdin);
				}
				break;
			case 'i':
				if(FD_ISSET(0, &fdset)){
					process_stdin(tv);
					process_stderr(tv);
				}
				mode = 'c';
				break;
			case 'o':
				if(FD_ISSET(stdout_pipe[0], &fdset)){
					process_stdout(tv);
					process_stderr(tv);
				}
				if(FD_ISSET(0, &fdset)){
					process_stdin(tv);
					process_stderr(tv);
				}
				mode = 'c';
				break;
			default:
				break;
			}

			struct timeval tv_more;
			tv_more.tv_sec = 1;
			tv_more.tv_usec = 0;

			FD_ZERO(&fdset);
			FD_SET(stdout_pipe[0], &fdset);
			int more = select(stdout_pipe[0] + 1, &fdset, NULL, NULL, &tv_more);
			char moreText[5] = "";
			if(more){
				strcpy(moreText, "more");
			}

			printf("%d %c %s #\n", cpid, mode, moreText);

			waitpid(cpid, &wstatus, WNOHANG);
			if(WIFEXITED(wstatus)){
				printf("The child %d has terminated with code %d\n", cpid, WEXITSTATUS(wstatus));
				return 0;
			}
		}
		

		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stderr_pipe[0]);
		
	}

	// wait(NULL);

	return 0;
}


void process_stdin(struct timeval tv){
	
	int s;
	char buf = 0;
	int inCount = 0;
	fflush(stdin);
	do{
		read(0, &buf, 1);
		write(stdin_pipe[1], &buf, 1);
		inCount += 1;
		s = select(stdin_pipe[1] + 1, &fdset, NULL, NULL, &tv);
	} while(s && (buf != EOF) && (inCount < maxInput));
}

void process_stdout(struct timeval tv){
	int s;
	int line = 0;
	FD_ZERO(&fdset);
	do{
		char buf;
		FD_SET(stdout_pipe[0], &fdset);
		s = select(stdout_pipe[0] + 1, &fdset, NULL, NULL, &tv);
		if(s <= 0)
			break;
		read(stdout_pipe[0], &buf, 1);
		if(buf == '\n')
			line += 1;
		write(1, &buf, 1);
		FD_SET(stdout_pipe[0], &fdset);
		s = select(stdout_pipe[0] + 1, &fdset, NULL, NULL, &tv);
	} while(s && (line <= maxText));

}

void process_stderr(struct timeval tv){

	int s;
	FD_ZERO(&fdset);
	do{
		char buf[32];
		FD_SET(stderr_pipe[0], &fdset);
		s = select(stderr_pipe[0] + 1, &fdset, NULL, NULL, &tv);
		if(s <= 0)
			break;
		read(stderr_pipe[0], buf, 32);
		write(2, buf, 32);
		s = select(stderr_pipe[0] + 1, &fdset, NULL, NULL, &tv);
	} while(s);
}

void process_cmd(char cmd){

	char buf1[12];
	char buf2[3];
	switch (cmd)
	{
	case 'i':
		mode = MODE_INPUT;
		printf("Now in input mode\n");
		break;
	case 'o':
		mode = MODE_INOUT;
		printf("Now in input/output mode\n");
		break;
	case 'c':
		mode = MODE_CMD;
		printf("Now in command mode\n");
		break;
	case 'm':
		read(0, buf1, 12);
		maxText  = atoi(buf1);
		printf("maxText = %d\n", maxText);
		break;
	case 'k':
		read(0, buf2, 3);
		int sig = atoi(buf2);
		kill(cpid, sig);
		break;
	default:
		printf("Command format -> /<i,o,c,m,k\n");
		fflush(stdin);
		break;
	}
}