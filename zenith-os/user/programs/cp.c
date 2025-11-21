#include "../libc/stdio/stdio.h"
#include "../libc/stdlib/stdlib.h"
#include "../libc/string/string.h"

// Copy file program
// Usage: cp <source> <destination>

#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: cp <source> <destination>\n");
        return 1;
    }
    
    const char* src_path = argv[1];
    const char* dst_path = argv[2];
    
    // Open source file
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("Error: Cannot open source file: %s\n", src_path);
        return 1;
    }
    
    // Open/create destination file
    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (dst_fd < 0) {
        printf("Error: Cannot create destination file: %s\n", dst_path);
        close(src_fd);
        return 1;
    }
    
    // Copy data
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int total_copied = 0;
    
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        int bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written < 0) {
            printf("Error: Write failed\n");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
        total_copied += bytes_written;
    }
    
    if (bytes_read < 0) {
        printf("Error: Read failed\n");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    
    // Close files
    close(src_fd);
    close(dst_fd);
    
    printf("Copied %d bytes from %s to %s\n", total_copied, src_path, dst_path);
    return 0;
}

