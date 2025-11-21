#ifndef STDIO_H
#define STDIO_H

#include "../string/string.h"  // For size_t

// Standard file descriptors
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// File operations
int open(const char* path, int flags);
int close(int fd);
int read(int fd, void* buf, size_t count);
int write(int fd, const void* buf, size_t count);
int seek(int fd, int offset, int whence);

// Formatted output
int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);

// Character I/O
int putchar(int c);
int getchar(void);
int puts(const char* str);

// Constants
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#endif // STDIO_H

