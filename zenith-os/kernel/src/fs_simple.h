#ifndef FS_SIMPLE_H
#define FS_SIMPLE_H

#include "vfs.h"

// Simple file system structure
// This is a minimal file system for demonstration
// In a real OS, you'd use FAT, Ext2, or similar

// File system header (at sector 0)
typedef struct {
    unsigned int magic;           // Magic number: 0x53494D50 ("SIMP")
    unsigned int version;         // File system version
    unsigned int root_inode;      // Root directory inode
    unsigned int total_blocks;   // Total blocks in file system
    unsigned int free_blocks;     // Free blocks
    char label[32];              // Volume label
} __attribute__((packed)) simple_fs_header_t;

// Inode structure
typedef struct {
    unsigned int inode_num;       // Inode number
    unsigned int type;            // File type
    unsigned int size;            // File size
    unsigned int blocks[16];      // Block pointers (direct)
    unsigned int parent_inode;    // Parent directory inode
    char name[256];              // File name
    unsigned int permissions;     // File permissions (rwx for owner/group/other)
    unsigned int owner;           // Owner user ID
    unsigned int group;           // Group ID
    unsigned int created_time;    // Creation timestamp
    unsigned int modified_time;   // Modification timestamp
    unsigned int accessed_time;   // Access timestamp
} __attribute__((packed)) simple_inode_t;

// Initialize simple file system
int simple_fs_init(void);

// Mount simple file system
int simple_fs_mount(const char* device, const char* mountpoint);

// Register simple file system with VFS
void simple_fs_register(void);

#endif // FS_SIMPLE_H

