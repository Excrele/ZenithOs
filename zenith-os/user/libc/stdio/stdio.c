#include "stdio.h"
#include "../sys/syscall.h"
#include "../string/string.h"

// File operations
int open(const char* path, int flags) {
    return syscall(SYS_OPEN, (unsigned int)path, flags, 0, 0);
}

int close(int fd) {
    return syscall(SYS_CLOSE, fd, 0, 0, 0);
}

int read(int fd, void* buf, size_t count) {
    return syscall(SYS_READ, fd, (unsigned int)buf, count, 0);
}

int write(int fd, const void* buf, size_t count) {
    return syscall(SYS_WRITE, fd, (unsigned int)buf, count, 0);
}

int seek(int fd, int offset, int whence) {
    return syscall(SYS_SEEK, fd, offset, whence, 0);
}

// Character I/O
int putchar(int c) {
    char ch = (char)c;
    return write(STDOUT_FILENO, &ch, 1);
}

int getchar(void) {
    char c;
    int result = read(STDIN_FILENO, &c, 1);
    if (result == 1) {
        return (unsigned char)c;
    }
    return -1;
}

int puts(const char* str) {
    size_t len = strlen(str);
    if (write(STDOUT_FILENO, str, len) < 0) {
        return -1;
    }
    if (putchar('\n') < 0) {
        return -1;
    }
    return 0;
}

// Simple number to string conversion helpers
static void uint_to_str(unsigned int value, char* buffer, int* pos) {
    if (value == 0) {
        buffer[(*pos)++] = '0';
        return;
    }
    
    char temp[32];
    int i = 0;
    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i > 0) {
        buffer[(*pos)++] = temp[--i];
    }
}

static void int_to_str(int value, char* buffer, int* pos) {
    if (value < 0) {
        buffer[(*pos)++] = '-';
        value = -value;
    }
    uint_to_str((unsigned int)value, buffer, pos);
}

static void hex_to_str(unsigned int value, char* buffer, int* pos, int uppercase) {
    if (value == 0) {
        buffer[(*pos)++] = '0';
        return;
    }
    
    char temp[32];
    int i = 0;
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    
    while (value > 0) {
        temp[i++] = digits[value % 16];
        value /= 16;
    }
    while (i > 0) {
        buffer[(*pos)++] = temp[--i];
    }
}

// Internal: variable argument sprintf
static int vsprintf(char* str, const char* format, char* args) {
    int pos = 0;
    int arg_pos = 0;
    
    while (*format && pos < 511) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int value = *(int*)(args + arg_pos);
                    arg_pos += sizeof(int);
                    int_to_str(value, str, &pos);
                    break;
                }
                case 'u': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    uint_to_str(value, str, &pos);
                    break;
                }
                case 'x': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    hex_to_str(value, str, &pos, 0);
                    break;
                }
                case 'X': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    hex_to_str(value, str, &pos, 1);
                    break;
                }
                case 'c': {
                    int value = *(int*)(args + arg_pos);
                    arg_pos += sizeof(int);
                    str[pos++] = (char)value;
                    break;
                }
                case 's': {
                    const char* value = *(const char**)(args + arg_pos);
                    arg_pos += sizeof(char*);
                    if (value) {
                        size_t len = strlen(value);
                        for (size_t i = 0; i < len && pos < 511; i++) {
                            str[pos++] = value[i];
                        }
                    } else {
                        const char* null_str = "(null)";
                        for (int i = 0; null_str[i] && pos < 511; i++) {
                            str[pos++] = null_str[i];
                        }
                    }
                    break;
                }
                case '%': {
                    str[pos++] = '%';
                    break;
                }
                default: {
                    str[pos++] = '%';
                    str[pos++] = *format;
                    break;
                }
            }
            format++;
        } else {
            str[pos++] = *format++;
        }
    }
    str[pos] = '\0';
    return pos;
}

// Internal: variable argument snprintf
static int vsnprintf(char* str, size_t size, const char* format, char* args) {
    int pos = 0;
    int arg_pos = 0;
    
    while (*format && pos < (int)(size - 1)) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int value = *(int*)(args + arg_pos);
                    arg_pos += sizeof(int);
                    int_to_str(value, str, &pos);
                    break;
                }
                case 'u': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    uint_to_str(value, str, &pos);
                    break;
                }
                case 'x': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    hex_to_str(value, str, &pos, 0);
                    break;
                }
                case 'X': {
                    unsigned int value = *(unsigned int*)(args + arg_pos);
                    arg_pos += sizeof(unsigned int);
                    hex_to_str(value, str, &pos, 1);
                    break;
                }
                case 'c': {
                    int value = *(int*)(args + arg_pos);
                    arg_pos += sizeof(int);
                    str[pos++] = (char)value;
                    break;
                }
                case 's': {
                    const char* value = *(const char**)(args + arg_pos);
                    arg_pos += sizeof(char*);
                    if (value) {
                        size_t len = strlen(value);
                        for (size_t i = 0; i < len && pos < (int)(size - 1); i++) {
                            str[pos++] = value[i];
                        }
                    } else {
                        const char* null_str = "(null)";
                        for (int i = 0; null_str[i] && pos < (int)(size - 1); i++) {
                            str[pos++] = null_str[i];
                        }
                    }
                    break;
                }
                case '%': {
                    str[pos++] = '%';
                    break;
                }
                default: {
                    str[pos++] = '%';
                    str[pos++] = *format;
                    break;
                }
            }
            format++;
        } else {
            str[pos++] = *format++;
        }
    }
    str[pos] = '\0';
    return pos;
}

