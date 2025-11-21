# Zenith OS User Programs

This directory contains user-space programs for Zenith OS.

## Available Programs

### 1. hello
Simple Hello World program demonstrating basic libc usage.

**Usage:**
```
zenith> exec /bin/hello
```

**Features:**
- Demonstrates printf
- Shows argc/argv handling

### 2. calc
Command-line calculator that evaluates arithmetic expressions.

**Usage:**
```
zenith> exec /bin/calc "2 + 3 * 4"
zenith> exec /bin/calc "(10 + 5) / 3"
```

**Features:**
- Basic arithmetic: `+`, `-`, `*`, `/`, `%`
- Parentheses support
- Operator precedence
- Error handling (division by zero, invalid syntax)

**Examples:**
- `calc "2 + 3"` → 5
- `calc "10 - 4 * 2"` → 2 (precedence: * before -)
- `calc "(10 - 4) * 2"` → 12 (parentheses override precedence)
- `calc "15 / 3"` → 5
- `calc "17 % 5"` → 2

### 3. cal
Calendar display program.

**Usage:**
```
zenith> exec /bin/cal              # Current month (placeholder)
zenith> exec /bin/cal 2024         # Entire year
zenith> exec /bin/cal 12 2024      # Specific month (December 2024)
```

**Features:**
- Display calendar for any month/year
- Display entire year
- Leap year support
- Proper week alignment

**Examples:**
- `cal` → Shows current month (currently defaults to December 2024)
- `cal 2024` → Shows all 12 months of 2024
- `cal 1 2025` → Shows January 2025
- `cal 2 2024` → Shows February 2024 (leap year, 29 days)

### 4. sysinfo
System information display.

**Usage:**
```
zenith> exec /bin/sysinfo
```

**Features:**
- Process ID
- System information
- Placeholder for future features (memory stats, process count, uptime)

**Note:** Some features require additional kernel system calls that will be added in future updates.

## Building Programs

All programs are built using the main Makefile in the `user/` directory:

```bash
cd user
make
```

This builds:
- `build/hello.bin`
- `build/calc.bin`
- `build/cal.bin`
- `build/sysinfo.bin`

## Installing Programs

After building, copy the `.bin` files to the file system:

1. Mount the file system
2. Create `/bin` directory if it doesn't exist
3. Copy binaries:
   ```
   cp build/hello.bin /bin/hello
   cp build/calc.bin /bin/calc
   cp build/cal.bin /bin/cal
   cp build/sysinfo.bin /bin/sysinfo
   ```

## Program Structure

Each program:
- Includes libc headers
- Has a `main(int argc, char* argv[])` entry point
- Uses system calls via libc wrappers
- Returns exit code (0 = success, non-zero = error)

## Future Programs

Planned programs (see FEATURE_GOALS.md):
- `edit` - Text editor
- `filemgr` - File manager
- `top` - Enhanced process manager
- `cp`, `mv`, `rm` - File utilities
- `grep` - Text search
- And more...

## Notes

- Programs must be statically linked (no dynamic libraries yet)
- All programs use the same libc (`build/libc.a`)
- Programs run in user mode (Ring 3)
- Stack and heap are managed by the kernel

