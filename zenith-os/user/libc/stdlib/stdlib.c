#include "stdlib.h"
#include "../sys/syscall.h"

// Simple heap management using sbrk
// This is a very basic implementation - a real malloc would use a more sophisticated algorithm

#define HEAP_BLOCK_SIZE 16  // Minimum allocation size
#define HEAP_ALIGN 4        // Alignment requirement

// Heap block header
typedef struct heap_block {
    size_t size;
    int free;
    struct heap_block* next;
} heap_block_t;

static heap_block_t* heap_base = 0;
static void* heap_break = 0;

// Initialize heap
static void heap_init(void) {
    if (heap_break == 0) {
        heap_break = (void*)syscall(SYS_SBRK, 0, 0, 0, 0);
        if (heap_break == (void*)-1) {
            heap_break = 0;
        }
    }
}

// Find free block
static heap_block_t* find_free_block(size_t size) {
    heap_block_t* current = heap_base;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

// Extend heap
static heap_block_t* extend_heap(size_t size) {
    size_t block_size = size + sizeof(heap_block_t);
    if (block_size < HEAP_BLOCK_SIZE) {
        block_size = HEAP_BLOCK_SIZE;
    }
    // Align to HEAP_ALIGN
    block_size = (block_size + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1);
    
    void* new_break = (void*)syscall(SYS_SBRK, block_size, 0, 0, 0);
    if (new_break == (void*)-1) {
        return 0;
    }
    
    heap_block_t* block = (heap_block_t*)heap_break;
    block->size = block_size - sizeof(heap_block_t);
    block->free = 0;
    block->next = 0;
    
    heap_break = new_break;
    return block;
}

// Allocate memory
void* malloc(size_t size) {
    if (size == 0) return 0;
    
    heap_init();
    
    // Align size
    size = (size + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1);
    
    // Try to find free block
    heap_block_t* block = find_free_block(size);
    if (block) {
        block->free = 0;
        return (void*)(block + 1);
    }
    
    // Extend heap
    block = extend_heap(size);
    if (!block) {
        return 0;
    }
    
    // Add to list
    if (!heap_base) {
        heap_base = block;
    } else {
        heap_block_t* current = heap_base;
        while (current->next) {
            current = current->next;
        }
        current->next = block;
    }
    
    return (void*)(block + 1);
}

// Free memory
void free(void* ptr) {
    if (!ptr) return;
    
    heap_block_t* block = (heap_block_t*)ptr - 1;
    block->free = 1;
    // Note: We don't merge free blocks in this simple implementation
}

// Allocate and zero memory
void* calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = malloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

// Reallocate memory
void* realloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return malloc(new_size);
    }
    
    if (new_size == 0) {
        free(ptr);
        return 0;
    }
    
    heap_block_t* block = (heap_block_t*)ptr - 1;
    if (block->size >= new_size) {
        return ptr;  // Already large enough
    }
    
    // Allocate new block and copy
    void* new_ptr = malloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        free(ptr);
    }
    return new_ptr;
}

// String to integer
int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

// String to long
long atol(const char* str) {
    long result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

// Integer to string
char* itoa(int value, char* str, int base) {
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    char* ptr = str;
    int negative = 0;
    
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }
    
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }
    
    // Convert to string (reverse order)
    char buffer[32];
    int i = 0;
    while (value > 0) {
        int digit = value % base;
        buffer[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    }
    
    if (negative) {
        *ptr++ = '-';
    }
    
    // Reverse and copy
    while (i > 0) {
        *ptr++ = buffer[--i];
    }
    *ptr = '\0';
    
    return str;
}

// Long to string
char* ltoa(long value, char* str, int base) {
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    char* ptr = str;
    int negative = 0;
    
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }
    
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }
    
    // Convert to string (reverse order)
    char buffer[32];
    int i = 0;
    while (value > 0) {
        int digit = value % base;
        buffer[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    }
    
    if (negative) {
        *ptr++ = '-';
    }
    
    // Reverse and copy
    while (i > 0) {
        *ptr++ = buffer[--i];
    }
    *ptr = '\0';
    
    return str;
}

// Absolute value
int abs(int x) {
    return (x < 0) ? -x : x;
}

// Exit program
void exit(int status) {
    syscall(SYS_EXIT, status, 0, 0, 0);
    // Should never return
    while (1) {}
}

