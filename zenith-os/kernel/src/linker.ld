ENTRY(_start)

SECTIONS {
    . = 0x100000;  /* Kernel load addr */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}