#include "scheduler.h"
#include "process.h"
#include "timer.h"

// Access to process list (needed by scheduler)
extern process_t* g_process_list;

// Scheduler state
static unsigned long long g_scheduler_ticks = 0;
static unsigned int g_time_quantum = 10; // 10 ticks = 100ms at 100Hz

// Initialize scheduler
void scheduler_init(void) {
    g_scheduler_ticks = 0;
}

// Get next process to run (round-robin)
process_t* scheduler_get_next(void) {
    process_t* current = process_get_current();
    process_t* start = current;
    process_t* next = 0;
    
    // If no current process, find first ready process
    if (!current) {
        process_t* proc = g_process_list;
        while (proc) {
            if (proc->state == PROCESS_STATE_READY) {
                return proc;
            }
            proc = proc->next;
        }
        return 0;
    }
    
    // Round-robin: find next ready process
    next = current->next;
    if (!next) {
        next = g_process_list; // Wrap around
    }
    
    // Search for next ready process
    while (next != start) {
        if (next && next->state == PROCESS_STATE_READY) {
            return next;
        }
        next = next->next;
        if (!next) {
            next = g_process_list; // Wrap around
        }
    }
    
    // If we get here, current process is the only ready one
    if (current->state == PROCESS_STATE_READY) {
        return current;
    }
    
    // No ready processes
    return 0;
}

// Schedule a process (add to ready queue)
void scheduler_schedule(process_t* proc) {
    if (proc) {
        proc->state = PROCESS_STATE_READY;
        proc->time_slice = g_time_quantum;
    }
}

// Tick handler (called from timer interrupt)
void scheduler_tick(void) {
    g_scheduler_ticks++;
    
    process_t* current = process_get_current();
    
    if (current) {
        // Decrement time slice
        if (current->time_slice > 0) {
            current->time_slice--;
        }
        current->total_time++;
        
        // Time slice expired - yield
        if (current->time_slice == 0) {
            process_yield();
        }
    } else {
        // No current process - try to schedule one
        process_t* next = scheduler_get_next();
        if (next) {
            process_switch(next);
        }
    }
}

// Get scheduler statistics
unsigned long long scheduler_get_ticks(void) {
    return g_scheduler_ticks;
}

