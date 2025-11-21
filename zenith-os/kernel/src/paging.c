#include "paging.h"
#include "pmm.h"
#include "memory.h"

// Current page directory
static page_directory_t* g_page_directory = 0;

// Helper: Get page directory index from virtual address
static inline unsigned int get_pde_index(unsigned int addr) {
    return (addr >> 22) & 0x3FF;  // Bits 22-31
}

// Helper: Get page table index from virtual address
static inline unsigned int get_pte_index(unsigned int addr) {
    return (addr >> 12) & 0x3FF;  // Bits 12-21
}

// Helper: Get page-aligned address
static inline unsigned int align_page(unsigned int addr) {
    return addr & ~(PAGE_SIZE - 1);
}

// Initialize paging system
void paging_init(void) {
    // Allocate page directory (must be page-aligned)
    unsigned long long dir_phys = pmm_alloc_page();
    if (!dir_phys) {
        // Failed to allocate - this is critical
        return;
    }
    
    g_page_directory = (page_directory_t*)(dir_phys + 0xC0000000); // Map to higher half (3GB)
    // Actually, we can't use higher half yet since paging isn't enabled
    // So we'll use identity mapping for now
    g_page_directory = (page_directory_t*)dir_phys;
    
    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        g_page_directory->entries[i] = 0;
    }
    
    // Identity map first 4MB (1024 pages) for kernel
    // This includes kernel code, data, and initial page tables
    // First PDE covers 0x00000000-0x003FFFFF (first 4MB)
    unsigned int pde_idx = 0;
    
    // Allocate page table for first 4MB
    unsigned long long table_phys = pmm_alloc_page();
    if (!table_phys) {
        // Critical failure
        return;
    }
    
    page_table_t* page_table = (page_table_t*)table_phys;
    
    // Clear the page table
    for (int j = 0; j < 1024; j++) {
        page_table->entries[j] = 0;
    }
    
    // Identity map all 1024 pages in this table
    for (int j = 0; j < 1024; j++) {
        unsigned int phys_addr = j * PAGE_SIZE;
        page_table->entries[j] = phys_addr | PAGE_PRESENT | PAGE_WRITABLE;
    }
    
    // Set PDE to point to page table
    g_page_directory->entries[pde_idx] = ((unsigned int)table_phys) | PAGE_PRESENT | PAGE_WRITABLE;
    
    // Identity map VGA buffer (0xB8000-0xC0000)
    unsigned int vga_start = 0xB8000;
    unsigned int vga_end = 0xC0000;
    for (unsigned int addr = align_page(vga_start); addr < vga_end; addr += PAGE_SIZE) {
        paging_map_page(addr, addr, PAGE_PRESENT | PAGE_WRITABLE);
    }
    
    // Load page directory into CR3
    paging_load_directory(g_page_directory);
    
    // Enable paging in CR0
    unsigned int cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set PG bit (bit 31)
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

// Get current page directory
page_directory_t* paging_get_directory(void) {
    return g_page_directory;
}

// Map a virtual address to a physical address
int paging_map_page(unsigned int virtual_addr, unsigned int physical_addr, unsigned int flags) {
    if (!g_page_directory) {
        return -1;
    }
    
    unsigned int pde_idx = get_pde_index(virtual_addr);
    unsigned int pte_idx = get_pte_index(virtual_addr);
    
    // Get or create page table
    page_table_t* page_table = 0;
    
    if (!(g_page_directory->entries[pde_idx] & PAGE_PRESENT)) {
        // Allocate a new page table
        unsigned long long table_phys = pmm_alloc_page();
        if (!table_phys) {
            return -1;
        }
        
        page_table = (page_table_t*)table_phys;
        
        // Clear the page table
        for (int i = 0; i < 1024; i++) {
            page_table->entries[i] = 0;
        }
        
        // Set PDE
        g_page_directory->entries[pde_idx] = ((unsigned int)table_phys) | PAGE_PRESENT | PAGE_WRITABLE;
    } else {
        // Get existing page table
        unsigned int table_phys = g_page_directory->entries[pde_idx] & ~0xFFF;
        page_table = (page_table_t*)table_phys;
    }
    
    // Set PTE
    page_table->entries[pte_idx] = (physical_addr & ~0xFFF) | (flags & 0xFFF);
    
    // Invalidate TLB entry
    asm volatile ("invlpg (%0)" : : "r"(virtual_addr) : "memory");
    
    return 0;
}

// Unmap a virtual address
void paging_unmap_page(unsigned int virtual_addr) {
    if (!g_page_directory) {
        return;
    }
    
    unsigned int pde_idx = get_pde_index(virtual_addr);
    unsigned int pte_idx = get_pte_index(virtual_addr);
    
    if (!(g_page_directory->entries[pde_idx] & PAGE_PRESENT)) {
        return; // Not mapped
    }
    
    unsigned int table_phys = g_page_directory->entries[pde_idx] & ~0xFFF;
    page_table_t* page_table = (page_table_t*)table_phys;
    
    // Clear PTE
    page_table->entries[pte_idx] = 0;
    
    // Invalidate TLB entry
    asm volatile ("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

// Map a range of virtual addresses
int paging_map_range(unsigned int virtual_start, unsigned int physical_start, unsigned int size, unsigned int flags) {
    unsigned int virt = align_page(virtual_start);
    unsigned int phys = align_page(physical_start);
    unsigned int end = align_page(virtual_start + size);
    
    while (virt < end) {
        if (paging_map_page(virt, phys, flags) != 0) {
            return -1;
        }
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }
    
    return 0;
}

// Allocate and map a page
unsigned int paging_alloc_page(unsigned int virtual_addr, unsigned int flags) {
    // Allocate physical page
    unsigned long long phys = pmm_alloc_page();
    if (!phys) {
        return 0;
    }
    
    // Map it
    if (paging_map_page(virtual_addr, (unsigned int)phys, flags) != 0) {
        pmm_free_page(phys);
        return 0;
    }
    
    return (unsigned int)phys;
}

// Get physical address from virtual address
unsigned int paging_get_physical(unsigned int virtual_addr) {
    if (!g_page_directory) {
        return 0;
    }
    
    unsigned int pde_idx = get_pde_index(virtual_addr);
    unsigned int pte_idx = get_pte_index(virtual_addr);
    
    if (!(g_page_directory->entries[pde_idx] & PAGE_PRESENT)) {
        return 0; // Not mapped
    }
    
    unsigned int table_phys = g_page_directory->entries[pde_idx] & ~0xFFF;
    page_table_t* page_table = (page_table_t*)table_phys;
    
    if (!(page_table->entries[pte_idx] & PAGE_PRESENT)) {
        return 0; // Not mapped
    }
    
    return (page_table->entries[pte_idx] & ~0xFFF) | (virtual_addr & 0xFFF);
}

// Load page directory into CR3
void paging_load_directory(page_directory_t* dir) {
    asm volatile ("mov %0, %%cr3" : : "r"((unsigned int)dir));
}

// Switch page directory
void paging_switch_directory(page_directory_t* dir) {
    g_page_directory = dir;
    paging_load_directory(dir);
}

