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

kernel.bin: kernel/src/boot.s kernel/src/kernel.c kernel/src/memory.h kernel/src/pmm.h kernel/src/pmm.c kernel/src/idt.h kernel/src/idt.c kernel/src/idt_asm.s kernel/src/pic.h kernel/src/pic.c kernel/src/timer.h kernel/src/timer.c kernel/src/exceptions.c
	$(CC) $(CFLAGS) -c kernel/src/boot.s -o kernel/src/boot.o
	$(CC) $(CFLAGS) -c kernel/src/kernel.c -o kernel/src/kernel.o
	$(CC) $(CFLAGS) -c kernel/src/pmm.c -o kernel/src/pmm.o
	$(CC) $(CFLAGS) -c kernel/src/idt.c -o kernel/src/idt.o
	$(CC) $(CFLAGS) -c kernel/src/idt_asm.s -o kernel/src/idt_asm.o
	$(CC) $(CFLAGS) -c kernel/src/pic.c -o kernel/src/pic.o
	$(CC) $(CFLAGS) -c kernel/src/timer.c -o kernel/src/timer.o
	$(CC) $(CFLAGS) -c kernel/src/exceptions.c -o kernel/src/exceptions.o
	$(LD) $(LDFLAGS) -o $@ kernel/src/boot.o kernel/src/kernel.o kernel/src/pmm.o kernel/src/idt.o kernel/src/idt_asm.o kernel/src/pic.o kernel/src/timer.o kernel/src/exceptions.o
	$(OBJCOPY) -O binary $@ kernel-stripped.bin
	mv kernel-stripped.bin $@

# Run in QEMU
run: boot.img
	qemu-system-x86_64 -fda boot.img -serial stdio

clean:
	rm -f *.bin *.img *.o kernel/src/*.o

.PHONY: all run clean