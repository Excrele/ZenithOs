# Zenith OS - Current Capabilities & Future Feature Goals

## 📊 Current OS Capabilities Analysis

### ✅ **Core Infrastructure (Complete)**
Zenith OS has a solid foundation with the following fully implemented systems:

#### **1. Boot System**
- ✅ Two-stage bootloader (Stage 1: MBR, Stage 2: Protected mode loader)
- ✅ Memory detection via E820 BIOS calls
- ✅ Kernel loading from disk (ATA/IDE)
- ✅ Protected mode transition with GDT setup

#### **2. Memory Management**
- ✅ Physical Memory Manager (PMM) - bitmap-based page frame allocator
- ✅ Virtual Memory (Paging) - 4KB pages, page directories/tables
- ✅ Kernel heap allocator (kmalloc/kfree) - linked list implementation
- ✅ User heap allocator (brk/sbrk) - per-process heap management
- ✅ Page fault handling
- ✅ Memory protection (basic)

#### **3. Process & Task Management**
- ✅ Process Control Block (PCB) with full state management
- ✅ Multi-tasking with round-robin scheduler
- ✅ Context switching (full register save/restore)
- ✅ Process states: NEW, READY, RUNNING, BLOCKED, TERMINATED
- ✅ Process creation/destruction
- ✅ Parent-child process relationships
- ✅ Process priority support (structure in place)

#### **4. System Call Interface**
- ✅ Software interrupt mechanism (INT 0x80)
- ✅ System call dispatcher
- ✅ **28 implemented system calls:**
  - Process: `exit`, `fork`, `exec`, `wait`, `getpid`, `kill`
  - File I/O: `open`, `close`, `read`, `write`, `seek`, `unlink`
  - Directory: `mkdir`, `rmdir`, `readdir`
  - Memory: `brk`, `sbrk`
  - IPC: `pipe`, `msgget`, `msgsnd`, `msgrcv`, `msgctl`, `shmget`, `shmat`, `shmdt`, `shmctl`
  - Signals: `signal`, `kill`

#### **5. File System**
- ✅ Virtual File System (VFS) abstraction layer
- ✅ Simple file system implementation
- ✅ Directory operations (mkdir, rmdir, readdir)
- ✅ File operations (open, close, read, write, seek, unlink)
- ✅ File metadata (size, permissions, timestamps)
- ✅ ATA/IDE block device driver

#### **6. Device Drivers**
- ✅ VGA text mode driver (80x25, scrolling, cursor, colors)
- ✅ PS/2 keyboard driver (scan code to ASCII, special keys)
- ✅ Serial port driver (COM1/COM2, debugging)
- ✅ ATA/IDE disk driver (PIO mode, sector read/write)

#### **7. Inter-Process Communication (IPC)**
- ✅ Pipes (unidirectional communication)
- ✅ Message queues (SysV-style)
- ✅ Shared memory (SysV-style)
- ✅ Signals (basic signal handling)

#### **8. User Space Support**
- ✅ Ring 3 (user mode) execution
- ✅ Separate page tables per process
- ✅ User stack setup
- ✅ Privilege level separation
- ✅ ELF loader for executable files

#### **9. Shell & Command Line**
- ✅ Interactive shell with command parsing
- ✅ Built-in commands: `help`, `clear`, `echo`, `ls`, `cat`, `mkdir`, `rmdir`, `ps`, `exit`
- ✅ Keyboard input handling
- ✅ Command execution

#### **10. Interrupt & Exception Handling**
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Programmable Interrupt Controller (PIC) remapping
- ✅ Exception handlers (0-20)
- ✅ IRQ handlers (0-15)
- ✅ Timer interrupt (PIT, 100Hz)

---

## 🎯 Future Feature Goals

### **Phase 1: User Program Infrastructure** (Prerequisites)

Before building user programs, we need to establish the foundation:

#### **1.1 Standard C Library (libc)**
**Priority: CRITICAL**

User programs need a standard library to be practical. This includes:

- **String Functions:**
  - `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`
  - `strcmp`, `strncmp`, `strstr`, `strchr`, `strrchr`
  - `strtok`, `strdup`, `memset`, `memcpy`, `memmove`, `memcmp`

- **Memory Management:**
  - `malloc`, `free`, `calloc`, `realloc` (wrapper around sbrk)

- **I/O Functions:**
  - `printf`, `sprintf`, `snprintf`, `vprintf`
  - `scanf`, `fscanf`, `sscanf`
  - `putchar`, `getchar`, `puts`, `gets`
  - `fopen`, `fclose`, `fread`, `fwrite`, `fseek`, `ftell`

- **System Call Wrappers:**
  - `open`, `close`, `read`, `write`, `lseek`
  - `fork`, `exec`, `wait`, `exit`
  - `mkdir`, `rmdir`, `unlink`

- **Utility Functions:**
  - `atoi`, `itoa`, `atol`, `ltoa`
  - `abs`, `rand`, `srand`

**Implementation Notes:**
- Create `user/libc/` directory structure
- Implement minimal versions first, expand later
- Use system calls for I/O operations
- Link statically with user programs

