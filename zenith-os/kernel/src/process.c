#include "process.h"
#include "pmm.h"
#include "paging.h"
#include "memory.h"
#include "scheduler.h"
#include "vfs.h"
#include "elf.h"

// External assembly functions
extern void save_context(process_t* proc);
extern void restore_context(process_t* proc);

// Process management globals (exported for scheduler)
process_t* g_process_list = 0;
static process_t* g_current_process = 0;
static pid_t g_next_pid = 1;
static unsigned int g_process_count = 0;

// Initialize process management
void process_init(void) {
    g_process_list = 0;
    g_current_process = 0;
    g_next_pid = 1;
    g_process_count = 0;
}

// Create a new process
process_t* process_create(const char* name, void (*entry_point)(void), unsigned int stack_size) {
    // Allocate process structure
    process_t* proc = (process_t*)pmm_alloc_page();
    if (!proc) {
        return 0;
    }
    
    // Clear process structure
    for (unsigned int i = 0; i < sizeof(process_t); i++) {
        ((unsigned char*)proc)[i] = 0;
    }
    
    // Set basic information
    proc->pid = g_next_pid++;
    proc->ppid = g_current_process ? g_current_process->pid : 0;
    proc->state = PROCESS_STATE_NEW;
    proc->priority = 0;
    proc->parent = g_current_process;
    proc->first_child = 0;
    proc->next_sibling = 0;
    proc->prev_sibling = 0;
    proc->exit_status = 0;
    
    // Copy name
    int i = 0;
    while (name[i] && i < 31) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';
    
    // Allocate page directory for process
    unsigned long long dir_phys = pmm_alloc_page();
    if (!dir_phys) {
        pmm_free_page((unsigned long long)proc);
        return 0;
    }
    
    proc->page_dir = (page_directory_t*)dir_phys;
    
    // Copy kernel mappings from current page directory
    if (g_current_process && g_current_process->page_dir) {
        // Copy first PDE (kernel space identity mapping)
        proc->page_dir->entries[0] = g_current_process->page_dir->entries[0];
    } else {
        // Initialize with kernel mappings
        page_directory_t* kernel_dir = paging_get_directory();
        if (kernel_dir) {
            proc->page_dir->entries[0] = kernel_dir->entries[0];
        }
    }
    
    // Allocate stack (default 64KB if not specified)
    if (stack_size == 0) {
        stack_size = 64 * 1024; // 64KB
    }
    
    // Round up to page size
    stack_size = (stack_size + 4095) & ~4095;
    
    // Allocate stack pages
    unsigned int stack_virt = 0x400000; // Start at 4MB (after kernel)
    unsigned int num_stack_pages = stack_size / 4096;
    
    proc->stack_bottom = stack_virt;
    proc->stack_top = stack_virt + stack_size;
    
    for (unsigned int i = 0; i < num_stack_pages; i++) {
        unsigned long long stack_page = pmm_alloc_page();
        if (!stack_page) {
            // Failed to allocate - cleanup
            process_destroy(proc);
            return 0;
        }
        
        // Map stack page
        unsigned int virt_addr = stack_virt + (i * 4096);
        paging_map_page(virt_addr, (unsigned int)stack_page, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }
    
    // Set up initial CPU context
    proc->registers.eip = (unsigned int)entry_point;
    proc->registers.cs = 0x18;  // User code segment (Ring 3) - 0x18 = GDT_USER_CS
    proc->registers.ds = 0x20;  // User data segment (Ring 3) - 0x20 = GDT_USER_DS
    proc->registers.es = 0x20;
    proc->registers.fs = 0x20;
    proc->registers.gs = 0x20;
    proc->registers.ss = 0x20;
    
    // Set stack pointer (grows downward)
    proc->registers.esp = proc->stack_top - 16; // Leave some space
    proc->registers.esp_user = proc->registers.esp;
    
    // Initialize user heap (starts after stack, grows upward)
    proc->heap_start = proc->stack_top; // Heap starts after stack
    proc->heap_end = proc->heap_start;  // Initially empty
    
    // Set flags (interrupts enabled, IOPL 0, user mode)
    proc->registers.eflags = 0x202; // IF bit set
    
    // Set time slice (100ms = 10 ticks at 100Hz)
    proc->time_slice = 10;
    proc->total_time = 0;
    
    // Add to process list
    proc->next = g_process_list;
    if (g_process_list) {
        g_process_list->prev = proc;
    }
    g_process_list = proc;
    proc->prev = 0;
    
    // Add to parent's child list
    if (proc->parent) {
        proc->next_sibling = proc->parent->first_child;
        if (proc->parent->first_child) {
            proc->parent->first_child->prev_sibling = proc;
        }
        proc->parent->first_child = proc;
    }
    
    g_process_count++;
    
    // Set state to ready
    proc->state = PROCESS_STATE_READY;
    
    return proc;
}

// Destroy a process
void process_destroy(process_t* proc) {
    if (!proc) {
        return;
    }
    
    // Free stack pages
    if (proc->stack_bottom) {
        unsigned int stack_size = proc->stack_top - proc->stack_bottom;
        unsigned int num_pages = stack_size / 4096;
        
        for (unsigned int i = 0; i < num_pages; i++) {
            unsigned int virt_addr = proc->stack_bottom + (i * 4096);
            unsigned int phys_addr = paging_get_physical(virt_addr);
            if (phys_addr) {
                paging_unmap_page(virt_addr);
                pmm_free_page(phys_addr);
            }
        }
    }
    
    // Free page directory (but keep kernel mappings)
    if (proc->page_dir) {
        // Free page tables (except first one which is shared)
        for (int i = 1; i < 1024; i++) {
            if (proc->page_dir->entries[i] & PAGE_PRESENT) {
                unsigned int table_phys = proc->page_dir->entries[i] & ~0xFFF;
                pmm_free_page(table_phys);
            }
        }
        // Free page directory itself
        pmm_free_page((unsigned long long)proc->page_dir);
    }
    
    // Remove from parent's child list
    if (proc->parent) {
        if (proc->prev_sibling) {
            proc->prev_sibling->next_sibling = proc->next_sibling;
        } else {
            proc->parent->first_child = proc->next_sibling;
        }
        if (proc->next_sibling) {
            proc->next_sibling->prev_sibling = proc->prev_sibling;
        }
    }
    
    // Remove from process list
    if (proc->prev) {
        proc->prev->next = proc->next;
    } else {
        g_process_list = proc->next;
    }
    
    if (proc->next) {
        proc->next->prev = proc->prev;
    }
    
    // Free process structure
    pmm_free_page((unsigned long long)proc);
    
    g_process_count--;
}

// Get current running process
process_t* process_get_current(void) {
    return g_current_process;
}

// Get process by PID
process_t* process_get_by_pid(pid_t pid) {
    process_t* proc = g_process_list;
    while (proc) {
        if (proc->pid == pid) {
            return proc;
        }
        proc = proc->next;
    }
    return 0;
}

// Switch to a different process (context switch)
// This is called from the scheduler
void process_switch(process_t* new_process) {
    if (!new_process || new_process->state != PROCESS_STATE_READY) {
        return;
    }
    
    process_t* old_process = g_current_process;
    
    // Save old process context (if any)
    if (old_process) {
        save_context(old_process);
        old_process->state = PROCESS_STATE_READY;
    }
    
    // Switch to new process
    g_current_process = new_process;
    new_process->state = PROCESS_STATE_RUNNING;
    new_process->time_slice = 10; // Reset time slice
    
    // Switch page directory
    paging_switch_directory(new_process->page_dir);
    
    // Restore CPU context
    restore_context(new_process);
}

// Yield CPU to next process
void process_yield(void) {
    // Trigger scheduler
    process_t* next = scheduler_get_next();
    if (next && next != g_current_process) {
        process_switch(next);
    }
}

// Block current process
void process_block(void) {
    if (g_current_process) {
        g_current_process->state = PROCESS_STATE_BLOCKED;
        process_yield();
    }
}

// Unblock a process
void process_unblock(process_t* proc) {
    if (proc && proc->state == PROCESS_STATE_BLOCKED) {
        proc->state = PROCESS_STATE_READY;
        scheduler_schedule(proc);
    }
}

// Exit current process
void process_exit(unsigned int exit_code) {
    if (g_current_process) {
        g_current_process->exit_code = exit_code;
        g_current_process->exit_status = exit_code;
        g_current_process->state = PROCESS_STATE_TERMINATED;
        
        // Unblock parent if waiting
        if (g_current_process->parent && g_current_process->parent->state == PROCESS_STATE_BLOCKED) {
            process_unblock(g_current_process->parent);
        }
        
        // Switch to next process
        process_t* next = scheduler_get_next();
        if (next) {
            process_switch(next);
        }
        
        // Destroy terminated process
        process_t* to_destroy = g_current_process;
        g_current_process = 0;
        process_destroy(to_destroy);
    }
}

// Get process count
unsigned int process_get_count(void) {
    return g_process_count;
}

// Fork current process (create a copy)
pid_t process_fork(void) {
    process_t* parent = g_current_process;
    if (!parent) {
        return -1; // No current process
    }
    
    // Create new process (child)
    process_t* child = (process_t*)pmm_alloc_page();
    if (!child) {
        return -1; // Out of memory
    }
    
    // Copy parent's process structure
    for (unsigned int i = 0; i < sizeof(process_t); i++) {
        ((unsigned char*)child)[i] = ((unsigned char*)parent)[i];
    }
    
    // Set child-specific fields
    child->pid = g_next_pid++;
    child->ppid = parent->pid;
    child->parent = parent;
    child->state = PROCESS_STATE_READY;
    child->first_child = 0;
    child->next_sibling = 0;
    child->prev_sibling = 0;
    
    // Allocate new page directory for child
    unsigned long long dir_phys = pmm_alloc_page();
    if (!dir_phys) {
        pmm_free_page((unsigned long long)child);
        return -1;
    }
    
    child->page_dir = (page_directory_t*)dir_phys;
    
    // Copy kernel mappings
    child->page_dir->entries[0] = parent->page_dir->entries[0];
    
    // Copy parent's page tables (simplified - in real OS, use copy-on-write)
    for (int i = 1; i < 1024; i++) {
        if (parent->page_dir->entries[i] & PAGE_PRESENT) {
            // Allocate new page table
            unsigned long long table_phys = pmm_alloc_page();
            if (!table_phys) {
                // Cleanup
                pmm_free_page((unsigned long long)child->page_dir);
                pmm_free_page((unsigned long long)child);
                return -1;
            }
            
            page_table_t* parent_table = (page_table_t*)(parent->page_dir->entries[i] & ~0xFFF);
            page_table_t* child_table = (page_table_t*)table_phys;
            
            // Copy page table entries
            for (int j = 0; j < 1024; j++) {
                if (parent_table->entries[j] & PAGE_PRESENT) {
                    // Allocate new physical page
                    unsigned long long page_phys = pmm_alloc_page();
                    if (!page_phys) {
                        // Cleanup
                        pmm_free_page((unsigned long long)child_table);
                        pmm_free_page((unsigned long long)child->page_dir);
                        pmm_free_page((unsigned long long)child);
                        return -1;
                    }
                    
                    // Copy page data
                    unsigned int parent_virt = (i << 22) | (j << 12);
                    unsigned int child_virt = parent_virt;
                    unsigned int parent_phys = parent_table->entries[j] & ~0xFFF;
                    unsigned char* src = (unsigned char*)parent_phys;
                    unsigned char* dst = (unsigned char*)page_phys;
                    
                    for (int k = 0; k < 4096; k++) {
                        dst[k] = src[k];
                    }
                    
                    // Map in child's page table
                    child_table->entries[j] = (unsigned int)page_phys | 
                                             (parent_table->entries[j] & 0xFFF);
                }
            }
            
            // Set up page directory entry
            child->page_dir->entries[i] = (unsigned int)table_phys | 
                                          (parent->page_dir->entries[i] & 0xFFF);
        }
    }
    
    // Add to process list
    child->next = g_process_list;
    if (g_process_list) {
        g_process_list->prev = child;
    }
    g_process_list = child;
    child->prev = 0;
    
    // Add to parent's child list
    child->next_sibling = parent->first_child;
    if (parent->first_child) {
        parent->first_child->prev_sibling = child;
    }
    parent->first_child = child;
    
    g_process_count++;
    
    // Schedule child
    scheduler_schedule(child);
    
    // Return child PID in parent, 0 in child
    // This will be set by the context switch
    return child->pid;
}

// Execute a new program (replace current process)
int process_exec(const char* path, char* const argv[]) {
    process_t* proc = g_current_process;
    if (!proc) {
        return -1;
    }
    
    // Open file
    file_descriptor_t fd = vfs_open(path, O_RDONLY);
    if (fd < 0) {
        return -1; // File not found
    }
    
    // Read file into memory (simplified - assume it fits in one page)
    unsigned char* file_buffer = (unsigned char*)pmm_alloc_page();
    if (!file_buffer) {
        vfs_close(fd);
        return -1;
    }
    
    int file_size = vfs_read(fd, file_buffer, 4096);
    vfs_close(fd);
    
    if (file_size <= 0) {
        pmm_free_page((unsigned long long)file_buffer);
        return -1;
    }
    
    // Load ELF executable
    unsigned int entry_point = elf_load(file_buffer, file_size);
    pmm_free_page((unsigned long long)file_buffer);
    
    if (!entry_point) {
        return -1; // Failed to load ELF
    }
    
    // Set up argc/argv on user stack
    // Stack layout (growing downward):
    // [strings...] [argv[0]] [argv[1]] ... [argv[n]] [NULL] [argc] [padding]
    
    // Helper function to get string length
    static int str_len(const char* s) {
        int len = 0;
        while (s[len]) len++;
        return len;
    }
    
    unsigned int stack_ptr = proc->stack_top;
    unsigned int argc = 0;
    
    // Count arguments
    if (argv) {
        while (argv[argc]) {
            argc++;
        }
    }
    
    // Calculate total space needed for strings
    unsigned int total_string_size = 0;
    if (argv && argc > 0) {
        for (int i = 0; i < argc; i++) {
            if (argv[i]) {
                total_string_size += str_len(argv[i]) + 1;
            }
        }
    } else {
        // At least program name
        total_string_size = str_len(path) + 1;
        argc = 1;
    }
    
    // Align string area
    total_string_size = (total_string_size + 3) & ~3;
    
    // Reserve space for strings (at top of stack area)
    stack_ptr -= total_string_size;
    unsigned int string_base = stack_ptr;
    
    // Reserve space for argv array
    stack_ptr -= (argc + 1) * sizeof(char*);
    char** argv_ptr = (char**)stack_ptr;
    
    // Reserve space for argc
    stack_ptr -= sizeof(int);
    int* argc_ptr = (int*)stack_ptr;
    *argc_ptr = argc;
    
    // Store argument strings and set up argv pointers
    unsigned int string_offset = 0;
    
    if (argv && argc > 0) {
        for (int i = 0; i < argc; i++) {
            if (argv[i]) {
                int len = str_len(argv[i]);
                char* str_dest = (char*)(string_base + string_offset);
                
                // Copy string
                for (int j = 0; j <= len; j++) {  // Include null terminator
                    str_dest[j] = argv[i][j];
                }
                
                // Set argv pointer (relative to stack base)
                argv_ptr[i] = (char*)(string_base + string_offset);
                string_offset += len + 1;
                // Align
                string_offset = (string_offset + 3) & ~3;
            } else {
                argv_ptr[i] = 0;
            }
        }
    } else {
        // No arguments - just set argv[0] to program name
        char* prog_name = (char*)string_base;
        int i = 0;
        while (path[i]) {
            prog_name[i] = path[i];
            i++;
        }
        prog_name[i] = '\0';
        argv_ptr[0] = (char*)string_base;
    }
    argv_ptr[argc] = 0;  // NULL terminator
    
    // Align stack to 16 bytes
    stack_ptr = (stack_ptr & ~0xF);
    
    // Update process entry point and stack
    proc->registers.eip = entry_point;
    proc->registers.esp = stack_ptr;
    proc->registers.esp_user = stack_ptr;
    
    // Copy name
    int i = 0;
    while (path[i] && i < 31) {
        proc->name[i] = path[i];
        i++;
    }
    proc->name[i] = '\0';
    
    // Never returns - process continues from new entry point
    return 0;
}

// Wait for a child process to exit
pid_t process_wait(int* status) {
    process_t* parent = g_current_process;
    if (!parent) {
        return -1;
    }
    
    // Find a terminated child
    process_t* child = parent->first_child;
    while (child) {
        if (child->state == PROCESS_STATE_TERMINATED) {
            pid_t child_pid = child->pid;
            if (status) {
                *status = child->exit_status;
            }
            
            // Remove child from list
            process_destroy(child);
            
            return child_pid;
        }
        child = child->next_sibling;
    }
    
    // No terminated child found - block until one exits
    process_block();
    
    // Try again after being unblocked
    return process_wait(status);
}

// Set program break (end of data segment)
unsigned int process_brk(unsigned int new_break) {
    process_t* proc = g_current_process;
    if (!proc) {
        return 0;
    }
    
    // Validate new break address
    if (new_break < proc->heap_start) {
        return proc->heap_end; // Invalid - return current break
    }
    
    // Maximum heap size: 16MB
    unsigned int max_heap = proc->heap_start + (16 * 1024 * 1024);
    if (new_break > max_heap) {
        return proc->heap_end; // Too large - return current break
    }
    
    // If expanding heap, allocate and map new pages
    if (new_break > proc->heap_end) {
        unsigned int old_end = proc->heap_end;
        unsigned int new_end = new_break;
        
        // Round up to page boundaries
        old_end = (old_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        new_end = (new_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        
        // Allocate and map pages
        for (unsigned int addr = old_end; addr < new_end; addr += PAGE_SIZE) {
            unsigned long long page_phys = pmm_alloc_page();
            if (!page_phys) {
                // Out of memory - return current break
                return proc->heap_end;
            }
            
            // Map page in process's address space
            if (paging_map_page(addr, (unsigned int)page_phys, 
                               PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER) != 0) {
                pmm_free_page(page_phys);
                return proc->heap_end;
            }
            
            // Clear the page
            unsigned char* page = (unsigned char*)addr;
            for (int i = 0; i < PAGE_SIZE; i++) {
                page[i] = 0;
            }
        }
    } else if (new_break < proc->heap_end) {
        // Shrinking heap - unmap and free pages
        unsigned int old_end = proc->heap_end;
        unsigned int new_end = new_break;
        
        // Round to page boundaries
        old_end = (old_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        new_end = (new_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        
        // Unmap and free pages
        for (unsigned int addr = new_end; addr < old_end; addr += PAGE_SIZE) {
            unsigned int phys_addr = paging_get_physical(addr);
            if (phys_addr) {
                paging_unmap_page(addr);
                pmm_free_page(phys_addr);
            }
        }
    }
    
    proc->heap_end = new_break;
    return proc->heap_end;
}

// Change program break by increment
int process_sbrk(int increment) {
    process_t* proc = g_current_process;
    if (!proc) {
        return -1;
    }
    
    unsigned int new_break = proc->heap_end + increment;
    unsigned int result = process_brk(new_break);
    
    if (result == proc->heap_end && increment != 0) {
        return -1; // Failed
    }
    
    return (int)result;
}

