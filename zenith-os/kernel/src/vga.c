#include "vga.h"

// VGA ports
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_CURSOR_HIGH 0x0E
#define VGA_CURSOR_LOW  0x0F

// Current cursor position
static int g_cursor_row = 0;
static int g_cursor_col = 0;
static unsigned char g_text_color = VGA_DEFAULT_COLOR;

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

// Update hardware cursor position
static void vga_update_cursor(void) {
    unsigned short pos = g_cursor_row * VGA_WIDTH + g_cursor_col;
    
    outb(VGA_CRTC_INDEX, VGA_CURSOR_HIGH);
    outb(VGA_CRTC_DATA, (unsigned char)((pos >> 8) & 0xFF));
    outb(VGA_CRTC_INDEX, VGA_CURSOR_LOW);
    outb(VGA_CRTC_DATA, (unsigned char)(pos & 0xFF));
}

// Initialize VGA driver
void vga_init(void) {
    g_cursor_row = 0;
    g_cursor_col = 0;
    g_text_color = VGA_DEFAULT_COLOR;
    vga_clear();
    vga_update_cursor();
}

// Clear screen
void vga_clear(void) {
    char* video = (char*)VGA_MEMORY;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = g_text_color;
    }
    g_cursor_row = 0;
    g_cursor_col = 0;
    vga_update_cursor();
}

// Set cursor position
void vga_set_cursor(int row, int col) {
    if (row < 0) row = 0;
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col < 0) col = 0;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    
    g_cursor_row = row;
    g_cursor_col = col;
    vga_update_cursor();
}

// Get cursor position
void vga_get_cursor(int* row, int* col) {
    if (row) *row = g_cursor_row;
    if (col) *col = g_cursor_col;
}

// Scroll screen up by one line
void vga_scroll(void) {
    char* video = (char*)VGA_MEMORY;
    
    // Move all lines up by one
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        video[i * 2] = video[(i + VGA_WIDTH) * 2];
        video[i * 2 + 1] = video[(i + VGA_WIDTH) * 2 + 1];
    }
    
    // Clear last line
    int last_line_start = (VGA_HEIGHT - 1) * VGA_WIDTH;
    for (int i = 0; i < VGA_WIDTH; i++) {
        video[(last_line_start + i) * 2] = ' ';
        video[(last_line_start + i) * 2 + 1] = g_text_color;
    }
    
    // Move cursor up if it was on the last line
    if (g_cursor_row >= VGA_HEIGHT - 1) {
        g_cursor_row = VGA_HEIGHT - 2;
    }
    vga_update_cursor();
}

// Print a character at current position
void vga_putchar(char c) {
    vga_putchar_color(c, g_text_color);
}

// Print a character with color
void vga_putchar_color(char c, unsigned char color) {
    char* video = (char*)VGA_MEMORY;
    
    if (c == '\n') {
        // Newline: move to next line
        g_cursor_row++;
        g_cursor_col = 0;
    } else if (c == '\r') {
        // Carriage return: move to start of line
        g_cursor_col = 0;
    } else if (c == '\t') {
        // Tab: move to next tab stop (every 8 characters)
        g_cursor_col = (g_cursor_col + 8) & ~7;
    } else if (c == '\b') {
        // Backspace: move cursor back one position
        if (g_cursor_col > 0) {
            g_cursor_col--;
            video[(g_cursor_row * VGA_WIDTH + g_cursor_col) * 2] = ' ';
            video[(g_cursor_row * VGA_WIDTH + g_cursor_col) * 2 + 1] = color;
        }
        vga_update_cursor();
        return;
    } else {
        // Regular character
        video[(g_cursor_row * VGA_WIDTH + g_cursor_col) * 2] = c;
        video[(g_cursor_row * VGA_WIDTH + g_cursor_col) * 2 + 1] = color;
        g_cursor_col++;
    }
    
    // Check if we need to wrap or scroll
    if (g_cursor_col >= VGA_WIDTH) {
        g_cursor_col = 0;
        g_cursor_row++;
    }
    
    if (g_cursor_row >= VGA_HEIGHT) {
        vga_scroll();
    }
    
    vga_update_cursor();
}

// Print a string
void vga_print(const char* str) {
    vga_print_color(str, g_text_color);
}

// Print a string with color
void vga_print_color(const char* str, unsigned char color) {
    while (*str) {
        vga_putchar_color(*str, color);
        str++;
    }
}

// Set text color
void vga_set_color(unsigned char color) {
    g_text_color = color;
}

// Get current text color
unsigned char vga_get_color(void) {
    return g_text_color;
}