#### **1.2 User Program Build System**
**Priority: CRITICAL**

- **Build Infrastructure:**
  - Cross-compiler toolchain setup for user programs
  - Makefile for compiling user programs
  - Linker script for user space (separate from kernel)
  - ELF generation for user executables

- **Program Loading:**
  - Enhance ELF loader to handle user programs
  - Set up proper user space memory layout
  - Initialize user stack with argc/argv
  - Handle dynamic linking (future)

- **Program Storage:**
  - Store user programs on disk (file system)
  - Create `/bin/` directory for system programs
  - Create `/usr/bin/` for user programs
  - Executable file format support

#### **1.3 Time & Date Functions**
**Priority: HIGH**

For calendar and other time-based programs:

- **RTC (Real-Time Clock) Integration:**
  - Read RTC via CMOS ports
  - Parse BCD time/date format
  - Convert to Unix timestamp format
  - Handle timezone (basic support)

- **Time System Calls:**
  - `time()` - Get current time
  - `gettimeofday()` - Get time with microseconds
  - `settimeofday()` - Set system time

- **Time Library Functions:**
  - `ctime()`, `localtime()`, `gmtime()`
  - `strftime()`, `mktime()`
  - `difftime()`

#### **1.4 Enhanced I/O & Terminal**
**Priority: MEDIUM**

- **Terminal Control:**
  - ANSI escape sequences support
  - Cursor positioning
  - Color output
  - Screen clearing/management

- **File Descriptor Management:**
  - Standard file descriptors (stdin=0, stdout=1, stderr=2)
  - File descriptor table per process
  - Redirection support (future)

- **Buffered I/O:**
  - `FILE*` structure for buffered streams
  - `fopen()`, `fclose()`, `fread()`, `fwrite()`
  - Line buffering for interactive programs

---

### **Phase 2: Basic User Programs**

Once infrastructure is in place, we can build practical applications:

#### **2.1 Calculator Program** (`calc`)
**Priority: HIGH**  
**Complexity: LOW**

A simple command-line calculator that evaluates arithmetic expressions.

**Features:**
- Basic arithmetic: `+`, `-`, `*`, `/`, `%`
- Parentheses support
- Floating-point operations (if FPU support added)
- Expression parsing (recursive descent or shunting yard)
- Error handling for invalid expressions

**Implementation:**
- File: `user/programs/calc.c`
- Binary: `/bin/calc`
- Uses: libc (printf, scanf, atof, etc.)

**Example Usage:**
```
zenith> calc "2 + 3 * 4"
14
zenith> calc "(10 + 5) / 3"
5
```

#### **2.2 Calendar Program** (`cal`)
**Priority: HIGH**  
**Complexity: MEDIUM**

Display a calendar for a given month/year.

**Features:**
- Display current month calendar
- Display specific month/year: `cal 12 2024`
- Highlight current date (if RTC available)
- Month names and day headers
- Proper week alignment

**Implementation:**
- File: `user/programs/cal.c`
- Binary: `/bin/cal`
- Uses: libc, time functions, RTC system calls

**Example Usage:**
```
zenith> cal
    December 2024
Su Mo Tu We Th Fr Sa
 1  2  3  4  5  6  7
 8  9 10 11 12 13 14
15 16 17 18 19 20 21
22 23 24 25 26 27 28
29 30 31

zenith> cal 1 2025
    January 2025
...
```

#### **2.3 Text Editor** (`edit` or `vi`)
**Priority: HIGH**  
**Complexity: HIGH**

A basic text editor for creating and editing files.

**Features (Minimal Version):**
- Open/create files
- Insert mode (typing text)
- Navigation: arrow keys, home, end
- Delete characters (backspace, delete)
- Save file (`Ctrl+S` or command)
- Quit (`Ctrl+Q` or command)
- Line-based editing (simpler than full-screen)

**Features (Advanced Version):**
- Full-screen mode (clear and redraw)
- Command mode vs. insert mode (vi-like)
- Search functionality
- Line numbers
- Multiple files (tabs/buffers)
- Undo/redo (basic)

**Implementation:**
- File: `user/programs/edit.c`
- Binary: `/bin/edit`
- Uses: libc, keyboard input, file I/O, VGA for display

**Example Usage:**
```
zenith> edit myfile.txt
[Opens editor, user types text, saves with Ctrl+S]
zenith> cat myfile.txt
[Shows file contents]
```

#### **2.4 File Manager** (`filemgr` or `fm`)
**Priority: MEDIUM**  
**Complexity: MEDIUM**

A simple file browser/manager.

**Features:**
- List files in current directory
- Navigate directories (cd)
- View file contents
- Create/delete files and directories
- Copy/move files
- File information display

**Implementation:**
- File: `user/programs/filemgr.c`
- Binary: `/bin/filemgr`

#### **2.5 System Information** (`sysinfo`)
**Priority: LOW**  
**Complexity: LOW**

Display system information.

**Features:**
- Memory usage (total, free, used)
- Process count
- Uptime
- Kernel version
- CPU information (if available)

**Implementation:**
- File: `user/programs/sysinfo.c`
- Binary: `/bin/sysinfo`

