#include "string.h"

// String length
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// String copy
char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    size_t i = 0;
    while (i < n && src[i] != '\0') {
        d[i] = src[i];
        i++;
    }
    while (i < n) {
        d[i++] = '\0';
    }
    return dest;
}

// String concatenation
char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;  // Find end of dest
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (*d) d++;  // Find end of dest
    size_t i = 0;
    while (i < n && src[i] != '\0') {
        *d++ = src[i++];
    }
    *d = '\0';
    return dest;
}

// String comparison
int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    size_t i = 0;
    while (i < n && s1[i] && s2[i] && s1[i] == s2[i]) {
        i++;
    }
    if (i == n) return 0;
    return (int)(unsigned char)s1[i] - (int)(unsigned char)s2[i];
}

// String search
char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    
    for (const char* h = haystack; *h; h++) {
        const char* n = needle;
        const char* h2 = h;
        while (*n && *h2 && *n == *h2) {
            n++;
            h2++;
        }
        if (!*n) return (char*)h;
    }
    return 0;
}

char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    if (c == '\0') return (char*)str;
    return 0;
}

char* strrchr(const char* str, int c) {
    const char* last = 0;
    while (*str) {
        if (*str == (char)c) {
            last = str;
        }
        str++;
    }
    if (c == '\0') return (char*)str;
    return (char*)last;
}

// String tokenization
static char* strtok_saved = 0;

char* strtok(char* str, const char* delim) {
    if (str) {
        strtok_saved = str;
    }
    if (!strtok_saved || !*strtok_saved) {
        return 0;
    }
    
    // Skip leading delimiters
    while (*strtok_saved) {
        int is_delim = 0;
        for (const char* d = delim; *d; d++) {
            if (*strtok_saved == *d) {
                is_delim = 1;
                break;
            }
        }
        if (!is_delim) break;
        strtok_saved++;
    }
    
    if (!*strtok_saved) {
        return 0;
    }
    
    char* start = strtok_saved;
    
    // Find end of token
    while (*strtok_saved) {
        int is_delim = 0;
        for (const char* d = delim; *d; d++) {
            if (*strtok_saved == *d) {
                is_delim = 1;
                break;
            }
        }
        if (is_delim) {
            *strtok_saved = '\0';
            strtok_saved++;
            return start;
        }
        strtok_saved++;
    }
    
    strtok_saved = 0;
    return start;
}

// String duplication
char* strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* copy = (char*)malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

// Memory functions
void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < num; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    if (d < s) {
        // Copy forward
        for (size_t i = 0; i < num; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        // Copy backward
        for (size_t i = num; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}

