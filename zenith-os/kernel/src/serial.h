#ifndef SERIAL_H
#define SERIAL_H

// Serial port base addresses
#define COM1_BASE 0x3F8
#define COM2_BASE 0x2F8

// Serial port registers (offset from base)
#define SERIAL_DATA    0x0
#define SERIAL_INT_EN  0x1
#define SERIAL_FIFO    0x2
#define SERIAL_LINE    0x3
#define SERIAL_MODEM   0x4
#define SERIAL_STATUS  0x5

// Line control register bits
#define SERIAL_LINE_DLAB 0x80  // Divisor latch access bit

// Initialize serial port
int serial_init(unsigned short port);

// Write a character to serial port
void serial_putchar(unsigned short port, char c);

// Write a string to serial port
void serial_write(unsigned short port, const char* str);

// Read a character from serial port (non-blocking)
// Returns character if available, -1 if no data
int serial_getchar(unsigned short port);

// Check if data is available
int serial_has_data(unsigned short port);

#endif // SERIAL_H

