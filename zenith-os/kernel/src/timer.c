#include "timer.h"
#include "idt.h"
#include "pic.h"
#include "scheduler.h"

// System tick counter
static volatile unsigned long long g_ticks = 0;

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

// Timer interrupt handler (IRQ 0)
static void timer_handler(void) {
    g_ticks++;
    
    // Call scheduler tick
    extern void scheduler_tick(void);
    scheduler_tick();
}

// Initialize PIT timer
void timer_init(void) {
    // Register timer interrupt handler
    idt_register_handler(32, timer_handler);  // IRQ 0 = interrupt 32
    
    // Calculate divisor for desired frequency
    // PIT runs at 1193182 Hz, we want TIMER_FREQUENCY Hz
    unsigned short divisor = 1193182 / TIMER_FREQUENCY;
    
    // Send command byte: channel 0, lobyte/hibyte, mode 3 (square wave)
    outb(PIT_COMMAND, PIT_CHANNEL0_SELECT | PIT_ACCESS_LOBYTE | PIT_ACCESS_HIBYTE | PIT_MODE_SQUARE_WAVE);
    
    // Send divisor (low byte then high byte)
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    // Enable timer interrupt (IRQ 0)
    pic_enable_irq(0);
}

// Get current tick count
unsigned long long timer_get_ticks(void) {
    return g_ticks;
}

// Sleep for specified milliseconds
void timer_sleep(unsigned long long milliseconds) {
    unsigned long long start = g_ticks;
    unsigned long long ticks_to_wait = milliseconds / (1000 / TIMER_FREQUENCY);
    
    while ((g_ticks - start) < ticks_to_wait) {
        // Wait (busy wait - could be improved with proper sleep)
        asm volatile ("hlt");
    }
}

