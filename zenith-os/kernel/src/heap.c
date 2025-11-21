#include "heap.h"
#include "paging.h"
#include "memory.h"

// Heap configuration
#define HEAP_START 0xE0000000  // Virtual address where heap starts (3.5GB)
#define HEAP_INITIAL_SIZE (1024 * 1024)  // Initial heap size: 1MB
#define HEAP_GROW_SIZE (1024 * 1024)     // Grow heap by 1MB at a time
#define MIN_BLOCK_SIZE 16                 // Minimum allocation size
#define ALIGNMENT 8                        // Alignment requirement

// Heap state
static heap_block_t* g_heap_start = 0;
static unsigned int g_heap_size = 0;
static unsigned int g_heap_used = 0;
static unsigned int g_heap_initialized = 0;

// Align a size to ALIGNMENT boundary
static unsigned int align_size(unsigned int size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

// Get the header for a block
static heap_block_t* get_block_header(void* ptr) {
    if (!ptr) return 0;
    return (heap_block_t*)((unsigned char*)ptr - sizeof(heap_block_t));
}

// Get the data pointer for a block
static void* get_block_data(heap_block_t* block) {
    if (!block) return 0;
    return (unsigned char*)block + sizeof(heap_block_t);
}

// Find a free block that can fit the requested size
static heap_block_t* find_free_block(unsigned int size) {
    if (!g_heap_start || g_heap_size == 0) {
        return 0;
    }
    
    heap_block_t* current = g_heap_start;
    unsigned int heap_end = (unsigned int)g_heap_start + g_heap_size;
    
    while ((unsigned int)current < heap_end) {
        // Validate block header
        if (current->size > g_heap_size) {
            break; // Invalid block
        }
        
        if (!current->used && current->size >= size) {
            return current;
        }
        
        // Move to next block
        unsigned int next_addr = (unsigned int)current + sizeof(heap_block_t) + current->size;
        if (next_addr >= heap_end) {
            break;
        }
        current = (heap_block_t*)next_addr;
    }
    
    return 0;
}

// Split a block if it's too large
static void split_block(heap_block_t* block, unsigned int size) {
    if (!block || block->used) return;
    
    unsigned int remaining = block->size - size;
    if (remaining >= sizeof(heap_block_t) + MIN_BLOCK_SIZE) {
        // Create a new free block from the remainder
        heap_block_t* new_block = (heap_block_t*)((unsigned char*)block + sizeof(heap_block_t) + size);
        new_block->size = remaining - sizeof(heap_block_t);
        new_block->used = 0;
        new_block->next = 0;
        
        // Update original block
        block->size = size;
    }
}

// Merge adjacent free blocks
static void merge_free_blocks(void) {
    if (!g_heap_start || g_heap_size == 0) {
        return;
    }
    
    // Keep merging until no more merges are possible
    int merged = 1;
    while (merged) {
        merged = 0;
        heap_block_t* current = g_heap_start;
        unsigned int heap_end = (unsigned int)g_heap_start + g_heap_size;
        
        while ((unsigned int)current < heap_end) {
            if (!current->used) {
                // Check if next block is adjacent and free
                unsigned int next_addr = (unsigned int)current + sizeof(heap_block_t) + current->size;
                if (next_addr < heap_end) {
                    heap_block_t* next = (heap_block_t*)next_addr;
                    if (!next->used) {
                        // Merge blocks
                        current->size += sizeof(heap_block_t) + next->size;
                        merged = 1;
                        continue; // Check this block again for further merges
                    }
                }
            }
            
            // Move to next block
            unsigned int next_addr = (unsigned int)current + sizeof(heap_block_t) + current->size;
            if (next_addr >= heap_end) {
                break;
            }
            current = (heap_block_t*)next_addr;
        }
    }
}

// Grow the heap
static int grow_heap(unsigned int size) {
    // Calculate how many pages we need
    unsigned int pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (pages_needed == 0) pages_needed = 1;
    
    // Allocate physical pages
    unsigned long long physical_addr = pmm_alloc_pages(pages_needed);
    if (!physical_addr) {
        return -1; // Out of memory
    }
    
    // Map virtual pages
    unsigned int virtual_addr = HEAP_START + g_heap_size;
    for (unsigned int i = 0; i < pages_needed; i++) {
        if (paging_map_page(virtual_addr + i * PAGE_SIZE, 
                           (unsigned int)physical_addr + i * PAGE_SIZE,
                           PAGE_PRESENT | PAGE_WRITABLE) != 0) {
            // Failed to map, free allocated pages
            pmm_free_pages(physical_addr, pages_needed);
            return -1;
        }
    }
    
    // Initialize new heap space
    if (g_heap_size == 0) {
        // First allocation - initialize first block
        g_heap_start = (heap_block_t*)HEAP_START;
        g_heap_start->size = pages_needed * PAGE_SIZE - sizeof(heap_block_t);
        g_heap_start->used = 0;
        g_heap_start->next = 0;
    } else {
        // Add new free block at the end
        heap_block_t* new_block = (heap_block_t*)(HEAP_START + g_heap_size);
        new_block->size = pages_needed * PAGE_SIZE - sizeof(heap_block_t);
        new_block->used = 0;
        new_block->next = 0;
        
        // Try to merge with previous block
        merge_free_blocks();
    }
    
    g_heap_size += pages_needed * PAGE_SIZE;
    return 0;
}

// Initialize kernel heap
int heap_init(void) {
    if (g_heap_initialized) {
        return 0; // Already initialized
    }
    
    // Grow heap to initial size
    if (grow_heap(HEAP_INITIAL_SIZE) != 0) {
        return -1;
    }
    
    g_heap_initialized = 1;
    return 0;
}

// Allocate memory from kernel heap
void* kmalloc(unsigned int size) {
    if (!g_heap_initialized) {
        if (heap_init() != 0) {
            return 0; // Failed to initialize
        }
    }
    
    if (size == 0) {
        return 0;
    }
    
    // Align size
    size = align_size(size);
    if (size < MIN_BLOCK_SIZE) {
        size = MIN_BLOCK_SIZE;
    }
    
    // Find a free block
    heap_block_t* block = find_free_block(size);
    
    if (!block) {
        // No free block found, try to grow heap
        if (grow_heap(HEAP_GROW_SIZE) != 0) {
            return 0; // Out of memory
        }
        
        // Try again
        block = find_free_block(size);
        if (!block) {
            return 0; // Still no block found
        }
    }
    
    // Split block if necessary
    split_block(block, size);
    
    // Mark as used
    block->used = 1;
    g_heap_used += block->size + sizeof(heap_block_t);
    
    return get_block_data(block);
}

// Free memory allocated from kernel heap
void kfree(void* ptr) {
    if (!ptr || !g_heap_initialized) {
        return;
    }
    
    heap_block_t* block = get_block_header(ptr);
    
    // Validate block
    if ((unsigned int)block < HEAP_START || 
        (unsigned int)block >= HEAP_START + g_heap_size) {
        return; // Invalid pointer
    }
    
    if (!block->used) {
        return; // Already free (double free)
    }
    
    // Mark as free
    block->used = 0;
    g_heap_used -= block->size + sizeof(heap_block_t);
    
    // Try to merge with adjacent free blocks
    merge_free_blocks();
}

// Reallocate memory
void* krealloc(void* ptr, unsigned int size) {
    if (!ptr) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return 0;
    }
    
    heap_block_t* block = get_block_header(ptr);
    
    // Validate block
    if ((unsigned int)block < HEAP_START || 
        (unsigned int)block >= HEAP_START + g_heap_size) {
        return 0; // Invalid pointer
    }
    
    size = align_size(size);
    
    // If new size is smaller or same, just return same pointer
    if (size <= block->size) {
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = kmalloc(size);
    if (!new_ptr) {
        return 0;
    }
    
    // Copy old data
    unsigned int copy_size = block->size;
    if (copy_size > size) {
        copy_size = size;
    }
    
    unsigned char* src = (unsigned char*)ptr;
    unsigned char* dst = (unsigned char*)new_ptr;
    for (unsigned int i = 0; i < copy_size; i++) {
        dst[i] = src[i];
    }
    
    // Free old block
    kfree(ptr);
    
    return new_ptr;
}

// Get heap statistics
unsigned int heap_get_total_size(void) {
    return g_heap_size;
}

unsigned int heap_get_used_size(void) {
    return g_heap_used;
}

unsigned int heap_get_free_size(void) {
    return g_heap_size - g_heap_used;
}

