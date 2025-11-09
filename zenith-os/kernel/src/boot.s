# Multiboot stub (for future GRUB compat, but we ignore header for now)
.global _start
_start:
    mov $0x10, %ax   # Data seg
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $0x90FFF, %esp  # Stack

    call kernel_main
    cli
    hlt