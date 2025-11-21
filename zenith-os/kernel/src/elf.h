#ifndef ELF_H
#define ELF_H

#include "pmm.h"

// ELF header structure
typedef struct {
    unsigned char e_ident[16];  // ELF identification
    unsigned short e_type;       // Object file type
    unsigned short e_machine;    // Architecture
    unsigned int e_version;      // Object file version
    unsigned int e_entry;        // Entry point virtual address
    unsigned int e_phoff;        // Program header table offset
    unsigned int e_shoff;        // Section header table offset
    unsigned int e_flags;        // Processor-specific flags
    unsigned short e_ehsize;     // ELF header size
    unsigned short e_phentsize;  // Program header entry size
    unsigned short e_phnum;      // Number of program header entries
    unsigned short e_shentsize;  // Section header entry size
    unsigned short e_shnum;      // Number of section header entries
    unsigned short e_shstrndx;     // Section header string table index
} __attribute__((packed)) elf_header_t;

// Program header structure
typedef struct {
    unsigned int p_type;         // Segment type
    unsigned int p_offset;       // Offset in file
    unsigned int p_vaddr;        // Virtual address
    unsigned int p_paddr;        // Physical address
    unsigned int p_filesz;       // Size in file
    unsigned int p_memsz;        // Size in memory
    unsigned int p_flags;        // Segment flags
    unsigned int p_align;        // Alignment
} __attribute__((packed)) elf_program_header_t;

// ELF constants
#define ELF_MAGIC 0x464C457F  // "\x7FELF"
#define PT_LOAD 1            // Loadable segment

// Load an ELF executable
// Returns entry point address on success, 0 on failure
unsigned int elf_load(void* elf_data, unsigned int size);

#endif // ELF_H

