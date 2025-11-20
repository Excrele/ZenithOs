# Zenith OS Development Roadmap

This roadmap outlines the path from the current minimal kernel to a fully functional operating system.

---

## Phase 1: Foundation & Bootloader Completion
**Goal:** Fix bootloader issues and establish a solid foundation

### 1.1 Bootloader Improvements
- [ ] **Fix kernel loading in Stage 2**
  - Implement proper disk I/O in protected mode (use BIOS interrupts via real-mode trampoline or ATA PIO)
  - Load kernel from disk sectors instead of hardcoded jump
  - Add error handling for disk read failures
  - Verify kernel integrity (checksum or magic number)

- [ ] **Add bootloader features**
  - Support multiple boot devices (floppy, hard disk, USB)
  - Boot menu/configuration support
  - Boot parameters passing to kernel
  - Multiboot specification compliance (optional, for GRUB compatibility)

### 1.2 Memory Detection & Setup
- [ ] **Detect available memory**
  - Use BIOS INT 0x15, EAX=0xE820 (memory map)
  - Parse and store memory map (usable, reserved, ACPI, etc.)
  - Handle memory holes and reserved regions

- [ ] **Basic memory management**
  - Implement physical memory manager (bitmap or linked list)
  - Page frame allocator
  - Mark kernel memory as reserved

---

## Phase 2: Core Kernel Infrastructure
**Goal:** Essential kernel services for system operation

### 2.1 Interrupt Handling
- [ ] **Set up Interrupt Descriptor Table (IDT)**
  - Create IDT with exception handlers
  - Implement interrupt service routines (ISRs)
  - Handle CPU exceptions (divide by zero, page fault, etc.)

- [ ] **Programmable Interrupt Controller (PIC)**
  - Remap PIC (IRQ 0-15)
  - Implement IRQ handlers
  - Enable/disable interrupts properly

- [ ] **Timer & Clock**
  - Program PIT (Programmable Interval Timer) or use APIC timer
  - Implement system tick counter
  - Sleep/delay functions
  - Time-of-day tracking (RTC integration)

### 2.2 Memory Management
- [ ] **Virtual Memory (Paging)**
  - Set up page directory and page tables
  - Enable paging in CR0
  - Implement page fault handler
  - Map kernel space (identity mapping or higher half)
  - Map/unmap pages dynamically

- [ ] **Heap Management**
  - Implement kernel heap allocator (kmalloc/kfree)
  - Use algorithms like buddy system, slab allocator, or simple linked list
  - Handle fragmentation
  - Memory leak detection (optional)

### 2.3 I/O & Device Communication
- [ ] **VGA Text Mode Driver**
  - Scrolling support
  - Cursor management
  - Color support
  - Multiple virtual terminals (optional)

- [ ] **Serial Port Driver**
  - COM1/COM2 initialization
  - Serial output for debugging
  - Serial input handling

- [ ] **Keyboard Driver**
  - PS/2 keyboard initialization
  - Scan code to ASCII conversion
  - Key press/release events
  - Special keys (Ctrl, Alt, Shift)

---

## Phase 3: Process & Task Management
**Goal:** Multi-tasking capabilities

### 3.1 Process Management
- [ ] **Task Control Block (TCB/PCB)**
  - Process structure (PID, state, registers, memory map)
  - Process creation and destruction
  - Process list management

- [ ] **Context Switching**
  - Save/restore CPU registers
  - Switch page directories
  - Implement scheduler
  - Round-robin or priority-based scheduling

- [ ] **Process States**
  - Running, ready, blocked, terminated
  - State transitions
  - Process queues

### 3.2 Threading (Optional but Recommended)
- [ ] **Kernel threads**
  - Thread structure
  - Thread creation/destruction
  - Thread synchronization primitives

### 3.3 Inter-Process Communication (IPC)
- [ ] **Basic IPC mechanisms**
  - Pipes
  - Message queues
  - Shared memory
  - Signals (basic)

---

## Phase 4: System Calls & User Space
**Goal:** Separation between kernel and user programs

### 4.1 System Call Interface
- [ ] **System call mechanism**
  - Software interrupt (INT 0x80) or SYSCALL/SYSENTER
  - System call dispatcher
  - Parameter passing (registers or stack)
  - Return value handling

