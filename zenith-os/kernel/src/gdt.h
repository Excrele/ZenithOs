#ifndef GDT_H
#define GDT_H

// GDT segment selectors
#define GDT_NULL    0x00
#define GDT_KERNEL_CS  0x08  // Kernel code segment
#define GDT_KERNEL_DS  0x10  // Kernel data segment
#define GDT_USER_CS    0x18  // User code segment (Ring 3)
#define GDT_USER_DS    0x20  // User data segment (Ring 3)

// Initialize GDT with kernel and user segments
void gdt_init(void);

// Reload GDT (call after modifying)
void gdt_reload(void);

#endif // GDT_H

