#ifndef IPC_H
#define IPC_H

#include "process.h"

// IPC types
#define IPC_TYPE_PIPE        1
#define IPC_TYPE_MQUEUE     2
#define IPC_TYPE_SHMEM      3
#define IPC_TYPE_SIGNAL     4

// Pipe structure
typedef struct pipe {
    unsigned char* buffer;      // Circular buffer
    unsigned int size;          // Buffer size
    unsigned int read_pos;      // Read position
    unsigned int write_pos;     // Write position
    unsigned int count;         // Number of bytes in buffer
    pid_t reader;              // Process reading from pipe
    pid_t writer;              // Process writing to pipe
    int ref_count;             // Reference count
} pipe_t;

// Message queue structure
typedef struct message {
    unsigned int type;          // Message type
    unsigned int size;          // Message size
    unsigned char data[256];    // Message data
    struct message* next;       // Next message in queue
} message_t;

typedef struct message_queue {
    unsigned int key;           // Queue key/ID
    unsigned int max_messages;  // Maximum messages
    unsigned int message_count; // Current message count
    message_t* first;           // First message
    message_t* last;            // Last message
    int ref_count;             // Reference count
} message_queue_t;

// Shared memory structure
typedef struct shared_memory {
    unsigned int key;           // Shared memory key/ID
    unsigned int size;          // Size in bytes
    unsigned int virtual_addr;  // Virtual address
    unsigned int physical_addr; // Physical address
    int ref_count;             // Reference count
    pid_t owner;               // Owner process
} shared_memory_t;

// Signal numbers
#define SIGINT    2   // Interrupt
#define SIGQUIT   3   // Quit
#define SIGKILL   9   // Kill (cannot be caught)
#define SIGTERM   15  // Termination
#define SIGCHLD   17  // Child process terminated
#define SIGUSR1   10  // User-defined signal 1
#define SIGUSR2   12  // User-defined signal 2

// Signal handler type
typedef void (*signal_handler_t)(int);

// Signal action structure
typedef struct {
    signal_handler_t handler;   // Signal handler
    unsigned int flags;         // Signal flags
} signal_action_t;

// Initialize IPC subsystem
void ipc_init(void);

// Pipe operations
int pipe_create(int pipefd[2]);
int pipe_read(int fd, void* buf, unsigned int count);
int pipe_write(int fd, const void* buf, unsigned int count);
int pipe_close(int fd);

// Message queue operations
int msgget(unsigned int key, int flags);
int msgsnd(int msqid, const void* msgp, unsigned int msgsz, int flags);
int msgrcv(int msqid, void* msgp, unsigned int msgsz, long msgtyp, int flags);
int msgctl(int msqid, int cmd, void* buf);

// Shared memory operations
int shmget(unsigned int key, unsigned int size, int flags);
void* shmat(int shmid, const void* shmaddr, int flags);
int shmdt(const void* shmaddr);
int shmctl(int shmid, int cmd, void* buf);

// Signal operations
int signal_register(int signum, signal_handler_t handler);
int signal_send(pid_t pid, int signum);
void signal_handle(void);

#endif // IPC_H

