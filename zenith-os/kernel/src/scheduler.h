#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// Initialize scheduler
void scheduler_init(void);

// Get next process to run (round-robin)
process_t* scheduler_get_next(void);

// Schedule a process (add to ready queue)
void scheduler_schedule(process_t* proc);

// Tick handler (called from timer interrupt)
void scheduler_tick(void);

// Get scheduler statistics
unsigned long long scheduler_get_ticks(void);

#endif // SCHEDULER_H

