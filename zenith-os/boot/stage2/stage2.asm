; Stage 2: Switch to protected mode, load kernel
[org 0x7e00]
[bits 16]

start:
    ; Print status
    mov si, msg_protected
    call print_string_real

    ; Setup A20 line (enable >1MB mem)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    mov eax, cr0
    or al, 1            ; Set PE bit
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp 0x08:protected_mode  ; CS selector 0x08

[bits 32]
protected_mode:
    ; Set segments
    mov ax, 0x10        ; Data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000    ; Stack in high mem

    ; Print in protected mode (VGA text)
    mov ebx, msg_pm
    call print_pm

    ; Load kernel from disk (assume kernel.bin at sector 10, load to 0x100000)
    ; For now, stub: Jump to kernel entry (hardcoded)
    jmp 0x100000       ; Kernel load addr

print_pm:
    mov edi, 0xb8000    ; VGA buffer
.loop:
    mov al, [ebx]
    or al, al
    jz .done
    mov ah, 0x0f        ; White on black
    mov [edi], ax
    inc ebx
    add edi, 2
    jmp .loop
.done:
    ret

; GDT: Null, Code (32-bit), Data (32-bit)
gdt_start:
    dq 0x0000000000000000  ; Null
    dq 0x00cf9a000000ffff  ; Code
    dq 0x00cf92000000ffff  ; Data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Real-mode print (fallback)
print_string_real:
    ; Reuse stage1's print_string logic (int 0x10)
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string_real
.done:
    ret

msg_protected: db 'Switching to Protected Mode...', 0xd, 0xa, 0
msg_pm:        db 'Hello from Protected Mode! Kernel loading...', 0