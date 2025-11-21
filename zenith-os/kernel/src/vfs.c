#include "vfs.h"
#include "memory.h"
#include "pmm.h"
#include "paging.h"
#include "timer.h"

// Root file system node
static vfs_node_t* g_root = 0;

// Mount points
static mount_point_t g_mount_points[16];
static int g_mount_count = 0;

// File descriptors
#define MAX_FDS 256
static vfs_node_t* g_open_files[MAX_FDS];
static unsigned int g_fd_positions[MAX_FDS];
static int g_next_fd = 0;

// Registered file systems
static vfs_filesystem_t* g_filesystems[16];
static int g_fs_count = 0;

// Initialize VFS
void vfs_init(void) {
    // Create root node
    g_root = (vfs_node_t*)pmm_alloc_page();
    if (!g_root) {
        return;
    }
    
    // Clear root node
    for (unsigned int i = 0; i < sizeof(vfs_node_t); i++) {
        ((unsigned char*)g_root)[i] = 0;
    }
    
    g_root->name[0] = '/';
    g_root->name[1] = '\0';
    g_root->type = FS_TYPE_DIR;
    g_root->size = 0;
    g_root->parent = 0;
    g_root->next = 0;
    g_root->child = 0;
    g_root->permissions = FS_PERM_OWNER | (FS_PERM_READ | FS_PERM_EXEC) << 3 | (FS_PERM_READ | FS_PERM_EXEC) << 6;
    g_root->owner = 0;
    g_root->group = 0;
    g_root->created_time = 0;
    g_root->modified_time = 0;
    g_root->accessed_time = 0;
    
    // Clear mount points
    g_mount_count = 0;
    g_fs_count = 0;
    g_next_fd = 0;
    
    // Clear file descriptors
    for (int i = 0; i < MAX_FDS; i++) {
        g_open_files[i] = 0;
        g_fd_positions[i] = 0;
    }
}

// Register a file system driver
void vfs_register_filesystem(vfs_filesystem_t* fs) {
    if (g_fs_count < 16) {
        g_filesystems[g_fs_count++] = fs;
    }
}

// Mount a file system
int vfs_mount(const char* device, const char* mountpoint, const char* fstype) {
    // Find file system driver
    vfs_filesystem_t* fs = 0;
    for (int i = 0; i < g_fs_count; i++) {
        if (strcmp(g_filesystems[i]->name, fstype) == 0) {
            fs = g_filesystems[i];
            break;
        }
    }
    
    if (!fs) {
        return -1; // File system not found
    }
    
    // Find mount point node
    vfs_node_t* mount_node = vfs_find_node(mountpoint);
    if (!mount_node || mount_node->type != FS_TYPE_DIR) {
        return -1; // Invalid mount point
    }
    
    // Call file system mount function
    if (fs->mount && fs->mount(device, mountpoint) != 0) {
        return -1; // Mount failed
    }
    
    // Add mount point
    if (g_mount_count < 16) {
        strcpy(g_mount_points[g_mount_count].path, mountpoint);
        mount_point_t* mp = &g_mount_points[g_mount_count];
        mp->root = mount_node;
        mp->fs = fs;
        g_mount_count++;
        return 0;
    }
    
    return -1; // Too many mount points
}

// Find a node by path
vfs_node_t* vfs_find_node(const char* path) {
    if (!path || !g_root) {
        return 0;
    }
    
    // Handle root
    if (path[0] == '/' && path[1] == '\0') {
        return g_root;
    }
    
    // Simple path resolution (for now)
    // In a full implementation, we'd parse the path and traverse directories
    // For now, just return root
    return g_root;
}

// Open a file
file_descriptor_t vfs_open(const char* path, unsigned int flags) {
    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        return -1;
    }
    
    // Call node's open function if available
    if (node->open && node->open(node, flags) != 0) {
        return -1;
    }
    
    // Allocate file descriptor
    if (g_next_fd >= MAX_FDS) {
        return -1; // Too many open files
    }
    
    int fd = g_next_fd++;
    g_open_files[fd] = node;
    g_fd_positions[fd] = 0;
    
    return fd;
}

// Close a file
int vfs_close(file_descriptor_t fd) {
    if (fd < 0 || fd >= MAX_FDS || !g_open_files[fd]) {
        return -1;
    }
    
    vfs_node_t* node = g_open_files[fd];
    
    // Call node's close function if available
    if (node->close) {
        node->close(node);
    }
    
    g_open_files[fd] = 0;
    g_fd_positions[fd] = 0;
    
    return 0;
}

// Read from a file
int vfs_read(file_descriptor_t fd, void* buffer, unsigned int size) {
    if (fd < 0 || fd >= MAX_FDS || !g_open_files[fd]) {
        return -1;
    }
    
    vfs_node_t* node = g_open_files[fd];
    unsigned int pos = g_fd_positions[fd];
    
    if (!node->read) {
        return -1; // Read not supported
    }
    
    int bytes_read = node->read(node, pos, size, (unsigned char*)buffer);
    if (bytes_read > 0) {
        g_fd_positions[fd] += bytes_read;
    }
    
    return bytes_read;
}