- [ ] **Core system calls**
  - `exit()` - Process termination
  - `fork()` / `exec()` - Process creation
  - `wait()` - Process synchronization
  - `read()` / `write()` - I/O operations
  - `open()` / `close()` - File operations
  - `brk()` / `sbrk()` - Memory management
  - `getpid()` - Process information

### 4.2 User Mode & Privilege Levels
- [ ] **Ring 3 (User Mode) support**
  - Switch to user mode
  - User mode stack setup
  - Return to kernel mode
  - Privilege level checks

- [ ] **User space memory management**
  - Separate page tables for each process
  - User heap allocator
  - Stack growth handling
  - Memory protection (read-only, no-execute)

### 4.3 ELF Loader
- [ ] **ELF file format support**
  - Parse ELF headers
  - Load ELF segments into memory
  - Relocation handling
  - Dynamic linking (advanced)

---

## Phase 5: File System
**Goal:** Persistent storage and file management

### 5.1 Virtual File System (VFS)
- [ ] **VFS abstraction layer**
  - File system interface (open, read, write, close, seek)
  - Inode concept
  - Directory operations
  - Mount points

### 5.2 File System Implementation
- [ ] **Simple file system (choose one)**
  - **Option A:** FAT12/FAT16 (simple, well-documented)
  - **Option B:** Ext2-like (more features)
  - **Option C:** Custom minimal FS (learning experience)

- [ ] **File system features**
  - Directory structure
  - File creation/deletion
  - File metadata (size, permissions, timestamps)
  - Symbolic links (optional)

### 5.3 Block Device Driver
- [ ] **ATA/IDE driver**
  - ATA PIO mode
  - Read/write sectors
  - Device detection
  - Error handling

- [ ] **AHCI driver** (optional, for SATA)
  - More complex but modern
  - Better performance

---

## Phase 6: Advanced Features
**Goal:** Modern OS capabilities

### 6.1 Advanced Memory Management
- [ ] **Copy-on-Write (COW)**
  - Optimize fork() performance
  - Page sharing

- [ ] **Demand paging**
  - Lazy loading
  - Swap space (optional)

- [ ] **Memory protection**
  - Read-only pages
  - No-execute (NX) bit
  - Stack canaries (security)

### 6.2 Synchronization Primitives
- [ ] **Locks and synchronization**
  - Spinlocks
  - Mutexes
  - Semaphores
  - Condition variables
  - Read-write locks

- [ ] **Deadlock prevention**
  - Lock ordering
  - Timeout mechanisms

### 6.3 Advanced Scheduling
- [ ] **Scheduling algorithms**
  - Priority scheduling
  - Multilevel feedback queue
  - Real-time scheduling (optional)

- [ ] **Load balancing**
  - Multi-core support (SMP)
  - CPU affinity

---

## Phase 7: Device Drivers & Hardware Support
**Goal:** Support for common hardware

### 7.1 Input/Output Devices
- [ ] **Mouse driver** (PS/2)
  - Mouse packet parsing
  - Cursor movement
  - Button clicks

- [ ] **VGA/VESA framebuffer** (optional)
  - Graphics mode support
  - Pixel drawing
  - Basic graphics primitives

### 7.2 Storage Devices
- [ ] **USB support** (advanced)
  - USB stack
  - USB mass storage
  - USB keyboard/mouse

- [ ] **CD-ROM driver** (optional)

### 7.3 Network Stack (Advanced)
- [ ] **Network interface**
  - Ethernet driver (e.g., RTL8139, Intel E1000)
  - Packet reception/transmission

- [ ] **Network protocols**
  - Ethernet frame handling
  - ARP (Address Resolution Protocol)
  - IP (Internet Protocol)
  - ICMP (ping support)
  - TCP/UDP (advanced)
  - Socket API

---

## Phase 8: System Services & Utilities
**Goal:** User-facing functionality

### 8.1 Shell & Command Line
- [ ] **Basic shell**
  - Command parsing
  - Built-in commands (cd, ls, cat, echo, etc.)
  - Pipes and redirection
  - Background processes

