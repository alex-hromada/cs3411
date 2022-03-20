/* Prototypes for RPC calls implemented in r_client.c 
 * Include r_client.h in any user program that needs
 * to call these functions.
 */
int r_open(const char *pathname, int flags, int mode);
int r_close(int fd);
int r_read(int fd, void *buf, int count);
int r_write(int fd, const void *buf, int count);
int r_lseek(int fd, int offset, int whence);
int r_pipe(int pipefd[2]);
int r_dup2(int oldfd, int newfd);

int main(int argc, char *argv[]);