// Write to a file
int vfs_write(file_descriptor_t fd, const void* buffer, unsigned int size) {
    if (fd < 0 || fd >= MAX_FDS || !g_open_files[fd]) {
        return -1;
    }
    
    vfs_node_t* node = g_open_files[fd];
    unsigned int pos = g_fd_positions[fd];
    
    if (!node->write) {
        return -1; // Write not supported
    }
    
    int bytes_written = node->write(node, pos, size, (unsigned char*)buffer);
    if (bytes_written > 0) {
        g_fd_positions[fd] += bytes_written;
    }
    
    return bytes_written;
}

// Seek in a file
int vfs_seek(file_descriptor_t fd, int offset, int whence) {
    if (fd < 0 || fd >= MAX_FDS || !g_open_files[fd]) {
        return -1;
    }
    
    vfs_node_t* node = g_open_files[fd];
    unsigned int new_pos = 0;
    
    switch (whence) {
        case 0: // SEEK_SET
            new_pos = offset;
            break;
        case 1: // SEEK_CUR
            new_pos = g_fd_positions[fd] + offset;
            break;
        case 2: // SEEK_END
            new_pos = node->size + offset;
            break;
        default:
            return -1;
    }
    
    if (new_pos > node->size) {
        return -1; // Invalid position
    }
    
    g_fd_positions[fd] = new_pos;
    return new_pos;
}

// Simple string comparison (since we don't have libc)
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Simple string copy
static void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Simple string length
static unsigned int strlen(const char* s) {
    unsigned int len = 0;
    while (*s++) len++;
    return len;
}

// Create a directory
int vfs_mkdir(const char* path) {
    if (!path || !g_root) {
        return -1;
    }
    
    // Find parent directory
    vfs_node_t* parent = g_root;
    const char* name = path;
    
    // Simple implementation: assume path is just a name under root
    // In a full implementation, we'd parse the path
    if (path[0] == '/') {
        name = path + 1;
    }
    
    // Check if directory already exists
    if (parent->finddir && parent->finddir(parent, name)) {
        return -1; // Already exists
    }
    
    // Create new directory node
    vfs_node_t* new_dir = (vfs_node_t*)pmm_alloc_page();
    if (!new_dir) {
        return -1;
    }
    
    // Clear and initialize
    for (unsigned int i = 0; i < sizeof(vfs_node_t); i++) {
        ((unsigned char*)new_dir)[i] = 0;
    }
    
    strcpy(new_dir->name, name);
    new_dir->type = FS_TYPE_DIR;
    new_dir->size = 0;
    new_dir->parent = parent;
    new_dir->next = parent->child;
    parent->child = new_dir;
    
    // Set default permissions (rwxr-xr-x)
    new_dir->permissions = FS_PERM_OWNER | (FS_PERM_READ | FS_PERM_EXEC) << 3 | (FS_PERM_READ | FS_PERM_EXEC) << 6;
    new_dir->owner = 0; // Root
    new_dir->group = 0; // Root group
    
    // Set timestamps
    unsigned int current_time = (unsigned int)timer_get_ticks();
    new_dir->created_time = current_time;
    new_dir->modified_time = current_time;
    new_dir->accessed_time = current_time;
    
    // Inherit directory operations from parent if available
    if (parent->readdir) {
        new_dir->readdir = parent->readdir;
    }
    if (parent->finddir) {
        new_dir->finddir = parent->finddir;
    }
    
    return 0;
}

// Remove a directory
int vfs_rmdir(const char* path) {
    if (!path || !g_root) {
        return -1;
    }
    
    // Find the directory
    vfs_node_t* dir = vfs_find_node(path);
    if (!dir || dir->type != FS_TYPE_DIR) {
        return -1; // Not found or not a directory
    }
    
    // Check if directory is empty
    if (dir->child) {
        return -1; // Directory not empty
    }
    
    // Remove from parent's child list
    if (dir->parent) {
        vfs_node_t* current = dir->parent->child;
        vfs_node_t* prev = 0;
        
        while (current) {
            if (current == dir) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    dir->parent->child = current->next;
                }
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    
    // Free the node
    pmm_free_page((unsigned long long)dir);
    
    return 0;
}

// Read directory entry
vfs_node_t* vfs_readdir(file_descriptor_t fd, unsigned int index) {
    if (fd < 0 || fd >= MAX_FDS || !g_open_files[fd]) {
        return 0;
    }
    
    vfs_node_t* node = g_open_files[fd];
    if (node->type != FS_TYPE_DIR) {
        return 0; // Not a directory
    }
    
    if (node->readdir) {
        return node->readdir(node, index);
    }
    
    // Fallback: traverse child list
    vfs_node_t* current = node->child;
    unsigned int i = 0;
    while (current && i < index) {
        current = current->next;
        i++;
    }
    
    return current;
}

// Delete a file (unlink)
int vfs_unlink(const char* path) {
    if (!path || !g_root) {
        return -1;
    }
    
    // Find the file
    vfs_node_t* node = vfs_find_node(path);
    if (!node || node->type != FS_TYPE_FILE) {
        return -1; // Not found or not a file
    }
    
    // Remove from parent's child list
    if (node->parent) {
        vfs_node_t* current = node->parent->child;
        vfs_node_t* prev = 0;
        
        while (current) {
            if (current == node) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    node->parent->child = current->next;
                }
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    
    // Call file system specific unlink if available
    if (node->unlink) {
        node->unlink(node);
    }
    
    // Free the node
    pmm_free_page((unsigned long long)node);
    
    return 0;
}

