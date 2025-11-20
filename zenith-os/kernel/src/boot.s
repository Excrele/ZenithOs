# Kernel entry point
# EBX contains memory map pointer (passed from bootloader)
.global _start
_start:
    mov $0x10, %ax   # Data seg
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $0x90FFF, %esp  # Stack

    # EBX already contains memory map pointer from bootloader
    # kernel_main can access it via global variable
    call kernel_main
    cli
    hlt