#include "serial.h"

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

// Wait for serial port to be ready to transmit
static void serial_wait_transmit(unsigned short port) {
    unsigned int timeout = 100000;
    while (timeout--) {
        if (inb(port + SERIAL_STATUS) & 0x20) { // Transmit empty bit
            return;
        }
    }
}

// Wait for serial port to have data
static void serial_wait_receive(unsigned short port) {
    unsigned int timeout = 100000;
    while (timeout--) {
        if (inb(port + SERIAL_STATUS) & 0x01) { // Data ready bit
            return;
        }
    }
}

// Initialize serial port
int serial_init(unsigned short port) {
    // Disable interrupts
    outb(port + SERIAL_INT_EN, 0x00);
    
    // Enable DLAB (Divisor Latch Access Bit) to set baud rate
    outb(port + SERIAL_LINE, SERIAL_LINE_DLAB);
    
    // Set baud rate to 115200 (divisor = 1)
    // Low byte
    outb(port + SERIAL_DATA, 0x01);
    // High byte
    outb(port + SERIAL_INT_EN, 0x00);
    
    // Disable DLAB, set 8 data bits, 1 stop bit, no parity
    outb(port + SERIAL_LINE, 0x03);
    
    // Enable FIFO, clear TX/RX FIFOs, 14-byte threshold
    outb(port + SERIAL_FIFO, 0xC7);
    
    // Enable interrupts (IRQ enabled, RTS/DSR set)
    outb(port + SERIAL_MODEM, 0x0B);
    
    // Test serial port (loopback)
    outb(port + SERIAL_DATA, 0xAE);
    if (inb(port + SERIAL_DATA) != 0xAE) {
        return -1; // Serial port not working
    }
    
    return 0;
}

// Write a character to serial port
void serial_putchar(unsigned short port, char c) {
    serial_wait_transmit(port);
    outb(port + SERIAL_DATA, (unsigned char)c);
}

// Write a string to serial port
void serial_write(unsigned short port, const char* str) {
    while (*str) {
        serial_putchar(port, *str);
        str++;
    }
}

// Read a character from serial port (non-blocking)
int serial_getchar(unsigned short port) {
    if (!serial_has_data(port)) {
        return -1; // No data available
    }
    
    return (int)inb(port + SERIAL_DATA);
}

// Check if data is available
int serial_has_data(unsigned short port) {
    return (inb(port + SERIAL_STATUS) & 0x01) != 0;
}

