#include "gdt.h"

// GDT entry structure
typedef struct {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed)) gdt_entry_t;

// GDT pointer structure
typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) gdt_ptr_t;

// GDT with 5 entries: Null, Kernel CS, Kernel DS, User CS, User DS
static gdt_entry_t gdt[5];
static gdt_ptr_t gdt_ptr;

// Set a GDT entry
static void gdt_set_entry(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    
    gdt[num].access = access;
}

// Initialize GDT
void gdt_init(void) {
    // Set up GDT pointer
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (unsigned int)&gdt;
    
    // Null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);
    
    // Kernel code segment (Ring 0)
    // Base: 0x00000000, Limit: 0xFFFFFFFF, Access: 0x9A (present, ring 0, code, readable)
    // Granularity: 0xCF (4KB pages, 32-bit)
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Kernel data segment (Ring 0)
    // Base: 0x00000000, Limit: 0xFFFFFFFF, Access: 0x92 (present, ring 0, data, writable)
    // Granularity: 0xCF
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // User code segment (Ring 3)
    // Base: 0x00000000, Limit: 0xFFFFFFFF, Access: 0xFA (present, ring 3, code, readable)
    // Granularity: 0xCF
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // User data segment (Ring 3)
    // Base: 0x00000000, Limit: 0xFFFFFFFF, Access: 0xF2 (present, ring 3, data, writable)
    // Granularity: 0xCF
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    // Load GDT
    gdt_reload();
}

// Reload GDT
void gdt_reload(void) {
    asm volatile ("lgdt %0" : : "m"(gdt_ptr));
    
    // Reload segment registers
    asm volatile (
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        :
        : "ax"
    );
}

