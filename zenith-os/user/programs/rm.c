#include "../libc/stdio/stdio.h"
#include "../libc/stdlib/stdlib.h"
#include "../libc/sys/syscall.h"

// Remove file program
// Usage: rm <file>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: rm <file> [file2] ...\n");
        return 1;
    }
    
    int errors = 0;
    
    for (int i = 1; i < argc; i++) {
        int result = syscall(SYS_UNLINK, (unsigned int)argv[i], 0, 0, 0);
        if (result < 0) {
            printf("Error: Cannot remove %s\n", argv[i]);
            errors++;
        } else {
            printf("Removed: %s\n", argv[i]);
        }
    }
    
    return (errors > 0) ? 1 : 0;
}

