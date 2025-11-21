# User Program Build System
CROSS_COMPILE = x86_64-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = nasm
OBJCOPY = $(CROSS_COMPILE)objcopy

# Compiler flags for user programs
CFLAGS = -ffreestanding -m32 -g -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector -I.
LDFLAGS = -T linker.ld -nostdlib -m elf_i386

# Directories
LIBC_DIR = libc
PROGRAMS_DIR = programs
BUILD_DIR = build

# Libc source files
LIBC_STRING_SRC = $(LIBC_DIR)/string/string.c
LIBC_STDLIB_SRC = $(LIBC_DIR)/stdlib/stdlib.c
LIBC_STDIO_SRC = $(LIBC_DIR)/stdio/stdio.c

# Libc object files
LIBC_STRING_OBJ = $(BUILD_DIR)/libc_string.o
LIBC_STDLIB_OBJ = $(BUILD_DIR)/libc_stdlib.o
LIBC_STDIO_OBJ = $(BUILD_DIR)/libc_stdio.o

LIBC_OBJS = $(LIBC_STRING_OBJ) $(LIBC_STDLIB_OBJ) $(LIBC_STDIO_OBJ)

# Create libc archive
LIBC_AR = $(BUILD_DIR)/libc.a

.PHONY: all clean libc programs

all: libc programs

# Build libc
libc: $(LIBC_AR)

$(LIBC_AR): $(LIBC_OBJS)
	$(CROSS_COMPILE)ar rcs $@ $^

$(LIBC_STRING_OBJ): $(LIBC_STRING_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC_STDLIB_OBJ): $(LIBC_STDLIB_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC_STDIO_OBJ): $(LIBC_STDIO_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build user programs
programs: $(BUILD_DIR)/hello.bin

# Hello World test program
$(BUILD_DIR)/hello.bin: $(PROGRAMS_DIR)/hello.c $(LIBC_AR) $(LIBC_DIR)/crt0.s
	@mkdir -p $(BUILD_DIR)
	$(AS) -f elf32 $(LIBC_DIR)/crt0.s -o $(BUILD_DIR)/crt0.o
	$(CC) $(CFLAGS) -c $(PROGRAMS_DIR)/hello.c -o $(BUILD_DIR)/hello.o
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/hello.elf $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello.o $(LIBC_AR)
	$(OBJCOPY) -O binary $(BUILD_DIR)/hello.elf $@

clean:
	rm -rf $(BUILD_DIR)

