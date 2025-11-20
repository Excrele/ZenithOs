#include "pmm.h"

// Global PMM instance
static pmm_t g_pmm;

// Helper: Get page index from physical address
static inline unsigned long long addr_to_page_index(unsigned long long addr) {
    return (addr - g_pmm.mem_start) >> PAGE_SIZE_SHIFT;
}

// Helper: Get physical address from page index
static inline unsigned long long page_index_to_addr(unsigned long long index) {
    return g_pmm.mem_start + (index << PAGE_SIZE_SHIFT);
}

// Helper: Get bitmap byte and bit for a page
static inline void get_bitmap_pos(unsigned long long page_index, 
                                  unsigned long long* byte_index, 
                                  unsigned char* bit_mask) {
    *byte_index = page_index / 8;
    *bit_mask = 1 << (page_index % 8);
}

// Helper: Check if bitmap bit is set (used/reserved)
static inline int is_bitmap_set(unsigned long long page_index) {
    unsigned long long byte_index;
    unsigned char bit_mask;
    get_bitmap_pos(page_index, &byte_index, &bit_mask);
    
    if (byte_index >= g_pmm.bitmap_size) {
        return 1; // Out of range = considered used/reserved
    }
    
    return (g_pmm.bitmap[byte_index] & bit_mask) != 0;
}

// Helper: Set bitmap bit for a page
static inline void set_bitmap_bit(unsigned long long page_index, unsigned char value) {
    unsigned long long byte_index;
    unsigned char bit_mask;
    get_bitmap_pos(page_index, &byte_index, &bit_mask);
    
    if (byte_index >= g_pmm.bitmap_size) {
        return; // Out of range
    }
    
    if (value) {
        g_pmm.bitmap[byte_index] |= bit_mask;
    } else {
        g_pmm.bitmap[byte_index] &= ~bit_mask;
    }
}

