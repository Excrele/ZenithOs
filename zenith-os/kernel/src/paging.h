#ifndef PAGING_H
#define PAGING_H

#include "pmm.h"

// Page size: 4KB
#define PAGE_SIZE 4096
#define PAGE_SIZE_SHIFT 12

// Page directory and page table entry flags
#define PAGE_PRESENT        0x001   // Page is present in memory
#define PAGE_WRITABLE       0x002   // Page is writable
#define PAGE_USER           0x004   // User mode accessible
#define PAGE_WRITE_THROUGH  0x008   // Write-through caching
#define PAGE_CACHE_DISABLE  0x010   // Cache disabled
#define PAGE_ACCESSED       0x020   // Page has been accessed
#define PAGE_DIRTY          0x040   // Page has been written to
#define PAGE_SIZE_4MB       0x080   // 4MB page (for PDE only)
#define PAGE_GLOBAL         0x100   // Global page (TLB)

// Page Directory Entry (PDE) - points to a page table
typedef unsigned int pde_t;

// Page Table Entry (PTE) - points to a physical page
typedef unsigned int pte_t;

// Page Directory - array of 1024 PDEs (4KB)
typedef struct {
    pde_t entries[1024];
} page_directory_t;

// Page Table - array of 1024 PTEs (4KB)
typedef struct {
    pte_t entries[1024];
} page_table_t;

// Initialize paging system
void paging_init(void);

// Get current page directory
page_directory_t* paging_get_directory(void);

// Map a virtual address to a physical address
// Returns 0 on success, -1 on failure
int paging_map_page(unsigned int virtual_addr, unsigned int physical_addr, unsigned int flags);

// Unmap a virtual address
void paging_unmap_page(unsigned int virtual_addr);

// Map a range of virtual addresses to physical addresses
int paging_map_range(unsigned int virtual_start, unsigned int physical_start, unsigned int size, unsigned int flags);

// Allocate and map a page (allocates physical page and maps it)
unsigned int paging_alloc_page(unsigned int virtual_addr, unsigned int flags);

// Get physical address from virtual address (if mapped)
unsigned int paging_get_physical(unsigned int virtual_addr);

// Switch page directory (for process switching later)
void paging_switch_directory(page_directory_t* dir);

// Load page directory into CR3
void paging_load_directory(page_directory_t* dir);

#endif // PAGING_H

