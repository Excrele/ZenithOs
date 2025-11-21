#include "idt.h"
#include "memory.h"

// Simple VGA text output (declared in kernel.c, but we'll use our own)
static void print_string_at(const char* str, int row, int col) {
    char* video = (char*) 0xb8000;
    int pos = (row * 80 + col) * 2;
    for (int i = 0; str[i] != '\0'; ++i) {
        video[pos + i * 2] = str[i];
        video[pos + i * 2 + 1] = 0x0C; // Red on black for errors
    }
}

// Exception handler implementations
void exception_handler_0(void) {
    print_string_at("EXCEPTION: Divide by Zero!", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_1(void) {
    print_string_at("EXCEPTION: Debug", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_2(void) {
    print_string_at("EXCEPTION: NMI", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_3(void) {
    print_string_at("EXCEPTION: Breakpoint", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_4(void) {
    print_string_at("EXCEPTION: Overflow", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_5(void) {
    print_string_at("EXCEPTION: Bound Range Exceeded", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_6(void) {
    print_string_at("EXCEPTION: Invalid Opcode", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_7(void) {
    print_string_at("EXCEPTION: Device Not Available", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_8(void) {
    print_string_at("EXCEPTION: Double Fault", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_9(void) {
    print_string_at("EXCEPTION: Coprocessor Segment Overrun", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_10(void) {
    print_string_at("EXCEPTION: Invalid TSS", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_11(void) {
    print_string_at("EXCEPTION: Segment Not Present", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_12(void) {
    print_string_at("EXCEPTION: Stack Fault", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_13(void) {
    print_string_at("EXCEPTION: General Protection Fault", 24, 0);
    while (1) asm volatile ("hlt");
}

// Page fault handler - get faulting address from CR2
void exception_handler_14(void) {
    unsigned int faulting_address;
    asm volatile ("mov %%cr2, %0" : "=r"(faulting_address));
    
    // Get page fault error code from stack (pushed by CPU)
    unsigned int error_code;
    asm volatile ("mov 4(%%esp), %0" : "=r"(error_code));
    
    print_string_at("EXCEPTION: Page Fault", 24, 0);
    
    // Display faulting address (simple hex display)
    char* video = (char*) 0xb8000;
    int pos = (24 * 80 + 22) * 2;
    const char hex_chars[] = "0123456789ABCDEF";
    
    video[pos++] = 'A';
    video[pos++] = 0x0C;
    video[pos++] = 'D';
    video[pos++] = 0x0C;
    video[pos++] = 'R';
    video[pos++] = 0x0C;
    video[pos++] = ':';
    video[pos++] = 0x0C;
    
    for (int i = 7; i >= 0; i--) {
        int digit = (faulting_address >> (i * 4)) & 0xF;
        video[pos++] = hex_chars[digit];
        video[pos++] = 0x0C;
    }
    
    // Check error code bits
    if (error_code & 0x1) {
        print_string_at("Present", 23, 0);
    }
    if (error_code & 0x2) {
        print_string_at("Write", 23, 10);
    }
    if (error_code & 0x4) {
        print_string_at("User", 23, 20);
    }
    
    while (1) asm volatile ("hlt");
}

void exception_handler_15(void) {
    print_string_at("EXCEPTION: Reserved (15)", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_16(void) {
    print_string_at("EXCEPTION: FPU Fault", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_17(void) {
    print_string_at("EXCEPTION: Alignment Check", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_18(void) {
    print_string_at("EXCEPTION: Machine Check", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_19(void) {
    print_string_at("EXCEPTION: SIMD Fault", 24, 0);
    while (1) asm volatile ("hlt");
}

void exception_handler_20(void) {
    print_string_at("EXCEPTION: Virtualization", 24, 0);
    while (1) asm volatile ("hlt");
}

// IRQ handlers (stubs for now)
void irq_handler_0(void) {
    // Timer - handled by timer.c
}

void irq_handler_1(void) {
    // Keyboard - handled by keyboard.c
    extern void keyboard_interrupt_handler(void);
    keyboard_interrupt_handler();
}

void irq_handler_2(void) {}
void irq_handler_3(void) {}
void irq_handler_4(void) {}
void irq_handler_5(void) {}
void irq_handler_6(void) {}
void irq_handler_7(void) {}
void irq_handler_8(void) {}
void irq_handler_9(void) {}
void irq_handler_10(void) {}
void irq_handler_11(void) {}
void irq_handler_12(void) {}
void irq_handler_13(void) {}
void irq_handler_14(void) {}
void irq_handler_15(void) {}

