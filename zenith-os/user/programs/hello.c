#include "../libc/stdio/stdio.h"

int main(int argc, char* argv[]) {
    printf("Hello, World from Zenith OS!\n");
    printf("Arguments: %d\n", argc);
    
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i] ? argv[i] : "(null)");
    }
    
    return 0;
}

