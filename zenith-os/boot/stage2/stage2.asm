; Stage 2: Switch to protected mode, load kernel
[org 0x7e00]
[bits 16]

start:
    ; Print status
    mov si, msg_loading
    call print_string_real

    ; Load kernel from disk (sector 10 onwards)
    ; Kernel starts after stage1 (sector 1) + stage2 (sectors 2-9) = sector 10
    ; Load to temporary buffer at 0x90000 (below 1MB, accessible in real mode)
    ; We'll copy it to 0x100000 after entering protected mode
    mov si, msg_loading_kernel
    call print_string_real
    
    mov ah, 0x02        ; Read sector function
    mov al, 64          ; Number of sectors to read (32KB max)
    mov ch, 0           ; Cylinder 0
    mov cl, 10          ; Sector 10 (kernel starts here)
    mov dh, 0           ; Head 0
    mov dl, 0           ; Drive 0 (floppy A:)
    mov bx, 0x9000      ; Segment 0x9000
    mov es, bx
    mov bx, 0x0000      ; Offset 0 (0x90000)
    
    int 0x13            ; BIOS disk service
    jc kernel_load_error
    
    ; Verify we read at least some data
    cmp al, 0
    je kernel_load_error
    
    mov si, msg_kernel_loaded
    call print_string_real

    ; Detect memory using E820 (INT 0x15, EAX=0xE820)
    mov si, msg_detecting_memory
    call print_string_real
    
    ; Memory map will be stored at 0x80000
    ; Structure: [count(4)] [entries...]
    mov ax, 0x8000      ; Segment 0x8000
    mov es, ax
    mov di, 0x0000      ; Offset 0 (0x80000)
    mov dword [es:di], 0 ; Initialize count to 0
    mov si, di          ; Save count location in SI
    add di, 4           ; Skip count, point to first entry (DI = 4)
    
    xor ebx, ebx        ; Continuation value (0 for first call)
    mov edx, 0x534D4150 ; 'SMAP' signature
    mov ecx, 24         ; Buffer size (24 bytes per entry)
    
e820_loop:
    mov eax, 0xE820     ; E820 function
    int 0x15            ; BIOS interrupt
    jc e820_done        ; CF set on error or end
    
    ; Check if we got valid entry
    cmp eax, 0x534D4150 ; Should return 'SMAP'
    jne e820_done
    
    ; Increment count (at ES:SI)
    push di
    mov di, si          ; Point to count
    inc dword [es:di]   ; Increment count
    pop di
    
    ; Move to next entry (24 bytes per entry)
    add di, 24
    
    ; Check if we have more entries
    cmp ebx, 0
    je e820_done        ; EBX=0 means no more entries
    
    ; Check if we've filled our buffer (max 127 entries)
    push di
    mov di, si          ; Point to count
    cmp dword [es:di], 127
    pop di
    jae e820_done
    
    jmp e820_loop

e820_done:
    mov si, msg_memory_detected
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

    ; Copy kernel from temporary buffer (0x90000) to final location (0x100000)
    ; Assuming max 32KB kernel (64 sectors * 512 bytes)
    mov esi, 0x90000    ; Source
    mov edi, 0x100000   ; Destination
    mov ecx, 8192       ; 32KB = 8192 dwords
    rep movsd           ; Copy dwords
    
    ; Pass memory map pointer to kernel via EBX register
    ; Memory map is at 0x80000, kernel can access it there
    mov ebx, 0x80000    ; Memory map address
    
    ; Jump to kernel (kernel_main will receive memory map pointer)
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

kernel_load_error:
    mov si, msg_kernel_error
    call print_string_real
    cli
    hlt

msg_loading:         db 'Stage 2 Loaded...', 0xd, 0xa, 0
msg_loading_kernel:  db 'Loading kernel from disk...', 0xd, 0xa, 0
msg_kernel_loaded:   db 'Kernel loaded successfully!', 0xd, 0xa, 0
msg_detecting_memory: db 'Detecting memory...', 0xd, 0xa, 0
msg_memory_detected:  db 'Memory map created!', 0xd, 0xa, 0
msg_pm:               db 'Switching to Protected Mode...', 0
msg_kernel_error:     db 'Kernel load error!', 0xd, 0xa, 0