- [ ] **System utilities**
  - `ls` - List directory
  - `cat` - Display file
  - `echo` - Print text
  - `mkdir` / `rmdir` - Directory operations
  - `rm` - Delete file
  - `cp` / `mv` - File operations
  - `ps` - Process list
  - `kill` - Signal processes

### 8.2 System Libraries
- [ ] **Standard C library (libc)**
  - String functions (strlen, strcpy, etc.)
  - Memory functions (memset, memcpy, etc.)
  - Math functions
  - I/O functions (printf, scanf, etc.)

- [ ] **System libraries**
  - Math library
  - Time/date functions

### 8.3 Init System
- [ ] **Init process**
  - First user process (PID 1)
  - Service management
  - Runlevels (optional)

---

## Phase 9: Security & Stability
**Goal:** Production-ready features

### 9.1 Security Features
- [ ] **User authentication**
  - User accounts
  - Password system
  - Permissions (read, write, execute)

- [ ] **Access control**
  - File permissions (rwx)
  - User/group ownership
  - Setuid/setgid (optional)

- [ ] **Kernel protection**
  - Prevent user space from accessing kernel memory
  - System call validation
  - Buffer overflow protection

### 9.2 Error Handling & Recovery
- [ ] **Kernel panic handling**
  - Graceful error messages
  - Stack traces
  - System state dump

- [ ] **Exception recovery**
  - Handle recoverable errors
  - Kill misbehaving processes
  - Resource cleanup

### 9.3 Logging & Debugging
- [ ] **Kernel logging**
  - Log levels (debug, info, warn, error)
  - Log to file or serial
  - Circular buffer

- [ ] **Debugging tools**
  - Kernel debugger (optional)
  - Stack trace on panic
  - Memory leak detection

---

## Phase 10: Optimization & Polish
**Goal:** Performance and usability

### 10.1 Performance Optimization
- [ ] **Kernel optimization**
  - Reduce memory footprint
  - Optimize critical paths
  - Cache-friendly data structures
  - Profile and optimize

- [ ] **I/O optimization**
  - Buffering
  - Caching
  - DMA support (optional)

### 10.2 Documentation
- [ ] **Code documentation**
  - Inline comments
  - Architecture documentation
  - API documentation

- [ ] **User documentation**
  - User manual
  - Developer guide
  - Installation instructions

### 10.3 Testing
- [ ] **Testing infrastructure**
  - Unit tests for kernel components
  - Integration tests
  - Stress testing
  - Regression testing

---

## Implementation Priority Recommendations

### **Critical Path (Must Have):**
1. Fix kernel loading in bootloader
2. Interrupt handling (IDT, PIC, timer)
3. Memory management (paging, heap)
4. Basic device drivers (keyboard, VGA)
5. Process management & scheduling
6. System calls
7. User mode support
8. File system
9. Basic shell

### **Important (Should Have):**
- ELF loader
- IPC mechanisms
- Advanced scheduling
- More device drivers
- System utilities

### **Nice to Have (Advanced):**
- Networking
- Graphics support
- Multi-core (SMP)
- Advanced security features
- USB support

---

## Estimated Complexity

- **Phase 1-2:** Medium (2-4 months for experienced developer)
- **Phase 3-4:** High (3-6 months)
- **Phase 5-6:** High (4-8 months)
- **Phase 7-8:** Very High (6-12 months)
- **Phase 9-10:** Medium-High (3-6 months)

**Total estimated time:** 1.5-3 years for a single developer, depending on experience level and time commitment.

---

## Resources & References

### Essential Reading:
- Intel x86 Architecture Manuals
- OSDev.org wiki
- "Operating Systems: Three Easy Pieces" (book)
- "The Design and Implementation of the 4.3BSD UNIX Operating System"

### Useful Tools:
- QEMU (emulation)
- GDB (debugging)
- Bochs (alternative emulator)
- Hex editors for binary inspection

---

## Notes

- This roadmap is ambitious. Start with Phase 1-2 to get a working foundation.
- Many features can be implemented in parallel once the foundation is solid.
- Consider implementing a minimal version of each feature first, then enhancing it.
- Regular testing is crucial - test each feature as you implement it.
- Version control and backups are essential for OS development.

**Good luck with Zenith OS development! 🚀**

