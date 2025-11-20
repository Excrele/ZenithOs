# Interrupt Service Routine stubs
# These push the interrupt number and call the common handler

# Exception handlers (0-20)
.global isr0
isr0:
    push $0
    push $0
    jmp isr_common_stub

.global isr1
isr1:
    push $0
    push $1
    jmp isr_common_stub

.global isr2
isr2:
    push $0
    push $2
    jmp isr_common_stub

.global isr3
isr3:
    push $0
    push $3
    jmp isr_common_stub

.global isr4
isr4:
    push $0
    push $4
    jmp isr_common_stub

.global isr5
isr5:
    push $0
    push $5
    jmp isr_common_stub

.global isr6
isr6:
    push $0
    push $6
    jmp isr_common_stub

.global isr7
isr7:
    push $0
    push $7
    jmp isr_common_stub

.global isr8
isr8:
    # Error code is pushed by CPU
    push $8
    jmp isr_common_stub

.global isr9
isr9:
    push $0
    push $9
    jmp isr_common_stub

.global isr10
isr10:
    # Error code is pushed by CPU
    push $10
    jmp isr_common_stub

.global isr11
isr11:
    # Error code is pushed by CPU
    push $11
    jmp isr_common_stub

.global isr12
isr12:
    # Error code is pushed by CPU
    push $12
    jmp isr_common_stub

.global isr13
isr13:
    # Error code is pushed by CPU
    push $13
    jmp isr_common_stub

.global isr14
isr14:
    # Error code is pushed by CPU
    push $14
    jmp isr_common_stub

.global isr15
isr15:
    push $0
    push $15
    jmp isr_common_stub

.global isr16
isr16:
    push $0
    push $16
    jmp isr_common_stub

.global isr17
isr17:
    # Error code is pushed by CPU
    push $17
    jmp isr_common_stub

.global isr18
isr18:
    push $0
    push $18
    jmp isr_common_stub

.global isr19
isr19:
    push $0
    push $19
    jmp isr_common_stub

.global isr20
isr20:
    push $0
    push $20
    jmp isr_common_stub

# IRQ handlers (32-47)
.global irq0
irq0:
    push $0
    push $32
    jmp irq_common_stub

.global irq1
irq1:
    push $0
    push $33
    jmp irq_common_stub

.global irq2
irq2:
    push $0
    push $34
    jmp irq_common_stub

.global irq3
irq3:
    push $0
    push $35
    jmp irq_common_stub

.global irq4
irq4:
    push $0
    push $36
    jmp irq_common_stub

.global irq5
irq5:
    push $0
    push $37
    jmp irq_common_stub

.global irq6
irq6:
    push $0
    push $38
    jmp irq_common_stub

.global irq7
irq7:
    push $0
    push $39
    jmp irq_common_stub

.global irq8
irq8:
    push $0
    push $40
    jmp irq_common_stub

.global irq9
irq9:
    push $0
    push $41
    jmp irq_common_stub

.global irq10
irq10:
    push $0
    push $42
    jmp irq_common_stub

.global irq11
irq11:
    push $0
    push $43
    jmp irq_common_stub

.global irq12
irq12:
    push $0
    push $44
    jmp irq_common_stub

.global irq13
irq13:
    push $0
    push $45
    jmp irq_common_stub

.global irq14
irq14:
    push $0
    push $46
    jmp irq_common_stub

.global irq15
irq15:
    push $0
    push $47
    jmp irq_common_stub

# Common stub for exceptions
isr_common_stub:
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
    
    # Call C handler (interrupt_handler_common)
    # Stack: [GS] [FS] [ES] [DS] [EDI] [ESI] [EBP] [ESP] [EBX] [EDX] [ECX] [EAX] [ERR] [INT#]
    # Interrupt number is at [ESP + 52] (13 * 4 bytes from current ESP)
    mov 52(%esp), %eax
    push %eax
    call interrupt_handler_common
    add $4, %esp
    
    # Restore registers
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
    # Remove error code and interrupt number
    add $8, %esp
    
    # Return from interrupt
    iret

# Common stub for IRQs (same as exceptions)
irq_common_stub:
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
    
    # Call C handler
    # Interrupt number is at [ESP + 52]
    mov 52(%esp), %eax
    push %eax
    call interrupt_handler_common
    add $4, %esp
    
    # Restore registers
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
    # Remove error code and interrupt number
    add $8, %esp
    
    # Return from interrupt
    iret

