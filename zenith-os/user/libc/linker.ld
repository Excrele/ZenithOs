/* User space linker script */
ENTRY(_start)

SECTIONS {
    /* User programs start at 0x08000000 (128MB) */
    . = 0x08000000;
    
    .text : {
        *(.text)
        *(.text.*)
    }
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* Stack starts at 0x80000000 (2GB) and grows downward */
    /* Heap is between code and stack */
}

