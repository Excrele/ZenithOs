#include "ipc.h"
#include "pmm.h"
#include "paging.h"
#include "scheduler.h"
#include "heap.h"

// IPC globals
#define MAX_PIPES 64
#define MAX_MQUEUES 32
#define MAX_SHMEM 32
#define PIPE_BUFFER_SIZE 4096

static pipe_t g_pipes[MAX_PIPES];
static message_queue_t g_mqueues[MAX_MQUEUES];
static shared_memory_t g_shmem[MAX_SHMEM];
static int g_pipe_count = 0;
static int g_mqueue_count = 0;
static int g_shmem_count = 0;

// Signal handlers per process
#define MAX_SIGNALS 32
static signal_action_t g_signal_handlers[MAX_PIPES][MAX_SIGNALS]; // Simplified: using MAX_PIPES as max processes

// Initialize IPC subsystem
void ipc_init(void) {
    g_pipe_count = 0;
    g_mqueue_count = 0;
    g_shmem_count = 0;
    
    // Clear all structures
    for (int i = 0; i < MAX_PIPES; i++) {
        g_pipes[i].buffer = 0;
        g_pipes[i].size = 0;
        g_pipes[i].read_pos = 0;
        g_pipes[i].write_pos = 0;
        g_pipes[i].count = 0;
        g_pipes[i].ref_count = 0;
    }
    
    for (int i = 0; i < MAX_MQUEUES; i++) {
        g_mqueues[i].key = 0;
        g_mqueues[i].ref_count = 0;
    }
    
    for (int i = 0; i < MAX_SHMEM; i++) {
        g_shmem[i].key = 0;
        g_shmem[i].ref_count = 0;
    }
}

// Find a free pipe slot
static int find_free_pipe(void) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (g_pipes[i].buffer == 0) {
            return i;
        }
    }
    return -1;
}

// Create a pipe
int pipe_create(int pipefd[2]) {
    int idx = find_free_pipe();
    if (idx < 0) {
        return -1; // No free pipes
    }
    
    pipe_t* pipe = &g_pipes[idx];
    
    // Allocate buffer
    pipe->buffer = (unsigned char*)kmalloc(PIPE_BUFFER_SIZE);
    if (!pipe->buffer) {
        return -1; // Out of memory
    }
    
    pipe->size = PIPE_BUFFER_SIZE;
    pipe->read_pos = 0;
    pipe->write_pos = 0;
    pipe->count = 0;
    pipe->ref_count = 2; // Two file descriptors
    pipe->reader = 0;
    pipe->writer = 0;
    
    // Get current process
    process_t* proc = process_get_current();
    if (proc) {
        pipe->reader = proc->pid;
        pipe->writer = proc->pid;
    }
    
    // Return file descriptors (simplified - in real OS, these would be actual FDs)
    pipefd[0] = idx * 2;      // Read end
    pipefd[1] = idx * 2 + 1;  // Write end
    
    g_pipe_count++;
    return 0;
}

// Read from pipe
int pipe_read(int fd, void* buf, unsigned int count) {
    int pipe_idx = fd / 2;
    if (pipe_idx < 0 || pipe_idx >= MAX_PIPES || (fd % 2) != 0) {
        return -1; // Invalid file descriptor
    }
    
    pipe_t* pipe = &g_pipes[pipe_idx];
    if (!pipe->buffer) {
        return -1; // Pipe doesn't exist
    }
    
    unsigned char* dest = (unsigned char*)buf;
    unsigned int bytes_read = 0;
    
    while (bytes_read < count && pipe->count > 0) {
        dest[bytes_read++] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % pipe->size;
        pipe->count--;
    }
    
    return bytes_read;
}

// Write to pipe
int pipe_write(int fd, void* buf, unsigned int count) {
    int pipe_idx = fd / 2;
    if (pipe_idx < 0 || pipe_idx >= MAX_PIPES || (fd % 2) != 1) {
        return -1; // Invalid file descriptor
    }
    
    pipe_t* pipe = &g_pipes[pipe_idx];
    if (!pipe->buffer) {
        return -1; // Pipe doesn't exist
    }
    
    unsigned char* src = (unsigned char*)buf;
    unsigned int bytes_written = 0;
    
    while (bytes_written < count && pipe->count < pipe->size) {
        pipe->buffer[pipe->write_pos] = src[bytes_written++];
        pipe->write_pos = (pipe->write_pos + 1) % pipe->size;
        pipe->count++;
    }
    
    return bytes_written;
}

// Close pipe
int pipe_close(int fd) {
    int pipe_idx = fd / 2;
    if (pipe_idx < 0 || pipe_idx >= MAX_PIPES) {
        return -1;
    }
    
    pipe_t* pipe = &g_pipes[pipe_idx];
    if (!pipe->buffer) {
        return -1;
    }
    
    pipe->ref_count--;
    if (pipe->ref_count <= 0) {
        kfree(pipe->buffer);
        pipe->buffer = 0;
        pipe->size = 0;
        g_pipe_count--;
    }
    
    return 0;
}

