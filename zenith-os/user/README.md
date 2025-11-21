# Zenith OS User Programs

This directory contains the user-space library (libc) and user programs for Zenith OS.

## Directory Structure

```
user/
├── libc/              # Standard C library
│   ├── string/        # String functions
│   ├── stdlib/        # Memory allocation, utilities
│   ├── stdio/         # I/O functions
│   ├── sys/           # System call wrappers
│   ├── crt0.s         # C runtime startup code
│   └── linker.ld      # User space linker script
├── programs/          # User programs
│   └── hello.c        # Hello World test program
├── build/             # Build output (generated)
├── Makefile           # Build system
└── README.md          # This file
```

## Building

To build the libc and user programs:

```bash
cd user
make
```

This will:
1. Compile libc components (string, stdlib, stdio)
2. Create libc archive (`build/libc.a`)
3. Build user programs (currently `hello`)

Output files:
- `build/libc.a` - Standard C library archive
- `build/hello.bin` - Hello World program binary

## Using User Programs

User programs are ELF executables that can be loaded and executed by the kernel using the `exec` system call.

### Example: Running Hello World

From the shell:
```
zenith> exec /bin/hello
```

Or from a C program:
```c
char* argv[] = {"/bin/hello", NULL};
exec("/bin/hello", argv);
```

## Libc Features

### Implemented Functions

**String Functions** (`string.h`):
- `strlen`, `strcpy`, `strncpy`
- `strcat`, `strncat`
- `strcmp`, `strncmp`
- `strstr`, `strchr`, `strrchr`
- `strtok`, `strdup`
- `memset`, `memcpy`, `memmove`, `memcmp`

**Memory Functions** (`stdlib.h`):
- `malloc`, `free`, `calloc`, `realloc`
- `atoi`, `atol`, `itoa`, `ltoa`
- `abs`, `exit`

**I/O Functions** (`stdio.h`):
- `printf`, `sprintf`, `snprintf`
- `putchar`, `getchar`, `puts`
- `open`, `close`, `read`, `write`, `seek`

**System Calls** (`sys/syscall.h`):
- All system calls wrapped with `syscall()` macro

## Creating New Programs

1. Create a new `.c` file in `programs/`
2. Add build rule to `Makefile`
3. Include libc headers as needed
4. Build with `make`

Example program:
```c
#include "../libc/stdio/stdio.h"

int main(int argc, char* argv[]) {
    printf("Hello from my program!\n");
    return 0;
}
```

## Notes

- User programs run in Ring 3 (user mode)
- Stack starts at 0x80000000 and grows downward
- Code starts at 0x08000000
- Heap is managed via `brk`/`sbrk` system calls
- All I/O goes through system calls

## Future Enhancements

- [ ] More libc functions (math, time, etc.)
- [ ] Buffered I/O (FILE* streams)
- [ ] Dynamic linking support
- [ ] Shared libraries
- [ ] More user programs (calc, cal, edit, etc.)

