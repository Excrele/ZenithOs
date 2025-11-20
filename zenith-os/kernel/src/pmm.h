#ifndef PMM_H
#define PMM_H

#include "memory.h"

// Page size: 4KB (standard)
#define PAGE_SIZE 4096
#define PAGE_SIZE_SHIFT 12

// Bitmap: 0 = free, 1 = used/reserved

// Physical Memory Manager structure
typedef struct {
    unsigned char* bitmap;        // Bitmap array (1 bit per page)
    unsigned long long bitmap_size; // Size of bitmap in bytes
    unsigned long long total_pages; // Total number of pages
    unsigned long long free_pages;  // Number of free pages
    unsigned long long used_pages; // Number of used pages
    unsigned long long mem_start;   // Start of managed memory
    unsigned long long mem_end;     // End of managed memory
} pmm_t;

// Initialize Physical Memory Manager
// Returns 0 on success, -1 on failure
int pmm_init(memory_map_t* mem_map);

// Allocate a single page frame (4KB)
// Returns physical address of allocated page, or 0 on failure
unsigned long long pmm_alloc_page(void);

// Allocate multiple contiguous page frames
// Returns physical address of first page, or 0 on failure
unsigned long long pmm_alloc_pages(unsigned int count);

// Free a page frame
// addr: physical address of page to free
void pmm_free_page(unsigned long long addr);

// Free multiple contiguous page frames
void pmm_free_pages(unsigned long long addr, unsigned int count);

// Mark a page as reserved (cannot be allocated)
void pmm_mark_reserved(unsigned long long addr);

// Mark a range of pages as reserved
void pmm_mark_reserved_range(unsigned long long start, unsigned long long end);

// Get statistics
unsigned long long pmm_get_free_pages(void);
unsigned long long pmm_get_used_pages(void);
unsigned long long pmm_get_total_pages(void);

#endif // PMM_H

