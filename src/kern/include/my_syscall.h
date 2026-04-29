#include <types.h>
#include <lib.h>
#include <syscall.h>

int sys_read(int fd, userptr_t buf, size_t size, int *retval);
int sys_write(int fd, userptr_t buf, size_t size, int *retval);
int sys_exit(int exitcode);