#ifndef STRING_H
#define STRING_H

// String length
size_t strlen(const char* str);

// String copy
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

// String concatenation
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);

// String comparison
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

// String search
char* strstr(const char* haystack, const char* needle);
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);

// String tokenization
char* strtok(char* str, const char* delim);

// String duplication
char* strdup(const char* str);

// Memory functions
void* memset(void* ptr, int value, size_t num);
void* memcpy(void* dest, const void* src, size_t num);
void* memmove(void* dest, const void* src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);

// Size type
typedef unsigned int size_t;

#endif // STRING_H

