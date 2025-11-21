#ifndef HEAP_H
#define HEAP_H

#include "pmm.h"

// Heap block header structure
typedef struct heap_block {
    struct heap_block* next;  // Next block in free list
    unsigned int size;        // Size of this block (excluding header)
    unsigned int used;        // 1 if used, 0 if free
} heap_block_t;

// Initialize kernel heap
// Returns 0 on success, -1 on failure
int heap_init(void);

// Allocate memory from kernel heap
// size: number of bytes to allocate
// Returns pointer to allocated memory, or NULL on failure
void* kmalloc(unsigned int size);

// Free memory allocated from kernel heap
// ptr: pointer to memory to free
void kfree(void* ptr);

// Reallocate memory (grow or shrink)
// ptr: pointer to existing allocation
// size: new size
// Returns pointer to reallocated memory, or NULL on failure
void* krealloc(void* ptr, unsigned int size);

// Get heap statistics
unsigned int heap_get_total_size(void);
unsigned int heap_get_used_size(void);
unsigned int heap_get_free_size(void);

#endif // HEAP_H

