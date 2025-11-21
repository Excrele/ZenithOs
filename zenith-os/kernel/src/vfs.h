#ifndef VFS_H
#define VFS_H

// File descriptor
typedef int file_descriptor_t;

// File modes
#define O_RDONLY    0x0001
#define O_WRONLY    0x0002
#define O_RDWR      0x0004
#define O_CREAT     0x0008
#define O_TRUNC     0x0010
#define O_APPEND    0x0020

// File types
#define FS_TYPE_FILE    1
#define FS_TYPE_DIR     2
#define FS_TYPE_CHAR    3
#define FS_TYPE_BLOCK   4

// File permissions
#define FS_PERM_READ    0x04
#define FS_PERM_WRITE   0x02
#define FS_PERM_EXEC    0x01
#define FS_PERM_OWNER   (FS_PERM_READ | FS_PERM_WRITE | FS_PERM_EXEC)
#define FS_PERM_GROUP   (FS_PERM_READ | FS_PERM_EXEC)
#define FS_PERM_OTHER   (FS_PERM_READ)

// File system node (inode-like structure)
typedef struct vfs_node {
    char name[256];              // File name
    unsigned int type;           // File type
    unsigned int size;           // File size in bytes
    unsigned int flags;           // File flags
    unsigned int inode;           // Inode number (file system specific)
    unsigned int permissions;     // File permissions
    unsigned int owner;           // Owner user ID
    unsigned int group;           // Group ID
    unsigned int created_time;    // Creation timestamp
    unsigned int modified_time;   // Modification timestamp
    unsigned int accessed_time;   // Access timestamp
    
    // File operations
    int (*read)(struct vfs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
    int (*write)(struct vfs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
    int (*open)(struct vfs_node* node, unsigned int flags);
    int (*close)(struct vfs_node* node);
    int (*unlink)(struct vfs_node* node);  // Delete file
    
    // Directory operations
    struct vfs_node* (*readdir)(struct vfs_node* node, unsigned int index);
    struct vfs_node* (*finddir)(struct vfs_node* node, const char* name);
    
    // File system specific data
    void* fs_data;               // File system private data
    
    // Linked list
    struct vfs_node* parent;     // Parent directory
    struct vfs_node* next;       // Sibling node
    struct vfs_node* child;      // First child (for directories)
} vfs_node_t;

// Mount point
typedef struct {
    char path[256];              // Mount path
    vfs_node_t* root;            // Root node of mounted file system
    struct vfs_filesystem* fs;    // File system driver
} mount_point_t;

// File system driver structure
typedef struct vfs_filesystem {
    char name[32];               // File system name
    int (*mount)(const char* device, const char* mountpoint);
    int (*unmount)(const char* mountpoint);
    vfs_node_t* (*open)(const char* path);
} vfs_filesystem_t;

// Initialize VFS
void vfs_init(void);

// Mount a file system
int vfs_mount(const char* device, const char* mountpoint, const char* fstype);

// Unmount a file system
int vfs_unmount(const char* mountpoint);

// Open a file
file_descriptor_t vfs_open(const char* path, unsigned int flags);

// Close a file
int vfs_close(file_descriptor_t fd);

// Read from a file
int vfs_read(file_descriptor_t fd, void* buffer, unsigned int size);

// Write to a file
int vfs_write(file_descriptor_t fd, const void* buffer, unsigned int size);

// Seek in a file
int vfs_seek(file_descriptor_t fd, int offset, int whence);

// Get file information
int vfs_stat(const char* path, vfs_node_t* stat);

// Register a file system driver
void vfs_register_filesystem(vfs_filesystem_t* fs);

// Find a node by path
vfs_node_t* vfs_find_node(const char* path);

// Directory operations
int vfs_mkdir(const char* path);
int vfs_rmdir(const char* path);
vfs_node_t* vfs_readdir(file_descriptor_t fd, unsigned int index);

// File operations
int vfs_unlink(const char* path);  // Delete a file

#endif // VFS_H

