#ifndef SYSCALL_H
#define SYSCALL_H

// System call numbers
#define SYS_EXIT    1
#define SYS_WRITE   2
#define SYS_READ    3
#define SYS_OPEN    4
#define SYS_CLOSE   5
#define SYS_SEEK    11
#define SYS_FORK    6
#define SYS_EXEC    7
#define SYS_WAIT    8
#define SYS_GETPID  9
#define SYS_BRK     10
#define SYS_MKDIR   12
#define SYS_RMDIR   13
#define SYS_READDIR 14
#define SYS_BRK     15
#define SYS_SBRK    16
#define SYS_PIPE    17
#define SYS_MSGGET  18
#define SYS_MSGSND  19
#define SYS_MSGRCV  20
#define SYS_MSGCTL  21
#define SYS_SHMGET  22
#define SYS_SHMAT   23
#define SYS_SHMDT   24
#define SYS_SHMCTL  25
#define SYS_SIGNAL  26
#define SYS_KILL    27
#define SYS_UNLINK  28

// System call function pointer type
typedef int (*syscall_handler_t)(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);

// Initialize system call interface
void syscall_init(void);

// Register a system call handler
void syscall_register(unsigned int syscall_num, syscall_handler_t handler);

// System call handler (called from assembly)
int syscall_handler(unsigned int syscall_num, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);

// System call implementations
int sys_exit(unsigned int exit_code, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_write(unsigned int fd, unsigned int buf, unsigned int count, unsigned int arg4);
int sys_read(unsigned int fd, unsigned int buf, unsigned int count, unsigned int arg4);
int sys_open(unsigned int path, unsigned int flags, unsigned int arg3, unsigned int arg4);
int sys_close(unsigned int fd, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_seek(unsigned int fd, unsigned int offset, unsigned int whence, unsigned int arg4);
int sys_getpid(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_mkdir(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_rmdir(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_readdir(unsigned int fd, unsigned int index, unsigned int buf, unsigned int arg4);
int sys_fork(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_exec(unsigned int path, unsigned int argv, unsigned int arg3, unsigned int arg4);
int sys_wait(unsigned int status, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_brk(unsigned int addr, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_sbrk(int increment, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_pipe(unsigned int pipefd, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_msgget(unsigned int key, unsigned int flags, unsigned int arg3, unsigned int arg4);
int sys_msgsnd(unsigned int msqid, unsigned int msgp, unsigned int msgsz, unsigned int flags);
int sys_msgrcv(unsigned int msqid, unsigned int msgp, unsigned int msgsz, unsigned int msgtyp, unsigned int flags);
int sys_msgctl(unsigned int msqid, unsigned int cmd, unsigned int buf, unsigned int arg4);
int sys_shmget(unsigned int key, unsigned int size, unsigned int flags, unsigned int arg4);
int sys_shmat(unsigned int shmid, unsigned int shmaddr, unsigned int flags, unsigned int arg4);
int sys_shmdt(unsigned int shmaddr, unsigned int arg2, unsigned int arg3, unsigned int arg4);
int sys_shmctl(unsigned int shmid, unsigned int cmd, unsigned int buf, unsigned int arg4);
int sys_signal(unsigned int signum, unsigned int handler, unsigned int arg3, unsigned int arg4);
int sys_kill(unsigned int pid, unsigned int signum, unsigned int arg3, unsigned int arg4);
int sys_unlink(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4);

#endif // SYSCALL_H