// Initialize Physical Memory Manager
int pmm_init(memory_map_t* mem_map) {
    if (!mem_map || mem_map->count == 0) {
        return -1;
    }
    
    // Find the highest usable memory address
    unsigned long long max_mem = 0;
    unsigned long long min_mem = 0xFFFFFFFFFFFFFFFFULL;
    
    for (unsigned int i = 0; i < mem_map->count; i++) {
        memory_map_entry_t* entry = &mem_map->entries[i];
        if (entry->type == MEMORY_TYPE_USABLE) {
            unsigned long long end = entry->base + entry->length;
            if (end > max_mem) {
                max_mem = end;
            }
            if (entry->base < min_mem) {
                min_mem = entry->base;
            }
        }
    }
    
    if (max_mem == 0) {
        return -1; // No usable memory found
    }
    
    // Round to page boundaries
    g_pmm.mem_start = (min_mem + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    g_pmm.mem_end = max_mem & ~(PAGE_SIZE - 1);
    
    // Calculate total pages
    g_pmm.total_pages = (g_pmm.mem_end - g_pmm.mem_start) >> PAGE_SIZE_SHIFT;
    
    // Calculate bitmap size (1 bit per page, rounded up)
    g_pmm.bitmap_size = (g_pmm.total_pages + 7) / 8;
    
    // Find a location for the bitmap in usable memory
    // Try to place it at 0x200000 (2MB) first, then find any suitable location
    unsigned long long bitmap_addr = 0x200000;
    g_pmm.bitmap = 0;
    
    // Check if 0x200000 fits in available memory
    for (unsigned int i = 0; i < mem_map->count; i++) {
        memory_map_entry_t* entry = &mem_map->entries[i];
        if (entry->type == MEMORY_TYPE_USABLE) {
            if (entry->base <= bitmap_addr && 
                (entry->base + entry->length) >= (bitmap_addr + g_pmm.bitmap_size)) {
                g_pmm.bitmap = (unsigned char*)bitmap_addr;
                break;
            }
        }
    }
    
    // If we didn't find a suitable location at 0x200000, find any large enough region
    if (!g_pmm.bitmap) {
        for (unsigned int i = 0; i < mem_map->count; i++) {
            memory_map_entry_t* entry = &mem_map->entries[i];
            if (entry->type == MEMORY_TYPE_USABLE && entry->length >= g_pmm.bitmap_size) {
                // Use start of this region, rounded up to page boundary
                unsigned long long candidate = (entry->base + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
                // Make sure it's above 1MB and doesn't overlap with kernel
                if (candidate >= 0x200000 && candidate + g_pmm.bitmap_size <= entry->base + entry->length) {
                    g_pmm.bitmap = (unsigned char*)candidate;
                    break;
                }
            }
        }
    }
    
    if (!g_pmm.bitmap) {
        return -1; // Could not find space for bitmap
    }
    
    // Initialize bitmap to all free (zero)
    for (unsigned long long i = 0; i < g_pmm.bitmap_size; i++) {
        g_pmm.bitmap[i] = 0;
    }
    
    // Mark all pages as free initially
    g_pmm.free_pages = g_pmm.total_pages;
    g_pmm.used_pages = 0;
    
    // Mark reserved regions from memory map
    for (unsigned int i = 0; i < mem_map->count; i++) {
        memory_map_entry_t* entry = &mem_map->entries[i];
        if (entry->type != MEMORY_TYPE_USABLE) {
            // Mark this entire region as reserved
            unsigned long long start = entry->base & ~(PAGE_SIZE - 1);
            unsigned long long end = (entry->base + entry->length + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            pmm_mark_reserved_range(start, end);
        }
    }
    
    // Mark kernel memory as reserved (0x100000 to 0x200000, 1MB)
    pmm_mark_reserved_range(0x100000, 0x200000);
    
    // Mark memory map location as reserved (0x80000)
    pmm_mark_reserved_range(0x80000, 0x90000);
    
    // Mark bootloader regions as reserved (0x7c00-0x90000)
    pmm_mark_reserved_range(0x7c00, 0x90000);
    
    // Mark VGA buffer as reserved (0xb8000-0xc0000)
    pmm_mark_reserved_range(0xb8000, 0xc0000);
    
    // Mark bitmap itself as reserved
    unsigned long long bitmap_start = (unsigned long long)g_pmm.bitmap;
    unsigned long long bitmap_end = bitmap_start + g_pmm.bitmap_size;
    bitmap_end = (bitmap_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1); // Round up
    pmm_mark_reserved_range(bitmap_start, bitmap_end);
    
    return 0;
}

// Mark a range of pages as reserved
void pmm_mark_reserved_range(unsigned long long start, unsigned long long end) {
    // Only mark pages that are within our managed range
    if (end <= g_pmm.mem_start || start >= g_pmm.mem_end) {
        return; // Completely outside managed range
    }
    
    // Clamp to managed range
    if (start < g_pmm.mem_start) {
        start = g_pmm.mem_start;
    }
    if (end > g_pmm.mem_end) {
        end = g_pmm.mem_end;
    }
    
    unsigned long long start_page = addr_to_page_index(start);
    unsigned long long end_page = addr_to_page_index(end);
    
    for (unsigned long long i = start_page; i < end_page; i++) {
        if (!is_bitmap_set(i)) {
            set_bitmap_bit(i, 1); // Mark as used/reserved
            if (g_pmm.free_pages > 0) {
                g_pmm.free_pages--;
            }
        }
    }
}

// Mark a single page as reserved
void pmm_mark_reserved(unsigned long long addr) {
    pmm_mark_reserved_range(addr, addr + PAGE_SIZE);
}

// Allocate a single page frame
unsigned long long pmm_alloc_page(void) {
    return pmm_alloc_pages(1);
}

// Allocate multiple contiguous page frames
unsigned long long pmm_alloc_pages(unsigned int count) {
    if (count == 0 || count > g_pmm.free_pages) {
        return 0; // Invalid request or not enough free pages
    }
    
    // Simple first-fit allocation
    unsigned long long consecutive = 0;
    unsigned long long start_page = 0;
    
    for (unsigned long long i = 0; i < g_pmm.total_pages; i++) {
        if (!is_bitmap_set(i)) { // Page is free
            if (consecutive == 0) {
                start_page = i;
            }
            consecutive++;
            
            if (consecutive >= count) {
                // Found enough consecutive pages
                for (unsigned long long j = start_page; j < start_page + count; j++) {
                    set_bitmap_bit(j, 1); // Mark as used
                }
                g_pmm.free_pages -= count;
                g_pmm.used_pages += count;
                return page_index_to_addr(start_page);
            }
        } else {
            consecutive = 0;
        }
    }
    
    return 0; // Not enough contiguous pages found
}

// Free a single page frame
void pmm_free_page(unsigned long long addr) {
    pmm_free_pages(addr, 1);
}

// Free multiple contiguous page frames
void pmm_free_pages(unsigned long long addr, unsigned int count) {
    if (addr < g_pmm.mem_start || addr >= g_pmm.mem_end) {
        return; // Invalid address
    }
    
    unsigned long long start_page = addr_to_page_index(addr);
    
    for (unsigned int i = 0; i < count; i++) {
        unsigned long long page_index = start_page + i;
        if (page_index >= g_pmm.total_pages) {
            break;
        }
        
        if (is_bitmap_set(page_index)) {
            set_bitmap_bit(page_index, 0); // Mark as free
            g_pmm.free_pages++;
            if (g_pmm.used_pages > 0) {
                g_pmm.used_pages--;
            }
        }
    }
}

// Get statistics
unsigned long long pmm_get_free_pages(void) {
    return g_pmm.free_pages;
}

unsigned long long pmm_get_used_pages(void) {
    return g_pmm.used_pages;
}

unsigned long long pmm_get_total_pages(void) {
    return g_pmm.total_pages;
}

