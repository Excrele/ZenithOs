#ifndef STDLIB_H
#define STDLIB_H

#include "../string/string.h"  // For size_t

// Memory allocation
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);

// String conversion
int atoi(const char* str);
long atol(const char* str);
char* itoa(int value, char* str, int base);
char* ltoa(long value, char* str, int base);

// Utility
int abs(int x);
void exit(int status);

// Constants
#define NULL ((void*)0)

#endif // STDLIB_H
