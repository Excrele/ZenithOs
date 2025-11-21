#ifndef VGA_H
#define VGA_H

// VGA text mode constants
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Color constants
#define VGA_COLOR_BLACK         0x0
#define VGA_COLOR_BLUE         0x1
#define VGA_COLOR_GREEN        0x2
#define VGA_COLOR_CYAN         0x3
#define VGA_COLOR_RED          0x4
#define VGA_COLOR_MAGENTA      0x5
#define VGA_COLOR_BROWN        0x6
#define VGA_COLOR_LIGHT_GREY   0x7
#define VGA_COLOR_DARK_GREY    0x8
#define VGA_COLOR_LIGHT_BLUE   0x9
#define VGA_COLOR_LIGHT_GREEN  0xA
#define VGA_COLOR_LIGHT_CYAN   0xB
#define VGA_COLOR_LIGHT_RED    0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW       0xE
#define VGA_COLOR_WHITE        0xF

// Create color byte (foreground | (background << 4))
#define VGA_COLOR(fg, bg) ((fg) | ((bg) << 4))

// Default colors
#define VGA_DEFAULT_COLOR VGA_COLOR(VGA_COLOR_WHITE, VGA_COLOR_BLACK)

// Initialize VGA driver
void vga_init(void);

// Clear screen
void vga_clear(void);

// Set cursor position
void vga_set_cursor(int row, int col);

// Get cursor position
void vga_get_cursor(int* row, int* col);

// Print a character at current position
void vga_putchar(char c);

// Print a string
void vga_print(const char* str);

// Print a character with color
void vga_putchar_color(char c, unsigned char color);

// Print a string with color
void vga_print_color(const char* str, unsigned char color);

// Scroll screen up by one line
void vga_scroll(void);

// Set text color
void vga_set_color(unsigned char color);

// Get current text color
unsigned char vga_get_color(void);

#endif // VGA_H

