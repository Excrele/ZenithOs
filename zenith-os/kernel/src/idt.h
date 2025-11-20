#ifndef IDT_H
#define IDT_H

// IDT entry structure (8 bytes)
typedef struct {
    unsigned short base_low;      // Lower 16 bits of ISR address
    unsigned short selector;      // Code segment selector
    unsigned char zero;           // Always zero
    unsigned char flags;          // Type and attributes
    unsigned short base_high;     // Higher 16 bits of ISR address
} __attribute__((packed)) idt_entry_t;

// IDT pointer structure (for LIDT instruction)
typedef struct {
    unsigned short limit;         // Size of IDT - 1
    unsigned int base;            // Base address of IDT
} __attribute__((packed)) idt_ptr_t;

// Exception numbers
#define EXCEPTION_DIVIDE_BY_ZERO      0
#define EXCEPTION_DEBUG               1
#define EXCEPTION_NMI                2
#define EXCEPTION_BREAKPOINT          3
#define EXCEPTION_OVERFLOW            4
#define EXCEPTION_BOUND_RANGE         5
#define EXCEPTION_INVALID_OPCODE      6
#define EXCEPTION_DEVICE_NOT_AVAIL    7
#define EXCEPTION_DOUBLE_FAULT        8
#define EXCEPTION_COPROC_SEG_OVERRUN  9
#define EXCEPTION_INVALID_TSS         10
#define EXCEPTION_SEGMENT_NOT_PRESENT 11
#define EXCEPTION_STACK_FAULT         12
#define EXCEPTION_GPF                 13
#define EXCEPTION_PAGE_FAULT          14
#define EXCEPTION_RESERVED_15          15
#define EXCEPTION_FPU_FAULT           16
#define EXCEPTION_ALIGNMENT_CHECK     17
#define EXCEPTION_MACHINE_CHECK       18
#define EXCEPTION_SIMD_FAULT          19
#define EXCEPTION_VIRTUALIZATION      20
#define EXCEPTION_RESERVED_21_31       21  // 21-31 are reserved

// IRQ numbers (after remapping, IRQ 0-15 map to interrupts 32-47)
#define IRQ_TIMER     0
#define IRQ_KEYBOARD  1
#define IRQ_CASCADE   2
#define IRQ_COM2      3
#define IRQ_COM1      4
#define IRQ_LPT2      5
#define IRQ_FLOPPY    6
#define IRQ_LPT1      7
#define IRQ_CMOS      8
#define IRQ_FREE1     9
#define IRQ_FREE2     10
#define IRQ_FREE3     11
#define IRQ_PS2       12
#define IRQ_FPU       13
#define IRQ_ATA0      14
#define IRQ_ATA1      15

// Interrupt handler function pointer type
typedef void (*interrupt_handler_t)(void);

// Initialize IDT
void idt_init(void);

// Set an IDT entry
void idt_set_entry(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags);

// Register an interrupt handler
void idt_register_handler(unsigned char interrupt, interrupt_handler_t handler);

// Exception handler declarations (implemented in idt.c)
void exception_handler_0(void);   // Divide by zero
void exception_handler_1(void);   // Debug
void exception_handler_2(void);   // NMI
void exception_handler_3(void);   // Breakpoint
void exception_handler_4(void);   // Overflow
void exception_handler_5(void);   // Bound range
void exception_handler_6(void);   // Invalid opcode
void exception_handler_7(void);   // Device not available
void exception_handler_8(void);   // Double fault
void exception_handler_9(void);   // Coprocessor segment overrun
void exception_handler_10(void);  // Invalid TSS
void exception_handler_11(void);  // Segment not present
void exception_handler_12(void);  // Stack fault
void exception_handler_13(void);  // General protection fault
void exception_handler_14(void);  // Page fault
void exception_handler_15(void);  // Reserved
void exception_handler_16(void);  // FPU fault
void exception_handler_17(void);  // Alignment check
void exception_handler_18(void);  // Machine check
void exception_handler_19(void);  // SIMD fault
void exception_handler_20(void);  // Virtualization

// IRQ handler declarations
void irq_handler_0(void);   // Timer
void irq_handler_1(void);   // Keyboard
void irq_handler_2(void);   // Cascade
void irq_handler_3(void);   // COM2
void irq_handler_4(void);   // COM1
void irq_handler_5(void);   // LPT2
void irq_handler_6(void);   // Floppy
void irq_handler_7(void);   // LPT1
void irq_handler_8(void);   // CMOS
void irq_handler_9(void);   // Free
void irq_handler_10(void);  // Free
void irq_handler_11(void);  // Free
void irq_handler_12(void);  // PS2
void irq_handler_13(void);  // FPU
void irq_handler_14(void);  // ATA0
void irq_handler_15(void);  // ATA1

#endif // IDT_H