// Find message queue by key
static int find_mqueue_by_key(unsigned int key) {
    for (int i = 0; i < MAX_MQUEUES; i++) {
        if (g_mqueues[i].key == key && g_mqueues[i].ref_count > 0) {
            return i;
        }
    }
    return -1;
}

// Find free message queue slot
static int find_free_mqueue(void) {
    for (int i = 0; i < MAX_MQUEUES; i++) {
        if (g_mqueues[i].ref_count == 0) {
            return i;
        }
    }
    return -1;
}

// Create or get message queue
int msgget(unsigned int key, int flags) {
    int idx = find_mqueue_by_key(key);
    
    if (idx >= 0) {
        // Queue exists
        g_mqueues[idx].ref_count++;
        return idx;
    }
    
    // Create new queue
    idx = find_free_mqueue();
    if (idx < 0) {
        return -1; // No free queues
    }
    
    message_queue_t* mq = &g_mqueues[idx];
    mq->key = key;
    mq->max_messages = 64;
    mq->message_count = 0;
    mq->first = 0;
    mq->last = 0;
    mq->ref_count = 1;
    
    g_mqueue_count++;
    return idx;
}

// Send message
int msgsnd(int msqid, const void* msgp, unsigned int msgsz, int flags) {
    if (msqid < 0 || msqid >= MAX_MQUEUES) {
        return -1;
    }
    
    message_queue_t* mq = &g_mqueues[msqid];
    if (mq->ref_count == 0) {
        return -1; // Queue doesn't exist
    }
    
    if (mq->message_count >= mq->max_messages) {
        return -1; // Queue full
    }
    
    // Allocate message
    message_t* msg = (message_t*)kmalloc(sizeof(message_t));
    if (!msg) {
        return -1; // Out of memory
    }
    
    msg->type = *((unsigned int*)msgp);
    msg->size = (msgsz > 256) ? 256 : msgsz;
    for (unsigned int i = 0; i < msg->size; i++) {
        msg->data[i] = ((unsigned char*)msgp)[i + sizeof(unsigned int)];
    }
    msg->next = 0;
    
    // Add to queue
    if (mq->last) {
        mq->last->next = msg;
    } else {
        mq->first = msg;
    }
    mq->last = msg;
    mq->message_count++;
    
    return 0;
}

// Receive message
int msgrcv(int msqid, void* msgp, unsigned int msgsz, long msgtyp, int flags) {
    if (msqid < 0 || msqid >= MAX_MQUEUES) {
        return -1;
    }
    
    message_queue_t* mq = &g_mqueues[msqid];
    if (mq->ref_count == 0) {
        return -1; // Queue doesn't exist
    }
    
    if (mq->message_count == 0) {
        return 0; // No messages
    }
    
    // Find message (simplified - just get first message)
    message_t* msg = mq->first;
    if (!msg) {
        return 0;
    }
    
    // Copy message
    *((unsigned int*)msgp) = msg->type;
    unsigned int copy_size = (msg->size < msgsz) ? msg->size : msgsz;
    for (unsigned int i = 0; i < copy_size; i++) {
        ((unsigned char*)msgp)[i + sizeof(unsigned int)] = msg->data[i];
    }
    
    // Remove from queue
    mq->first = msg->next;
    if (!mq->first) {
        mq->last = 0;
    }
    mq->message_count--;
    
    kfree(msg);
    return copy_size + sizeof(unsigned int);
}

// Control message queue
int msgctl(int msqid, int cmd, void* buf) {
    if (msqid < 0 || msqid >= MAX_MQUEUES) {
        return -1;
    }
    
    message_queue_t* mq = &g_mqueues[msqid];
    if (mq->ref_count == 0) {
        return -1;
    }
    
    // Simplified - just handle removal
    if (cmd == 0) { // IPC_RMID
        mq->ref_count--;
        if (mq->ref_count <= 0) {
            // Free all messages
            message_t* msg = mq->first;
            while (msg) {
                message_t* next = msg->next;
                kfree(msg);
                msg = next;
            }
            mq->key = 0;
            mq->ref_count = 0;
            g_mqueue_count--;
        }
        return 0;
    }
    
    return -1;
}

// Find shared memory by key
static int find_shmem_by_key(unsigned int key) {
    for (int i = 0; i < MAX_SHMEM; i++) {
        if (g_shmem[i].key == key && g_shmem[i].ref_count > 0) {
            return i;
        }
    }
    return -1;
}

// Find free shared memory slot
static int find_free_shmem(void) {
    for (int i = 0; i < MAX_SHMEM; i++) {
        if (g_shmem[i].ref_count == 0) {
            return i;
        }
    }
    return -1;
}

