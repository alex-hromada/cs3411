#include <sys/stat.h>

int rio_open(const char *pathname, int flags, mode_t mode);
void *rio_read(int fd, int *return_value);
int rio_write(int fd, const void*buf, int count);
int rio_lseek(int fd, int offset, int whence);
int rio_close(int fd);
