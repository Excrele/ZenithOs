; Stage 1: Real-mode boot sector
[org 0x7c00]          ; BIOS loads here
[bits 16]             ; Real mode

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00      ; Stack below boot sector

    ; Print "Loading..." (BIOS int 0x10)
    mov si, msg_loading
    call print_string

    ; Load stage2 from disk (sector 2, 8 sectors, to 0x7e00)
    mov ah, 0x02        ; Read sector
    mov al, 8           ; Sectors to read
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2 (after MBR)
    mov dh, 0           ; Head 0
    mov bx, 0x7e00      ; Load addr
    int 0x13            ; BIOS disk service
    jc disk_error       ; Jump if carry (error)

    ; Jump to stage2
    jmp 0x7e00

print_string:
    lodsb               ; Load byte from SI
    or al, al           ; Zero?
    jz .done
    mov ah, 0x0e        ; Teletype output
    int 0x10            ; BIOS video
    jmp print_string
.done:
    ret

disk_error:
    mov si, msg_error
    call print_string
    cli                 ; Halt on error
    hlt

msg_loading: db 'Zenith OS Loading...', 0xd, 0xa, 0
msg_error:   db 'Disk read error!', 0xd, 0xa, 0

times 510-($-$$) db 0  ; Pad to 510B
dw 0xaa55              ; Boot signature