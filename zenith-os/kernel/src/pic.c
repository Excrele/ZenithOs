#include "pic.h"

// Helper function to output byte to port
static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Helper function to input byte from port
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Remap PIC to interrupts 32-47
void pic_remap(void) {
    unsigned char a1, a2;
    
    // Save masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
    
    // Start initialization sequence
    outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    
    // ICW2: Set interrupt vector offsets
    // Master PIC: interrupts 32-39 (0x20-0x27)
    // Slave PIC: interrupts 40-47 (0x28-0x2F)
    outb(PIC1_DATA, 0x20);  // Master PIC offset
    outb(PIC2_DATA, 0x28);   // Slave PIC offset
    
    // ICW3: Tell master PIC about slave at IRQ2
    outb(PIC1_DATA, 0x04);   // Slave at IRQ2 (bit 2 set)
    outb(PIC2_DATA, 0x02);   // Slave cascade identity
    
    // ICW4: Set 8086 mode
    outb(PIC1_DATA, PIC_ICW4_8086);
    outb(PIC2_DATA, PIC_ICW4_8086);
    
    // Restore masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

// Send End of Interrupt to PIC
void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// Enable specific IRQ (clear mask bit)
void pic_enable_irq(unsigned char irq) {
    unsigned short port;
    unsigned char value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// Disable specific IRQ (set mask bit)
void pic_disable_irq(unsigned char irq) {
    unsigned short port;
    unsigned char value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) | (1 << irq);
    outb(port, value);
}

// Disable all PIC interrupts
void pic_disable(void) {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

