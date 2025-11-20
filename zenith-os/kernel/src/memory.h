#ifndef MEMORY_H
#define MEMORY_H

// Memory map entry types (from E820 specification)
#define MEMORY_TYPE_USABLE        1
#define MEMORY_TYPE_RESERVED      2
#define MEMORY_TYPE_ACPI_RECLAIM  3
#define MEMORY_TYPE_ACPI_NVS      4
#define MEMORY_TYPE_BAD           5

// Memory map entry structure (24 bytes)
typedef struct {
    unsigned long long base;      // Base address
    unsigned long long length;    // Length in bytes
    unsigned int type;            // Memory type
    unsigned int extended;        // Extended attributes (ACPI 3.0+)
} __attribute__((packed)) memory_map_entry_t;

// Memory map structure passed from bootloader
// Layout: [count(4)] [entry0(24)] [entry1(24)] ...
// Stored at 0x80000 by bootloader
typedef struct {
    unsigned int count;                    // Number of entries (at offset 0)
    memory_map_entry_t entries[127];       // Up to 127 entries (starting at offset 4)
} __attribute__((packed)) memory_map_t;

// Global memory map (set by bootloader)
extern memory_map_t* g_memory_map;

#endif // MEMORY_H

