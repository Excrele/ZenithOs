#include "fs_simple.h"
#include "vfs.h"
#include "ata.h"
#include "pmm.h"
#include "paging.h"
#include "timer.h"

// Simple file system magic number
#define SIMPLE_FS_MAGIC 0x504D4953  // "SIMP"

// Block size (512 bytes = 1 sector)
#define SIMPLE_BLOCK_SIZE 512

// File system layout:
// Block 0: File system header
// Block 1: Block bitmap (1 bit per block, tracks free/used blocks)
// Block 2-17: Inode table (16 inodes, 1 inode per block)
// Block 18+: Data blocks

#define FS_HEADER_BLOCK 0
#define BITMAP_BLOCK 1
#define INODE_TABLE_START 2
#define INODE_TABLE_SIZE 16
#define DATA_BLOCK_START (INODE_TABLE_START + INODE_TABLE_SIZE)

// In-memory file system state
static simple_fs_header_t g_fs_header;
static int g_fs_mounted = 0;
static unsigned char g_block_bitmap[512];  // Bitmap cache (1 block = 512 bytes = 4096 blocks)

// Read a block from disk
static int read_block(unsigned int block_num, unsigned char* buffer) {
    return ata_read_sectors(block_num, 1, buffer);
}

// Write a block to disk
static int write_block(unsigned int block_num, unsigned char* buffer) {
    return ata_write_sectors(block_num, 1, buffer);
}

// Load block bitmap from disk
static int load_bitmap(void) {
    return read_block(BITMAP_BLOCK, g_block_bitmap);
}

// Save block bitmap to disk
static int save_bitmap(void) {
    return write_block(BITMAP_BLOCK, g_block_bitmap);
}

// Check if a block is free
static int is_block_free(unsigned int block_num) {
    if (block_num < DATA_BLOCK_START || block_num >= g_fs_header.total_blocks) {
        return 0; // Invalid or reserved block
    }
    
    unsigned int bitmap_index = block_num - DATA_BLOCK_START;
    unsigned int byte_index = bitmap_index / 8;
    unsigned int bit_index = bitmap_index % 8;
    
    if (byte_index >= 512) {
        return 0; // Out of range
    }
    
    return !(g_block_bitmap[byte_index] & (1 << bit_index));
}

// Mark a block as used
static void mark_block_used(unsigned int block_num) {
    if (block_num < DATA_BLOCK_START || block_num >= g_fs_header.total_blocks) {
        return;
    }
    
    unsigned int bitmap_index = block_num - DATA_BLOCK_START;
    unsigned int byte_index = bitmap_index / 8;
    unsigned int bit_index = bitmap_index % 8;
    
    if (byte_index >= 512) {
        return;
    }
    
    if (!(g_block_bitmap[byte_index] & (1 << bit_index))) {
        g_block_bitmap[byte_index] |= (1 << bit_index);
        g_fs_header.free_blocks--;
    }
}

// Mark a block as free
static void mark_block_free(unsigned int block_num) {
    if (block_num < DATA_BLOCK_START || block_num >= g_fs_header.total_blocks) {
        return;
    }
    
    unsigned int bitmap_index = block_num - DATA_BLOCK_START;
    unsigned int byte_index = bitmap_index / 8;
    unsigned int bit_index = bitmap_index % 8;
    
    if (byte_index >= 512) {
        return;
    }
    
    if (g_block_bitmap[byte_index] & (1 << bit_index)) {
        g_block_bitmap[byte_index] &= ~(1 << bit_index);
        g_fs_header.free_blocks++;
    }
}

// Allocate a free block
static unsigned int allocate_block(void) {
    // Find first free block
    for (unsigned int block = DATA_BLOCK_START; block < g_fs_header.total_blocks; block++) {
        if (is_block_free(block)) {
            mark_block_used(block);
            save_bitmap();  // Persist bitmap
            return block;
        }
    }
    return 0; // No free blocks
}

// Free a block
static void free_block(unsigned int block_num) {
    mark_block_free(block_num);
    save_bitmap();  // Persist bitmap
}

// Free all blocks used by an inode
static void free_inode_blocks(simple_inode_t* inode) {
    if (!inode) {
        return;
    }
    
    // Free all blocks
    for (int i = 0; i < 16; i++) {
        if (inode->blocks[i] != 0) {
            free_block(inode->blocks[i]);
            inode->blocks[i] = 0;
        }
    }
    
    inode->size = 0;
}

