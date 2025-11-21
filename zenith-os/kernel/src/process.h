#ifndef PROCESS_H
#define PROCESS_H

#include "paging.h"

// Process states
typedef enum {
    PROCESS_STATE_NEW,        // Newly created, not yet started
    PROCESS_STATE_READY,      // Ready to run
    PROCESS_STATE_RUNNING,    // Currently executing
    PROCESS_STATE_BLOCKED,    // Waiting for I/O or event
    PROCESS_STATE_TERMINATED  // Finished execution
} process_state_t;

// Process ID type
typedef unsigned int pid_t;

// CPU registers structure (for context switching)
typedef struct {
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;      // Stack pointer (unused if kernel stack)
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
    
    // Segment registers
    unsigned int gs;
    unsigned int fs;
    unsigned int es;
    unsigned int ds;
    
    // Control registers
    unsigned int eip;      // Instruction pointer
    unsigned int cs;       // Code segment
    unsigned int eflags;   // Flags register
    unsigned int esp_user; // User stack pointer (if switching to user mode)
    unsigned int ss;       // Stack segment
} cpu_registers_t;

// Task Control Block (Process Control Block)
typedef struct process {
    pid_t pid;                      // Process ID
    pid_t ppid;                     // Parent Process ID
    process_state_t state;          // Current state
    unsigned int priority;          // Process priority (0 = highest)
    
    // Memory management
    page_directory_t* page_dir;     // Page directory for this process
    unsigned int stack_top;         // Top of process stack
    unsigned int stack_bottom;      // Bottom of process stack
    unsigned int heap_start;        // Start of heap
    unsigned int heap_end;          // End of heap
    
    // CPU context (registers)
    cpu_registers_t registers;
    
    // Scheduling
    unsigned long long time_slice;   // Remaining time slice
    unsigned long long total_time;   // Total CPU time used
    
    // Process information
    char name[32];                  // Process name
    unsigned int exit_code;         // Exit code (when terminated)
    int exit_status;                 // Exit status (for wait())
    
    // Parent-child relationships
    struct process* parent;          // Parent process
    struct process* first_child;    // First child process
    struct process* next_sibling;   // Next sibling process
    struct process* prev_sibling;   // Previous sibling process
    
    // Linked list
    struct process* next;           // Next process in list
    struct process* prev;            // Previous process in list
} process_t;

// Initialize process management system
void process_init(void);

// Create a new process
// Returns process pointer on success, NULL on failure
process_t* process_create(const char* name, void (*entry_point)(void), unsigned int stack_size);

// Destroy a process
void process_destroy(process_t* proc);

// Get current running process
process_t* process_get_current(void);

// Get process by PID
process_t* process_get_by_pid(pid_t pid);

// Switch to a different process (context switch)
void process_switch(process_t* new_process);

// Yield CPU to next process
void process_yield(void);

// Block current process
void process_block(void);

// Unblock a process
void process_unblock(process_t* proc);

// Exit current process
void process_exit(unsigned int exit_code);

// Get next process to run (scheduler)
process_t* scheduler_get_next(void);

// Schedule a process (add to ready queue)
void scheduler_schedule(process_t* proc);

// Initialize scheduler
void scheduler_init(void);

// Get process count
unsigned int process_get_count(void);

// Fork current process (create a copy)
// Returns child PID in parent, 0 in child, -1 on error
pid_t process_fork(void);

// Execute a new program (replace current process)
// Returns -1 on error, never returns on success
int process_exec(const char* path, char* const argv[]);

// Wait for a child process to exit
// Returns child PID, or -1 on error
pid_t process_wait(int* status);

// User heap management (brk/sbrk)
// Set program break (end of data segment)
// Returns new break address on success, current break on failure
unsigned int process_brk(unsigned int new_break);

// Change program break by increment
// Returns new break address on success, -1 on failure
int process_sbrk(int increment);

#endif // PROCESS_H

