#ifndef KEYBOARD_H
#define KEYBOARD_H

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Initialize PS/2 keyboard
// Returns 0 on success, -1 on failure
int keyboard_init(void);

// Keyboard interrupt handler (called from IRQ 1)
void keyboard_interrupt_handler(void);

// Get a character from keyboard buffer (non-blocking)
// Returns character if available, 0 if buffer is empty
unsigned char keyboard_getchar(void);

// Check if keyboard buffer has data
// Returns 1 if data available, 0 otherwise
int keyboard_has_data(void);

// Clear keyboard buffer
void keyboard_clear_buffer(void);

#endif // KEYBOARD_H

