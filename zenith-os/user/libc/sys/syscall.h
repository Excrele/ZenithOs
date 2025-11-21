#ifndef SYSCALL_H
#define SYSCALL_H

// System call numbers (must match kernel/src/syscall.h)
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

// System call wrapper macro
// EAX = syscall number, EBX = arg1, ECX = arg2, EDX = arg3, ESI = arg4
static inline int syscall(int num, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    int result;
    asm volatile (
        "int $0x80"
        : "=a" (result)
        : "a" (num), "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
        : "memory"
    );
    return result;
}

#endif // SYSCALL_H

