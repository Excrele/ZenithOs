#include "idt.h"

// IDT with 256 entries
static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

// Array of interrupt handler function pointers
static interrupt_handler_t interrupt_handlers[256];

// External assembly functions for exception handlers
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);

// External assembly functions for IRQ handlers
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

// Common interrupt handler stub (called from assembly)
void interrupt_handler_common(unsigned int interrupt_num) {
    // Call registered handler if available
    if (interrupt_handlers[interrupt_num]) {
        interrupt_handlers[interrupt_num]();
    }
    
    // If it's an IRQ (32-47), send EOI to PIC
    if (interrupt_num >= 32 && interrupt_num < 48) {
        // Send EOI to appropriate PIC
        if (interrupt_num >= 40) {
            // Slave PIC
            outb(0xA0, 0x20);
        }
        // Master PIC
        outb(0x20, 0x20);
    }
}

// Helper function to output byte to port
static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Set an IDT entry
void idt_set_entry(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Register an interrupt handler
void idt_register_handler(unsigned char interrupt, interrupt_handler_t handler) {
    interrupt_handlers[interrupt] = handler;
}

// Initialize IDT
void idt_init(void) {
    // Initialize IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (unsigned int)&idt;
    
    // Clear all handlers
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }
    
    // Set up exception handlers (0-31)
    idt_set_entry(0, (unsigned long)isr0, 0x08, 0x8E); // Present, ring 0, interrupt gate
    idt_set_entry(1, (unsigned long)isr1, 0x08, 0x8E);
    idt_set_entry(2, (unsigned long)isr2, 0x08, 0x8E);
    idt_set_entry(3, (unsigned long)isr3, 0x08, 0x8E);
    idt_set_entry(4, (unsigned long)isr4, 0x08, 0x8E);
    idt_set_entry(5, (unsigned long)isr5, 0x08, 0x8E);
    idt_set_entry(6, (unsigned long)isr6, 0x08, 0x8E);
    idt_set_entry(7, (unsigned long)isr7, 0x08, 0x8E);
    idt_set_entry(8, (unsigned long)isr8, 0x08, 0x8E);
    idt_set_entry(9, (unsigned long)isr9, 0x08, 0x8E);
    idt_set_entry(10, (unsigned long)isr10, 0x08, 0x8E);
    idt_set_entry(11, (unsigned long)isr11, 0x08, 0x8E);
    idt_set_entry(12, (unsigned long)isr12, 0x08, 0x8E);
    idt_set_entry(13, (unsigned long)isr13, 0x08, 0x8E);
    idt_set_entry(14, (unsigned long)isr14, 0x08, 0x8E);
    idt_set_entry(15, (unsigned long)isr15, 0x08, 0x8E);
    idt_set_entry(16, (unsigned long)isr16, 0x08, 0x8E);
    idt_set_entry(17, (unsigned long)isr17, 0x08, 0x8E);
    idt_set_entry(18, (unsigned long)isr18, 0x08, 0x8E);
    idt_set_entry(19, (unsigned long)isr19, 0x08, 0x8E);
    idt_set_entry(20, (unsigned long)isr20, 0x08, 0x8E);
    
    // Set up system call interrupt (INT 0x80)
    extern void syscall_interrupt(void);
    idt_set_entry(0x80, (unsigned long)syscall_interrupt, 0x08, 0xEE); // User-accessible interrupt gate
    
    // Set up IRQ handlers (32-47)
    idt_set_entry(32, (unsigned long)irq0, 0x08, 0x8E);
    idt_set_entry(33, (unsigned long)irq1, 0x08, 0x8E);
    idt_set_entry(34, (unsigned long)irq2, 0x08, 0x8E);
    idt_set_entry(35, (unsigned long)irq3, 0x08, 0x8E);
    idt_set_entry(36, (unsigned long)irq4, 0x08, 0x8E);
    idt_set_entry(37, (unsigned long)irq5, 0x08, 0x8E);
    idt_set_entry(38, (unsigned long)irq6, 0x08, 0x8E);
    idt_set_entry(39, (unsigned long)irq7, 0x08, 0x8E);
    idt_set_entry(40, (unsigned long)irq8, 0x08, 0x8E);
    idt_set_entry(41, (unsigned long)irq9, 0x08, 0x8E);
    idt_set_entry(42, (unsigned long)irq10, 0x08, 0x8E);
    idt_set_entry(43, (unsigned long)irq11, 0x08, 0x8E);
    idt_set_entry(44, (unsigned long)irq12, 0x08, 0x8E);
    idt_set_entry(45, (unsigned long)irq13, 0x08, 0x8E);
    idt_set_entry(46, (unsigned long)irq14, 0x08, 0x8E);
    idt_set_entry(47, (unsigned long)irq15, 0x08, 0x8E);
    
    // Load IDT
    asm volatile ("lidt %0" : : "m"(idt_ptr));
}

