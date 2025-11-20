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

void exception_handler_14(void) {
    print_string_at("EXCEPTION: Page Fault", 24, 0);
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
    // Keyboard - to be implemented
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

