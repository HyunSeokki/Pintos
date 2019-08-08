#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

/* prj2 Add */
int where_vaddr(void *vaddr); /* 1: user, 0: Kernel */
void syscall_halt(void);
void syscall_exit(int exitstatus);
int syscall_exec(const char *cmd_line);
int syscall_wait(int pid);
int syscall_read(int fd, void *buffer, unsigned size);
int syscall_write(int fd, const void *buffer, unsigned size);
int syscall_fibo(int n);
int syscall_4sum(int a, int b, int c, int d);
int syscall_open(const char *filename);
void syscall_close(int fd);
int syscall_filesize(int fd);
void syscall_seek(int fd, unsigned new_pos);
unsigned syscall_tell(int fd);
#endif /* userprog/syscall.h */