// Create or get shared memory
int shmget(unsigned int key, unsigned int size, int flags) {
    int idx = find_shmem_by_key(key);
    
    if (idx >= 0) {
        // Shared memory exists
        g_shmem[idx].ref_count++;
        return idx;
    }
    
    // Create new shared memory
    idx = find_free_shmem();
    if (idx < 0) {
        return -1; // No free slots
    }
    
    // Round up to page size
    size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    // Allocate physical pages
    unsigned int num_pages = size / PAGE_SIZE;
    unsigned long long phys_addr = pmm_alloc_pages(num_pages);
    if (!phys_addr) {
        return -1; // Out of memory
    }
    
    shared_memory_t* shm = &g_shmem[idx];
    shm->key = key;
    shm->size = size;
    shm->physical_addr = (unsigned int)phys_addr;
    shm->virtual_addr = 0; // Will be set when attached
    shm->ref_count = 1;
    
    process_t* proc = process_get_current();
    shm->owner = proc ? proc->pid : 0;
    
    g_shmem_count++;
    return idx;
}

// Attach shared memory
void* shmat(int shmid, const void* shmaddr, int flags) {
    if (shmid < 0 || shmid >= MAX_SHMEM) {
        return (void*)-1;
    }
    
    shared_memory_t* shm = &g_shmem[shmid];
    if (shm->ref_count == 0) {
        return (void*)-1; // Doesn't exist
    }
    
    process_t* proc = process_get_current();
    if (!proc) {
        return (void*)-1;
    }
    
    // Find virtual address in process (simplified - use fixed address)
    unsigned int virt_addr = 0x50000000; // 1.25GB - shared memory region
    
    // Map pages into process address space
    // Save current page directory
    page_directory_t* old_dir = paging_get_directory();
    
    // Switch to process's page directory
    paging_switch_directory(proc->page_dir);
    
    // Map pages
    for (unsigned int i = 0; i < shm->size; i += PAGE_SIZE) {
        unsigned int phys = shm->physical_addr + i;
        if (paging_map_page(virt_addr + i, phys, 
                           PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER) != 0) {
            // Restore old directory on failure
            paging_switch_directory(old_dir);
            return (void*)-1; // Failed to map
        }
    }
    
    // Restore old directory
    paging_switch_directory(old_dir);
    
    shm->virtual_addr = virt_addr;
    return (void*)virt_addr;
}

// Detach shared memory
int shmdt(const void* shmaddr) {
    // Find shared memory by virtual address
    for (int i = 0; i < MAX_SHMEM; i++) {
        if (g_shmem[i].virtual_addr == (unsigned int)shmaddr && 
            g_shmem[i].ref_count > 0) {
            g_shmem[i].ref_count--;
            // Unmap pages (simplified)
            return 0;
        }
    }
    return -1;
}

// Control shared memory
int shmctl(int shmid, int cmd, void* buf) {
    if (shmid < 0 || shmid >= MAX_SHMEM) {
        return -1;
    }
    
    shared_memory_t* shm = &g_shmem[shmid];
    if (shm->ref_count == 0) {
        return -1;
    }
    
    if (cmd == 0) { // IPC_RMID
        shm->ref_count--;
        if (shm->ref_count <= 0) {
            // Free physical pages
            pmm_free_pages(shm->physical_addr, shm->size / PAGE_SIZE);
            shm->key = 0;
            shm->ref_count = 0;
            g_shmem_count--;
        }
        return 0;
    }
    
    return -1;
}

// Register signal handler
int signal_register(int signum, signal_handler_t handler) {
    if (signum < 0 || signum >= MAX_SIGNALS) {
        return -1;
    }
    
    process_t* proc = process_get_current();
    if (!proc) {
        return -1;
    }
    
    // Simplified - use PID as index (not ideal, but works for now)
    int pid_idx = proc->pid % MAX_PIPES;
    g_signal_handlers[pid_idx][signum].handler = handler;
    g_signal_handlers[pid_idx][signum].flags = 0;
    
    return 0;
}

// Send signal to process
int signal_send(pid_t pid, int signum) {
    if (signum < 0 || signum >= MAX_SIGNALS) {
        return -1;
    }
    
    process_t* proc = process_get_by_pid(pid);
    if (!proc) {
        return -1; // Process not found
    }
    
    // In a real OS, we'd queue the signal and handle it on next context switch
    // For now, simplified implementation
    int pid_idx = pid % MAX_PIPES;
    if (g_signal_handlers[pid_idx][signum].handler) {
        // Call handler (simplified - in real OS, this would be done in user space)
        // For now, just mark that signal was sent
        return 0;
    }
    
    // Default action for some signals
    if (signum == SIGKILL || signum == SIGTERM) {
        process_exit(128 + signum);
    }
    
    return 0;
}

// Handle pending signals (called from scheduler/interrupt handler)
void signal_handle(void) {
    process_t* proc = process_get_current();
    if (!proc) {
        return;
    }
    
    // Check for pending signals and call handlers
    // Simplified implementation
}