// Read inode from disk
static int read_inode(unsigned int inode_num, simple_inode_t* inode) {
    if (inode_num >= INODE_TABLE_SIZE) {
        return -1;
    }
    
    unsigned int block_num = INODE_TABLE_START + inode_num;
    return read_block(block_num, (unsigned char*)inode);
}

// Write inode to disk
static int write_inode(unsigned int inode_num, simple_inode_t* inode) {
    if (inode_num >= INODE_TABLE_SIZE) {
        return -1;
    }
    
    unsigned int block_num = INODE_TABLE_START + inode_num;
    return write_block(block_num, (unsigned char*)inode);
}

// Simple file system read function
static int simple_fs_read(vfs_node_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
    if (!node || !buffer) {
        return -1;
    }
    
    simple_inode_t* inode = (simple_inode_t*)node->fs_data;
    if (!inode) {
        return -1;
    }
    
    if (offset >= node->size) {
        return 0; // End of file
    }
    
    if (offset + size > node->size) {
        size = node->size - offset;
    }
    
    // Calculate which blocks to read
    unsigned int start_block = offset / SIMPLE_BLOCK_SIZE;
    unsigned int end_block = (offset + size - 1) / SIMPLE_BLOCK_SIZE;
    
    unsigned int bytes_read = 0;
    unsigned char* temp_buffer = (unsigned char*)pmm_alloc_page();
    if (!temp_buffer) {
        return -1;
    }
    
    for (unsigned int i = start_block; i <= end_block && i < 16; i++) {
        if (inode->blocks[i] == 0) {
            break; // No more blocks
        }
        
        if (read_block(inode->blocks[i], temp_buffer) != 1) {
            break; // Read error
        }
        
        // Calculate copy range
        unsigned int block_offset = (i == start_block) ? (offset % SIMPLE_BLOCK_SIZE) : 0;
        unsigned int copy_size = SIMPLE_BLOCK_SIZE - block_offset;
        if (bytes_read + copy_size > size) {
            copy_size = size - bytes_read;
        }
        
        // Copy data
        for (unsigned int j = 0; j < copy_size; j++) {
            buffer[bytes_read + j] = temp_buffer[block_offset + j];
        }
        
        bytes_read += copy_size;
    }
    
    pmm_free_page((unsigned long long)temp_buffer);
    return bytes_read;
}

// Simple file system write function
static int simple_fs_write(vfs_node_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
    if (!node || !buffer || !g_fs_mounted) {
        return -1;
    }
    
    simple_inode_t* inode = (simple_inode_t*)node->fs_data;
    if (!inode) {
        return -1;
    }
    
    // Calculate which blocks we need
    unsigned int start_block_index = offset / SIMPLE_BLOCK_SIZE;
    unsigned int end_block_index = (offset + size - 1) / SIMPLE_BLOCK_SIZE;
    
    // Check if we need more blocks than available
    if (end_block_index >= 16) {
        return -1; // File too large (max 16 blocks)
    }
    
    // Allocate blocks if needed
    for (unsigned int i = start_block_index; i <= end_block_index; i++) {
        if (inode->blocks[i] == 0) {
            unsigned int new_block = allocate_block();
            if (new_block == 0) {
                return -1; // Out of space
            }
            inode->blocks[i] = new_block;
        }
    }
    
    // Write data to blocks
    unsigned int bytes_written = 0;
    unsigned char* temp_buffer = (unsigned char*)pmm_alloc_page();
    if (!temp_buffer) {
        return -1;
    }
    
    for (unsigned int i = start_block_index; i <= end_block_index && i < 16; i++) {
        if (inode->blocks[i] == 0) {
            break;
        }
        
        // Read existing block data
        if (read_block(inode->blocks[i], temp_buffer) != 1) {
            break; // Read error
        }
        
        // Calculate what part of this block to write
        unsigned int block_offset = (i == start_block_index) ? (offset % SIMPLE_BLOCK_SIZE) : 0;
        unsigned int write_size = SIMPLE_BLOCK_SIZE - block_offset;
        if (bytes_written + write_size > size) {
            write_size = size - bytes_written;
        }
        
        // Copy data into block buffer
        for (unsigned int j = 0; j < write_size; j++) {
            temp_buffer[block_offset + j] = buffer[bytes_written + j];
        }
        
        // Write block back to disk
        if (write_block(inode->blocks[i], temp_buffer) != 1) {
            break; // Write error
        }
        
        bytes_written += write_size;
    }
    
    pmm_free_page((unsigned long long)temp_buffer);
    
    // Update file size if we wrote beyond current size
    unsigned int new_size = offset + bytes_written;
    if (new_size > inode->size) {
        inode->size = new_size;
        node->size = new_size;
    }
    
    // Update modification time
    unsigned int current_time = (unsigned int)timer_get_ticks();
    inode->modified_time = current_time;
    node->modified_time = current_time;
    
    // Write inode back to disk
    write_inode(inode->inode_num, inode);
    
    // Update file system header
    write_block(FS_HEADER_BLOCK, (unsigned char*)&g_fs_header);
    
    return bytes_written;
}

