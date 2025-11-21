# System call interrupt handler (INT 0x80)
# System call number in EAX, arguments in EBX, ECX, EDX, ESI

.global syscall_interrupt
syscall_interrupt:
    # Save all registers
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    # Load kernel data segments
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # Get system call number and arguments from saved registers
    # Stack after pusha+segs: [GS] [FS] [ES] [DS] [EDI] [ESI] [EBP] [ESP] [EBX] [EDX] [ECX] [EAX]
    # Offsets from current ESP (pointing to GS):
    # EAX = 40, ECX = 32, EDX = 28, EBX = 24, ESI = 36
    
    # Push arguments in reverse order (C calling convention: arg4, arg3, arg2, arg1, syscall_num)
    # We need to adjust offsets as we push
    mov 36(%esp), %eax    # ESI (arg4) - offset 36
    push %eax
    mov 28+4(%esp), %eax  # EDX (arg3) - offset 28+4=32 (account for 1 push)
    push %eax
    mov 32+8(%esp), %eax  # ECX (arg2) - offset 32+8=40 (account for 2 pushes)
    push %eax
    mov 24+12(%esp), %eax # EBX (arg1) - offset 24+12=36 (account for 3 pushes)
    push %eax
    mov 40+16(%esp), %eax # EAX (syscall_num) - offset 40+16=56 (account for 4 pushes)
    push %eax
    
    # Call C handler
    call syscall_handler
    add $20, %esp         # Remove arguments
    
    # Return value is in EAX
    # Save it because popa will overwrite EAX
    mov %eax, %ecx        # Save return value in ECX
    
    # Restore registers
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa                  # Restore all registers (EAX will be overwritten)
    
    # Restore return value to EAX
    mov %ecx, %eax
    
    # Return from interrupt
    iret
