# Zenith OS Build
CROSS_COMPILE = x86_64-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = nasm
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -ffreestanding -m64 -g -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS = -T kernel/linker.ld -nostdlib

# Targets
all: boot.img

boot.img: stage1.bin stage2.bin kernel.bin
    cat stage1.bin stage2.bin kernel.bin > $@
    # Pad to floppy size if needed: dd if=/dev/zero of=boot.img bs=512 count=2880 conv=notrunc

stage1.bin: boot/stage1/boot.asm
	$(AS) -f bin $< -o $@

stage2.bin: boot/stage2/stage2.asm
	$(AS) -f bin $< -o $@

kernel.bin: kernel/src/boot.s kernel/src/kernel.c
	$(CC) $(CFLAGS) -c kernel/src/boot.s -o kernel/src/boot.o
	$(CC) $(CFLAGS) -c kernel/src/kernel.c -o kernel/src/kernel.o
	$(LD) $(LDFLAGS) -o $@ kernel/src/boot.o kernel/src/kernel.o
	$(OBJCOPY) -O binary $@ kernel-stripped.bin
	mv kernel-stripped.bin $@

# Run in QEMU
run: boot.img
	qemu-system-x86_64 -fda boot.img -serial stdio

clean:
	rm -f *.bin *.img *.o kernel/src/*.o

.PHONY: all run clean