#include "syscall.h"
#include "process.h"
#include "idt.h"
#include "memory.h"
#include "vfs.h"
#include "vga.h"
#include "ipc.h"

// Array of system call handlers
static syscall_handler_t syscall_handlers[256];

// Initialize system call interface
void syscall_init(void) {
    // Clear all handlers
    for (int i = 0; i < 256; i++) {
        syscall_handlers[i] = 0;
    }
    
    // Register system calls
    syscall_register(SYS_EXIT, sys_exit);
    syscall_register(SYS_WRITE, sys_write);
    syscall_register(SYS_READ, sys_read);
    syscall_register(SYS_OPEN, sys_open);
    syscall_register(SYS_CLOSE, sys_close);
    syscall_register(SYS_SEEK, sys_seek);
    syscall_register(SYS_GETPID, sys_getpid);
    syscall_register(SYS_MKDIR, sys_mkdir);
    syscall_register(SYS_RMDIR, sys_rmdir);
    syscall_register(SYS_READDIR, sys_readdir);
    syscall_register(SYS_FORK, sys_fork);
    syscall_register(SYS_EXEC, sys_exec);
    syscall_register(SYS_WAIT, sys_wait);
    syscall_register(SYS_BRK, sys_brk);
    syscall_register(SYS_SBRK, sys_sbrk);
    syscall_register(SYS_PIPE, sys_pipe);
    syscall_register(SYS_MSGGET, sys_msgget);
    syscall_register(SYS_MSGSND, sys_msgsnd);
    syscall_register(SYS_MSGRCV, sys_msgrcv);
    syscall_register(SYS_MSGCTL, sys_msgctl);
    syscall_register(SYS_SHMGET, sys_shmget);
    syscall_register(SYS_SHMAT, sys_shmat);
    syscall_register(SYS_SHMDT, sys_shmdt);
    syscall_register(SYS_SHMCTL, sys_shmctl);
    syscall_register(SYS_SIGNAL, sys_signal);
    syscall_register(SYS_KILL, sys_kill);
    syscall_register(SYS_UNLINK, sys_unlink);
}

// Register a system call handler
void syscall_register(unsigned int syscall_num, syscall_handler_t handler) {
    if (syscall_num < 256) {
        syscall_handlers[syscall_num] = handler;
    }
}

// System call handler (called from assembly)
int syscall_handler(unsigned int syscall_num, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    // Check if system call number is valid
    if (syscall_num >= 256 || !syscall_handlers[syscall_num]) {
        return -1; // Invalid system call
    }
    
    // Call the registered handler
    return syscall_handlers[syscall_num](arg1, arg2, arg3, arg4);
}

// System call: exit
int sys_exit(unsigned int exit_code, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    process_exit(exit_code);
    return 0; // Never returns
}

// System call: write
int sys_write(unsigned int fd, unsigned int buf, unsigned int count, unsigned int arg4) {
    // For now, only support stdout (fd = 1) and stderr (fd = 2)
    if (fd != 1 && fd != 2) {
        return -1;
    }
    
    // Get current process
    process_t* proc = process_get_current();
    if (!proc) {
        return -1;
    }
    
    // Use VGA driver for output
    char* src = (char*) buf;
    unsigned char color = (fd == 2) ? VGA_COLOR(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK) : VGA_DEFAULT_COLOR;
    
    for (unsigned int i = 0; i < count; i++) {
        vga_putchar_color(src[i], color);
    }
    
    return count;
}

// System call: read
int sys_read(unsigned int fd, unsigned int buf, unsigned int count, unsigned int arg4) {
    // Validate file descriptor
    if (fd < 0 || fd >= 256) {
        return -1;
    }
    
    // Read from file
    int bytes_read = vfs_read(fd, (void*)buf, count);
    return bytes_read;
}

// System call: open
int sys_open(unsigned int path, unsigned int flags, unsigned int arg3, unsigned int arg4) {
    char* path_str = (char*)path;
    file_descriptor_t fd = vfs_open(path_str, flags);
    return fd;
}

// System call: close
int sys_close(unsigned int fd, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    return vfs_close(fd);
}

// System call: seek
int sys_seek(unsigned int fd, unsigned int offset, unsigned int whence, unsigned int arg4) {
    return vfs_seek(fd, (int)offset, (int)whence);
}

// System call: getpid
int sys_getpid(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    process_t* proc = process_get_current();
    if (proc) {
        return proc->pid;
    }
    return -1;
}

