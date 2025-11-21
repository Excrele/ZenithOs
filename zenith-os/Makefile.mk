# Zenith OS Build
CROSS_COMPILE = x86_64-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = nasm
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -ffreestanding -m32 -g -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS = -T kernel/src/linker.ld -nostdlib -m elf_i386

# Targets
all: boot.img

boot.img: stage1.bin stage2.bin kernel.bin
    cat stage1.bin stage2.bin kernel.bin > $@
    # Pad to floppy size if needed: dd if=/dev/zero of=boot.img bs=512 count=2880 conv=notrunc

stage1.bin: boot/stage1/boot.asm
	$(AS) -f bin $< -o $@

stage2.bin: boot/stage2/stage2.asm
	$(AS) -f bin $< -o $@

kernel.bin: kernel/src/boot.s kernel/src/kernel.c kernel/src/memory.h kernel/src/pmm.h kernel/src/pmm.c kernel/src/idt.h kernel/src/idt.c kernel/src/idt_asm.s kernel/src/pic.h kernel/src/pic.c kernel/src/timer.h kernel/src/timer.c kernel/src/exceptions.c kernel/src/paging.h kernel/src/paging.c kernel/src/process.h kernel/src/process.c kernel/src/process_asm.s kernel/src/scheduler.h kernel/src/scheduler.c kernel/src/gdt.h kernel/src/gdt.c kernel/src/syscall.h kernel/src/syscall.c kernel/src/syscall_asm.s kernel/src/elf.h kernel/src/elf.c kernel/src/vfs.h kernel/src/vfs.c kernel/src/ata.h kernel/src/ata.c kernel/src/fs_simple.h kernel/src/fs_simple.c kernel/src/heap.h kernel/src/heap.c kernel/src/keyboard.h kernel/src/keyboard.c kernel/src/vga.h kernel/src/vga.c kernel/src/shell.h kernel/src/shell.c kernel/src/ipc.h kernel/src/ipc.c kernel/src/serial.h kernel/src/serial.c
	$(CC) $(CFLAGS) -c kernel/src/boot.s -o kernel/src/boot.o
	$(CC) $(CFLAGS) -c kernel/src/kernel.c -o kernel/src/kernel.o
	$(CC) $(CFLAGS) -c kernel/src/pmm.c -o kernel/src/pmm.o
	$(CC) $(CFLAGS) -c kernel/src/idt.c -o kernel/src/idt.o
	$(CC) $(CFLAGS) -c kernel/src/idt_asm.s -o kernel/src/idt_asm.o
	$(CC) $(CFLAGS) -c kernel/src/pic.c -o kernel/src/pic.o
	$(CC) $(CFLAGS) -c kernel/src/timer.c -o kernel/src/timer.o
	$(CC) $(CFLAGS) -c kernel/src/exceptions.c -o kernel/src/exceptions.o
	$(CC) $(CFLAGS) -c kernel/src/paging.c -o kernel/src/paging.o
	$(CC) $(CFLAGS) -c kernel/src/process.c -o kernel/src/process.o
	$(CC) $(CFLAGS) -c kernel/src/process_asm.s -o kernel/src/process_asm.o
	$(CC) $(CFLAGS) -c kernel/src/scheduler.c -o kernel/src/scheduler.o
	$(CC) $(CFLAGS) -c kernel/src/gdt.c -o kernel/src/gdt.o
	$(CC) $(CFLAGS) -c kernel/src/syscall.c -o kernel/src/syscall.o
	$(CC) $(CFLAGS) -c kernel/src/syscall_asm.s -o kernel/src/syscall_asm.o
	$(CC) $(CFLAGS) -c kernel/src/elf.c -o kernel/src/elf.o
	$(CC) $(CFLAGS) -c kernel/src/vfs.c -o kernel/src/vfs.o
	$(CC) $(CFLAGS) -c kernel/src/ata.c -o kernel/src/ata.o
	$(CC) $(CFLAGS) -c kernel/src/fs_simple.c -o kernel/src/fs_simple.o
	$(CC) $(CFLAGS) -c kernel/src/heap.c -o kernel/src/heap.o
	$(CC) $(CFLAGS) -c kernel/src/keyboard.c -o kernel/src/keyboard.o
	$(CC) $(CFLAGS) -c kernel/src/vga.c -o kernel/src/vga.o
	$(CC) $(CFLAGS) -c kernel/src/shell.c -o kernel/src/shell.o
	$(CC) $(CFLAGS) -c kernel/src/ipc.c -o kernel/src/ipc.o
	$(CC) $(CFLAGS) -c kernel/src/serial.c -o kernel/src/serial.o
	$(LD) $(LDFLAGS) -o $@ kernel/src/boot.o kernel/src/kernel.o kernel/src/pmm.o kernel/src/idt.o kernel/src/idt_asm.o kernel/src/pic.o kernel/src/timer.o kernel/src/exceptions.o kernel/src/paging.o kernel/src/process.o kernel/src/process_asm.o kernel/src/scheduler.o kernel/src/gdt.o kernel/src/syscall.o kernel/src/syscall_asm.o kernel/src/elf.o kernel/src/vfs.o kernel/src/ata.o kernel/src/fs_simple.o kernel/src/heap.o kernel/src/keyboard.o kernel/src/vga.o kernel/src/shell.o kernel/src/ipc.o kernel/src/serial.o
	$(OBJCOPY) -O binary $@ kernel-stripped.bin
	mv kernel-stripped.bin $@

# Run in QEMU
run: boot.img
	qemu-system-x86_64 -fda boot.img -serial stdio

clean:
	rm -f *.bin *.img *.o kernel/src/*.o

.PHONY: all run clean