#### **2.6 Process Manager** (`top` or `ps` enhancement)
**Priority: MEDIUM**  
**Complexity: MEDIUM**

Enhanced process listing with more details.

**Features:**
- Real-time process list
- CPU usage per process
- Memory usage per process
- Process state
- Parent-child relationships
- Sort by various criteria

**Implementation:**
- File: `user/programs/top.c`
- Binary: `/bin/top`

---

### **Phase 3: Advanced User Programs**

More sophisticated applications:

#### **3.1 Simple Shell Scripting**
**Priority: MEDIUM**  
**Complexity: MEDIUM**

- Script file support (`.sh` files)
- Basic control structures (if/else, loops)
- Variable expansion
- Command substitution

#### **3.2 File Utilities**
- `cp` - Copy files
- `mv` - Move/rename files
- `rm` - Delete files
- `find` - Search for files
- `grep` - Search text in files
- `wc` - Word/line count

#### **3.3 Network Tools** (if networking is added)
- `ping` - Network connectivity test
- `netstat` - Network statistics
- `telnet` - Remote terminal

#### **3.4 Development Tools**
- `as` - Assembler
- `ld` - Linker wrapper
- `objdump` - Object file inspector

---

### **Phase 4: Graphics & GUI** (Future)

If graphics support is added:

#### **4.1 Graphics Library**
- Framebuffer support
- Basic drawing primitives (lines, rectangles, circles)
- Text rendering
- Color management

#### **4.2 GUI Applications**
- Window manager (basic)
- Desktop environment
- Graphical file manager
- Graphical text editor
- Image viewer

---

## 📋 Implementation Roadmap

### **Short-term Goals (Next 1-2 months)**

1. **Week 1-2: Standard C Library Foundation**
   - [ ] Create `user/libc/` directory structure
   - [ ] Implement basic string functions
   - [ ] Implement memory functions (malloc/free)
   - [ ] Implement basic I/O wrappers (printf, scanf)
   - [ ] Create libc build system

2. **Week 3: User Program Build System**
   - [ ] Set up user program Makefile
   - [ ] Create user space linker script
   - [ ] Test compiling a simple "Hello World" program
   - [ ] Verify ELF loading works for user programs

3. **Week 4: Time & Date Support**
   - [ ] Implement RTC driver
   - [ ] Add time system calls
   - [ ] Implement time library functions
   - [ ] Test with simple time display program

4. **Week 5-6: First User Program - Calculator**
   - [ ] Implement expression parser
   - [ ] Add arithmetic operations
   - [ ] Create calculator program
   - [ ] Test and debug

5. **Week 7-8: Calendar Program**
   - [ ] Implement calendar display logic
   - [ ] Add month/year navigation
   - [ ] Integrate with time functions
   - [ ] Polish output formatting

### **Medium-term Goals (2-4 months)**

6. **Month 3: Text Editor**
   - [ ] Design editor architecture
   - [ ] Implement basic editing (insert, delete)
   - [ ] Add file I/O
   - [ ] Add navigation
   - [ ] Add save/load functionality

7. **Month 4: Additional Utilities**
   - [ ] File manager
   - [ ] System information tool
   - [ ] Enhanced process manager
   - [ ] File utilities (cp, mv, rm)

### **Long-term Goals (4+ months)**

8. **Advanced Features**
   - [ ] Shell scripting support
   - [ ] More development tools
   - [ ] Graphics support (if desired)
   - [ ] Network tools (if networking added)

---

## 🔧 Technical Considerations

### **User Program Memory Layout**
```
0x00000000 - 0x08000000: User code (128MB)
0x08000000 - 0x10000000: User heap (128MB)
0x10000000 - 0x80000000: User stack (grows downward)
0x80000000 - 0xFFFFFFFF: Reserved/Kernel space
```

### **Program Entry Point**
- User programs should have `main(int argc, char* argv[])` entry point
- Kernel sets up argc/argv on user stack before jumping to main
- Programs call `exit()` system call to terminate

### **Linking Considerations**
- Static linking only (no dynamic libraries initially)
- Link against libc statically
- Position-independent code not required (but could be added)

### **File System Structure**
```
/
├── bin/          # System programs
│   ├── calc
│   ├── cal
│   ├── edit
│   └── ...
├── usr/
│   └── bin/      # User programs
├── etc/          # Configuration files
├── home/         # User home directories
└── tmp/          # Temporary files
```

---

## 📝 Notes

- Start with minimal implementations, enhance later
- Focus on functionality over features initially
- Test each component thoroughly before moving to next
- Keep user programs simple and focused
- Document APIs and usage as you go
- Consider backward compatibility when adding features

---

## 🎉 Success Criteria

The OS will be considered successful when:
- ✅ Users can compile and run custom C programs
- ✅ Basic utilities (calc, cal, edit) are functional
- ✅ Programs can read/write files reliably
- ✅ Multiple programs can run simultaneously
- ✅ System is stable and doesn't crash during normal use

---

**Last Updated:** Based on current OS state analysis  
**Next Review:** After Phase 1 completion