// Simple file system open function
static int simple_fs_open(vfs_node_t* node, unsigned int flags) {
    if (!node) {
        return -1;
    }
    
    // Update access time
    node->accessed_time = (unsigned int)timer_get_ticks();
    
    // Update inode if it exists
    simple_inode_t* inode = (simple_inode_t*)node->fs_data;
    if (inode) {
        inode->accessed_time = node->accessed_time;
        write_inode(inode->inode_num, inode);
    }
    
    return 0;
}

// Simple file system close function
static int simple_fs_close(vfs_node_t* node) {
    // For now, just return success
    return 0;
}

// Simple file system readdir function
static vfs_node_t* simple_fs_readdir(vfs_node_t* node, unsigned int index) {
    if (!node || node->type != FS_TYPE_DIR) {
        return 0;
    }
    
    // For now, use the VFS in-memory structure
    // In a full implementation, we'd read from disk inodes
    vfs_node_t* current = node->child;
    unsigned int i = 0;
    while (current && i < index) {
        current = current->next;
        i++;
    }
    
    return current;
}

// Simple file system finddir function
static vfs_node_t* simple_fs_finddir(vfs_node_t* node, const char* name) {
    if (!node || node->type != FS_TYPE_DIR || !name) {
        return 0;
    }
    
    // Search in child list
    vfs_node_t* current = node->child;
    while (current) {
        // Simple string comparison
        const char* n1 = current->name;
        const char* n2 = name;
        int match = 1;
        while (*n1 && *n2 && *n1 == *n2) {
            n1++;
            n2++;
        }
        if (*n1 == '\0' && *n2 == '\0') {
            return current; // Found
        }
        current = current->next;
    }
    
    return 0; // Not found
}

// Simple file system unlink function
static int simple_fs_unlink(vfs_node_t* node) {
    if (!node || node->type != FS_TYPE_FILE) {
        return -1;
    }
    
    simple_inode_t* inode = (simple_inode_t*)node->fs_data;
    if (inode) {
        // Free all blocks used by the file
        free_inode_blocks(inode);
        
        // Mark inode as free (simplified - in real FS, we'd have an inode bitmap)
        // For now, just clear the inode
        write_inode(inode->inode_num, inode);
    }
    
    return 0;
}

// Initialize simple file system
int simple_fs_init(void) {
    g_fs_mounted = 0;
    return 0;
}

// Mount simple file system
int simple_fs_mount(const char* device, const char* mountpoint) {
    // Read file system header from sector 0
    if (read_block(FS_HEADER_BLOCK, (unsigned char*)&g_fs_header) != 1) {
        return -1; // Read error
    }
    
    // Check magic number
    if (g_fs_header.magic != SIMPLE_FS_MAGIC) {
        return -1; // Invalid file system
    }
    
    // Load block bitmap
    if (load_bitmap() != 1) {
        return -1; // Failed to load bitmap
    }
    
    g_fs_mounted = 1;
    return 0;
}

// Register simple file system with VFS
void simple_fs_register(void) {
    static vfs_filesystem_t fs = {
        .name = "simple",
        .mount = simple_fs_mount,
        .unmount = 0, // Not implemented
        .open = 0     // Will be handled by VFS
    };
    
    vfs_register_filesystem(&fs);
}

