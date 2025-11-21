#include "../libc/stdio/stdio.h"
#include "../libc/stdlib/stdlib.h"
#include "../libc/sys/syscall.h"

// System information program
// Displays system statistics

int main(int argc, char* argv[]) {
    printf("Zenith OS System Information\n");
    printf("============================\n\n");
    
    // Get process ID
    int pid = syscall(SYS_GETPID, 0, 0, 0, 0);
    printf("Current Process ID: %d\n", pid);
    
    // Memory information (would need new system calls)
    printf("\nMemory Information:\n");
    printf("  (Memory stats require additional system calls)\n");
    
    // Process count (would need new system call)
    printf("\nProcess Information:\n");
    printf("  (Process count requires additional system calls)\n");
    
    // Kernel version (placeholder)
    printf("\nKernel Version:\n");
    printf("  Zenith OS v0.1\n");
    
    // Uptime (would need time system calls)
    printf("\nUptime:\n");
    printf("  (Uptime requires RTC/time system calls)\n");
    
    printf("\nNote: Some information requires additional kernel features.\n");
    printf("See FEATURE_GOALS.md for planned enhancements.\n");
    
    return 0;
}

