#include "keyboard.h"
#include "pic.h"

// PS/2 keyboard ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT   0x64
#define KEYBOARD_COMMAND_PORT  0x64

// Keyboard status register bits
#define KEYBOARD_STATUS_OUTPUT_FULL  0x01
#define KEYBOARD_STATUS_INPUT_FULL   0x02
#define KEYBOARD_STATUS_SELF_TEST   0x04
#define KEYBOARD_STATUS_COMMAND     0x08
#define KEYBOARD_STATUS_UNLOCKED     0x10
#define KEYBOARD_STATUS_NO_ERROR     0x20
#define KEYBOARD_STATUS_TIMEOUT      0x40
#define KEYBOARD_STATUS_PARITY_ERROR 0x80

// Keyboard commands
#define KEYBOARD_CMD_ENABLE          0xAE
#define KEYBOARD_CMD_DISABLE         0xAD
#define KEYBOARD_CMD_RESET           0xFF
#define KEYBOARD_CMD_SET_LEDS        0xED

// Special scan codes
#define SCAN_CODE_RELEASE            0x80
#define SCAN_CODE_EXTENDED           0xE0
#define SCAN_CODE_EXTENDED_RELEASE   0xE1

// Keyboard state
static unsigned char g_keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static unsigned int g_keyboard_buffer_head = 0;
static unsigned int g_keyboard_buffer_tail = 0;
static unsigned int g_keyboard_buffer_count = 0;
static int g_keyboard_shift = 0;
static int g_keyboard_ctrl = 0;
static int g_keyboard_alt = 0;
static int g_keyboard_caps = 0;

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

// Wait for keyboard controller to be ready
static void keyboard_wait(void) {
    unsigned int timeout = 100000;
    while (timeout--) {
        if (!(inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_INPUT_FULL)) {
            return;
        }
    }
}

// Send command to keyboard controller
static void keyboard_send_command(unsigned char command) {
    keyboard_wait();
    outb(KEYBOARD_COMMAND_PORT, command);
}

// Send data to keyboard
static void keyboard_send_data(unsigned char data) {
    keyboard_wait();
    outb(KEYBOARD_DATA_PORT, data);
}

// Scan code to ASCII conversion table (normal keys)
static const unsigned char scan_code_normal[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// Scan code to ASCII conversion table (shifted keys)
static const unsigned char scan_code_shift[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// Add character to keyboard buffer
static void keyboard_buffer_add(unsigned char c) {
    if (g_keyboard_buffer_count >= KEYBOARD_BUFFER_SIZE) {
        return; // Buffer full
    }
    
    g_keyboard_buffer[g_keyboard_buffer_tail] = c;
    g_keyboard_buffer_tail = (g_keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    g_keyboard_buffer_count++;
}

// Process scan code
static void keyboard_process_scan_code(unsigned char scan_code) {
    // Check for release code
    if (scan_code & SCAN_CODE_RELEASE) {
        unsigned char key = scan_code & ~SCAN_CODE_RELEASE;
        
        // Handle modifier keys
        switch (key) {
            case 0x2A: // Left Shift
            case 0x36: // Right Shift
                g_keyboard_shift = 0;
                break;
            case 0x1D: // Left/Right Ctrl
                g_keyboard_ctrl = 0;
                break;
            case 0x38: // Left/Right Alt
                g_keyboard_alt = 0;
                break;
        }
        return;
    }
    
    // Handle modifier keys (press)
    switch (scan_code) {
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            g_keyboard_shift = 1;
            return;
        case 0x1D: // Left/Right Ctrl
            g_keyboard_ctrl = 1;
            return;
        case 0x38: // Left/Right Alt
            g_keyboard_alt = 1;
            return;
        case 0x3A: // Caps Lock
            g_keyboard_caps = !g_keyboard_caps;
            return;
    }
    
    // Convert scan code to ASCII
    unsigned char ascii = 0;
    if (scan_code < 128) {
        if (g_keyboard_shift || g_keyboard_caps) {
            ascii = scan_code_shift[scan_code];
            // If caps lock is on and it's a letter, use lowercase
            if (g_keyboard_caps && !g_keyboard_shift && 
                scan_code >= 0x10 && scan_code <= 0x19) { // Letters
                ascii = scan_code_normal[scan_code];
            } else if (g_keyboard_caps && !g_keyboard_shift &&
                       scan_code >= 0x1E && scan_code <= 0x26) { // More letters
                ascii = scan_code_normal[scan_code];
            } else if (g_keyboard_caps && !g_keyboard_shift &&
                       scan_code >= 0x2C && scan_code <= 0x32) { // Even more letters
                ascii = scan_code_normal[scan_code];
            }
        } else {
            ascii = scan_code_normal[scan_code];
        }
    }
    
    // Add to buffer if valid ASCII
    if (ascii != 0) {
        keyboard_buffer_add(ascii);
    }
}

// Initialize PS/2 keyboard
int keyboard_init(void) {
    // Clear buffer
    g_keyboard_buffer_head = 0;
    g_keyboard_buffer_tail = 0;
    g_keyboard_buffer_count = 0;
    g_keyboard_shift = 0;
    g_keyboard_ctrl = 0;
    g_keyboard_alt = 0;
    g_keyboard_caps = 0;
    
    // Enable keyboard
    keyboard_send_command(KEYBOARD_CMD_ENABLE);
    
    // Enable keyboard IRQ (IRQ 1)
    pic_enable_irq(1);
    
    return 0;
}

// Keyboard interrupt handler (called from IRQ 1)
void keyboard_interrupt_handler(void) {
    // Read scan code from keyboard data port
    unsigned char scan_code = inb(KEYBOARD_DATA_PORT);
    
    // Process scan code
    keyboard_process_scan_code(scan_code);
    
    // EOI is handled by interrupt_handler_common in idt.c
}

// Get a character from keyboard buffer (non-blocking)
unsigned char keyboard_getchar(void) {
    if (g_keyboard_buffer_count == 0) {
        return 0; // Buffer empty
    }
    
    unsigned char c = g_keyboard_buffer[g_keyboard_buffer_head];
    g_keyboard_buffer_head = (g_keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    g_keyboard_buffer_count--;
    
    return c;
}

// Check if keyboard buffer has data
int keyboard_has_data(void) {
    return g_keyboard_buffer_count > 0;
}

// Clear keyboard buffer
void keyboard_clear_buffer(void) {
    g_keyboard_buffer_head = 0;
    g_keyboard_buffer_tail = 0;
    g_keyboard_buffer_count = 0;
}

