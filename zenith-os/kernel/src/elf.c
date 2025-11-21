#include "elf.h"
#include "paging.h"
#include "pmm.h"
#include "memory.h"

// Load an ELF executable
unsigned int elf_load(void* elf_data, unsigned int size) {
    if (!elf_data || size < sizeof(elf_header_t)) {
        return 0;
    }
    
    elf_header_t* header = (elf_header_t*)elf_data;
    
    // Check ELF magic number
    if (*(unsigned int*)header->e_ident != ELF_MAGIC) {
        return 0; // Not an ELF file
    }
    
    // Check architecture (i386 = 3)
    if (header->e_ident[4] != 1 || header->e_ident[5] != 1) {
        return 0; // Not 32-bit little-endian
    }
    
    // Check type (executable = 2)
    if (header->e_type != 2) {
        return 0; // Not an executable
    }
    
    // Check machine (i386 = 3)
    if (header->e_machine != 3) {
        return 0; // Not i386
    }
    
    // Load program segments
    elf_program_header_t* phdr = (elf_program_header_t*)((unsigned char*)elf_data + header->e_phoff);
    
    for (int i = 0; i < header->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            // Allocate and map pages for this segment
            unsigned int vaddr = phdr[i].p_vaddr;
            unsigned int memsz = phdr[i].p_memsz;
            unsigned int filesz = phdr[i].p_filesz;
            
            // Align to page boundaries
            unsigned int start_page = vaddr & ~(PAGE_SIZE - 1);
            unsigned int end_page = (vaddr + memsz + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            unsigned int num_pages = (end_page - start_page) / PAGE_SIZE;
            
            // Allocate and map pages
            for (unsigned int j = 0; j < num_pages; j++) {
                unsigned int page_vaddr = start_page + (j * PAGE_SIZE);
                unsigned long long page_phys = pmm_alloc_page();
                
                if (!page_phys) {
                    return 0; // Out of memory
                }
                
                // Map page (user-accessible)
                paging_map_page(page_vaddr, (unsigned int)page_phys, 
                               PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
                
                // Clear page
                unsigned char* page = (unsigned char*)page_vaddr;
                for (unsigned int k = 0; k < PAGE_SIZE; k++) {
                    page[k] = 0;
                }
            }
            
            // Copy segment data
            unsigned char* src = (unsigned char*)elf_data + phdr[i].p_offset;
            unsigned char* dst = (unsigned char*)vaddr;
            
            for (unsigned int j = 0; j < filesz; j++) {
                dst[j] = src[j];
            }
        }
    }
    
    // Return entry point
    return header->e_entry;
}

