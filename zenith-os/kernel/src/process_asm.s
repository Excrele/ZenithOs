# Context switching assembly code
# Simplified version - full context switching should ideally be done from interrupt handlers

# Save current process context  
# Called with: save_context(process_t* proc)
.global save_context
save_context:
    push %ebp
    mov %esp, %ebp
    mov 8(%ebp), %eax      # Get proc parameter
    add $32, %eax          # Point to registers field
    
    # Save registers (EAX will be 0 for now - caller should save it)
    mov %edi, 0(%eax)
    mov %esi, 4(%eax)
    mov %ebp, 8(%eax)
    mov %esp, %ebx
    add $8, %ebx
    mov %ebx, 12(%eax)     # ESP
    mov %ebx, 16(%eax)     # EBX (temporary)
    mov %edx, 20(%eax)
    mov %ecx, 24(%eax)
    mov $0, 28(%eax)       # EAX (placeholder - should be saved by caller)
    
    # Save segment registers
    mov %gs, %bx
    mov %bx, 32(%eax)
    mov %fs, %bx
    mov %bx, 36(%eax)
    mov %es, %bx
    mov %bx, 40(%eax)
    mov %ds, %bx
    mov %bx, 44(%eax)
    
    # Save control registers
    mov 4(%ebp), %ebx      # Return address (EIP)
    mov %ebx, 48(%eax)
    mov $0x08, %ebx        # CS
    mov %ebx, 52(%eax)
    pushf
    pop %ebx
    mov %ebx, 56(%eax)     # EFLAGS
    mov %esp, %ebx
    add $8, %ebx
    mov %ebx, 60(%eax)     # ESP
    mov $0x10, %ebx        # SS
    mov %ebx, 64(%eax)
    
    pop %ebp
    ret

# Restore process context
# Called with: restore_context(process_t* proc)  
.global restore_context
restore_context:
    mov 4(%esp), %eax      # Get proc parameter
    add $32, %eax          # Point to registers
    
    # Restore segment registers
    movzwl 44(%eax), %ebx
    mov %bx, %ds
    movzwl 40(%eax), %ebx
    mov %bx, %es
    movzwl 36(%eax), %ebx
    mov %bx, %fs
    movzwl 32(%eax), %ebx
    mov %bx, %gs
    
    # Restore general registers (save EAX/EDX/ECX for after ESP change)
    mov 0(%eax), %edi
    mov 4(%eax), %esi
    mov 8(%eax), %ebp
    mov 16(%eax), %ebx
    
    # Save EAX/EDX/ECX values before we lose the pointer
    push 28(%eax)         # EAX value
    push 20(%eax)         # EDX value
    push 24(%eax)         # ECX value
    
    # Restore stack pointer
    mov 12(%eax), %esp
    
    # Set up iret stack frame
    # Get pointer back from original parameter (it's at [ESP+4] before our pushes)
    # After restoring ESP, the original frame is at a different location
    # We'll recalculate: original call frame had [ret_addr][proc]
    # New ESP points to caller's stack, so proc is at [ESP+4] relative to NEW frame
    # But we need to account for what we've pushed
    # Actually, the saved ESP should point to where the caller's stack was
    # So [ESP+4] should work, but we've pushed 3 values, so it's at ESP+16
    # Let's just recalculate from the function entry point
    # The parameter was at [old_ESP+4], and old_ESP is what we restored
    # So parameter is at [ESP+4] (the restored ESP)
    mov 4(%esp), %eax     # Get proc parameter from restored stack
    add $32, %eax         # Point to registers
    
    # Push iret frame: SS, ESP, EFLAGS, CS, EIP
    mov 64(%eax), %ebx    # SS
    push %ebx
    mov 60(%eax), %ebx    # ESP
    push %ebx
    mov 56(%eax), %ebx    # EFLAGS
    push %ebx
    mov 52(%eax), %ebx    # CS
    push %ebx
    mov 48(%eax), %ebx    # EIP
    push %ebx
    
    # Restore EAX, EDX, ECX (they're on stack below iret frame)
    # Stack: [ret_addr][proc][EAX_val][EDX_val][ECX_val][SS][ESP][EFLAGS][CS][EIP]
    # EAX is at ESP+28, EDX at ESP+24, ECX at ESP+20
    mov 28(%esp), %eax    # Restore EAX
    mov 24(%esp), %edx    # Restore EDX
    mov 20(%esp), %ecx    # Restore ECX
    
    # iret will pop EIP, CS, EFLAGS, ESP, SS and continue
    iret