// System call: mkdir
int sys_mkdir(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    char* path_str = (char*)path;
    return vfs_mkdir(path_str);
}

// System call: rmdir
int sys_rmdir(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    char* path_str = (char*)path;
    return vfs_rmdir(path_str);
}

// System call: readdir
int sys_readdir(unsigned int fd, unsigned int index, unsigned int buf, unsigned int arg4) {
    vfs_node_t* node = vfs_readdir(fd, index);
    if (!node) {
        return -1;
    }
    
    // Copy node name to user buffer
    char* user_buf = (char*)buf;
    process_t* proc = process_get_current();
    if (!proc) {
        return -1;
    }
    
    // Simple copy (in a real OS, we'd validate the buffer)
    unsigned int i = 0;
    while (node->name[i] && i < 255) {
        user_buf[i] = node->name[i];
        i++;
    }
    user_buf[i] = '\0';
    
    return 0;
}

// System call: fork
int sys_fork(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    pid_t child_pid = process_fork();
    if (child_pid < 0) {
        return -1;
    }
    
    // In parent, return child PID
    // In child, return 0 (this is handled by fork implementation)
    // For now, we return child PID - the actual 0 vs PID distinction
    // would be handled by the fork implementation setting the return value
    // in the child's context
    return child_pid;
}

// System call: exec
int sys_exec(unsigned int path, unsigned int argv, unsigned int arg3, unsigned int arg4) {
    char* path_str = (char*)path;
    char* const* argv_ptr = (char* const*)argv;
    
    int result = process_exec(path_str, argv_ptr);
    // Never returns on success, returns -1 on error
    return result;
}

// System call: wait
int sys_wait(unsigned int status, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    int* status_ptr = (int*)status;
    pid_t child_pid = process_wait(status_ptr);
    return child_pid;
}

// System call: brk
int sys_brk(unsigned int addr, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    unsigned int result = process_brk(addr);
    return (int)result;
}

// System call: sbrk
int sys_sbrk(int increment, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    return process_sbrk(increment);
}

// System call: pipe
int sys_pipe(unsigned int pipefd, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    int* fd_array = (int*)pipefd;
    return pipe_create(fd_array);
}

// System call: msgget
int sys_msgget(unsigned int key, unsigned int flags, unsigned int arg3, unsigned int arg4) {
    return msgget(key, (int)flags);
}

// System call: msgsnd
int sys_msgsnd(unsigned int msqid, unsigned int msgp, unsigned int msgsz, unsigned int flags) {
    return msgsnd((int)msqid, (const void*)msgp, msgsz, (int)flags);
}

// System call: msgrcv
int sys_msgrcv(unsigned int msqid, unsigned int msgp, unsigned int msgsz, unsigned int msgtyp, unsigned int flags) {
    return msgrcv((int)msqid, (void*)msgp, msgsz, (long)msgtyp, (int)flags);
}

// System call: msgctl
int sys_msgctl(unsigned int msqid, unsigned int cmd, unsigned int buf, unsigned int arg4) {
    return msgctl((int)msqid, (int)cmd, (void*)buf);
}

// System call: shmget
int sys_shmget(unsigned int key, unsigned int size, unsigned int flags, unsigned int arg4) {
    return shmget(key, size, (int)flags);
}

// System call: shmat
int sys_shmat(unsigned int shmid, unsigned int shmaddr, unsigned int flags, unsigned int arg4) {
    void* result = shmat((int)shmid, (const void*)shmaddr, (int)flags);
    return (int)result;
}

// System call: shmdt
int sys_shmdt(unsigned int shmaddr, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    return shmdt((const void*)shmaddr);
}

// System call: shmctl
int sys_shmctl(unsigned int shmid, unsigned int cmd, unsigned int buf, unsigned int arg4) {
    return shmctl((int)shmid, (int)cmd, (void*)buf);
}

// System call: signal
int sys_signal(unsigned int signum, unsigned int handler, unsigned int arg3, unsigned int arg4) {
    return signal_register((int)signum, (signal_handler_t)handler);
}

// System call: kill
int sys_kill(unsigned int pid, unsigned int signum, unsigned int arg3, unsigned int arg4) {
    return signal_send((pid_t)pid, (int)signum);
}

// System call: unlink (delete file)
int sys_unlink(unsigned int path, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
    char* path_str = (char*)path;
    return vfs_unlink(path_str);
}

