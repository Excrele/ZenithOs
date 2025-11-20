#include "memory.h"
#include "pmm.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"

// Global memory map pointer (set by bootloader at 0x80000)
memory_map_t* g_memory_map = (memory_map_t*)0x80000;

// Simple VGA text output function
static void print_string(const char* str, int row, int col) {
    char* video = (char*) 0xb8000;
    int pos = (row * 80 + col) * 2;
    for (int i = 0; str[i] != '\0'; ++i) {
        video[pos + i * 2] = str[i];
        video[pos + i * 2 + 1] = 0x0f; // White on black
    }
}

// Convert number to hex string (simple version)
static void print_hex(unsigned long long value, int row, int col) {
    char* video = (char*) 0xb8000;
    int pos = (row * 80 + col) * 2;
    const char hex_chars[] = "0123456789ABCDEF";
    
    // Print 16 hex digits
    for (int i = 15; i >= 0; i--) {
        int digit = (value >> (i * 4)) & 0xF;
        video[pos + (15 - i) * 2] = hex_chars[digit];
        video[pos + (15 - i) * 2 + 1] = 0x0f;
    }
}

// Convert number to decimal string (simple version)
static void print_decimal(unsigned long long value, int row, int col) {
    char* video = (char*) 0xb8000;
    int pos = (row * 80 + col) * 2;
    char buffer[32];
    int i = 0;
    
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        // Convert to string (reverse order)
        unsigned long long temp = value;
        while (temp > 0) {
            buffer[i++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    buffer[i] = '\0';
    
    // Reverse and print
    for (int j = i - 1; j >= 0; j--) {
        video[pos + (i - 1 - j) * 2] = buffer[j];
        video[pos + (i - 1 - j) * 2 + 1] = 0x0f;
    }
}

// Freestanding kernel entry
// EBX register contains memory map pointer (0x80000)
void kernel_main(void) {
    // Clear screen
    char* video = (char*) 0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x0f;
    }
    
    print_string("Zenith OS Kernel Loaded!", 0, 0);
    
    // Initialize Interrupt Descriptor Table
    print_string("Initializing IDT...", 1, 0);
    idt_init();
    
    // Register exception handlers
    idt_register_handler(0, exception_handler_0);
    idt_register_handler(1, exception_handler_1);
    idt_register_handler(2, exception_handler_2);
    idt_register_handler(3, exception_handler_3);
    idt_register_handler(4, exception_handler_4);
    idt_register_handler(5, exception_handler_5);
    idt_register_handler(6, exception_handler_6);
    idt_register_handler(7, exception_handler_7);
    idt_register_handler(8, exception_handler_8);
    idt_register_handler(9, exception_handler_9);
    idt_register_handler(10, exception_handler_10);
    idt_register_handler(11, exception_handler_11);
    idt_register_handler(12, exception_handler_12);
    idt_register_handler(13, exception_handler_13);
    idt_register_handler(14, exception_handler_14);
    idt_register_handler(15, exception_handler_15);
    idt_register_handler(16, exception_handler_16);
    idt_register_handler(17, exception_handler_17);
    idt_register_handler(18, exception_handler_18);
    idt_register_handler(19, exception_handler_19);
    idt_register_handler(20, exception_handler_20);
    
    // Register IRQ handlers
    idt_register_handler(32, irq_handler_0);
    idt_register_handler(33, irq_handler_1);
    idt_register_handler(34, irq_handler_2);
    idt_register_handler(35, irq_handler_3);
    idt_register_handler(36, irq_handler_4);
    idt_register_handler(37, irq_handler_5);
    idt_register_handler(38, irq_handler_6);
    idt_register_handler(39, irq_handler_7);
    idt_register_handler(40, irq_handler_8);
    idt_register_handler(41, irq_handler_9);
    idt_register_handler(42, irq_handler_10);
    idt_register_handler(43, irq_handler_11);
    idt_register_handler(44, irq_handler_12);
    idt_register_handler(45, irq_handler_13);
    idt_register_handler(46, irq_handler_14);
    idt_register_handler(47, irq_handler_15);
    
    // Remap PIC
    print_string("Remapping PIC...", 1, 20);
    pic_remap();
    
    // Initialize timer
    print_string("Initializing timer...", 1, 40);
    timer_init();
    
    // Enable interrupts
    asm volatile ("sti");
    print_string("Interrupts enabled!", 1, 60);
    
    // Initialize Physical Memory Manager
    print_string("Initializing PMM...", 1, 0);
    if (pmm_init(g_memory_map) == 0) {
        print_string("PMM initialized successfully!", 1, 0);
        
        // Display PMM statistics
        print_string("PMM Stats:", 2, 0);
        print_string("Total Pages: ", 3, 0);
        print_decimal(pmm_get_total_pages(), 3, 13);
        print_string("Free Pages: ", 4, 0);
        print_decimal(pmm_get_free_pages(), 4, 12);
        print_string("Used Pages: ", 5, 0);
        print_decimal(pmm_get_used_pages(), 5, 12);
        
        // Test allocation
        print_string("Testing allocation...", 6, 0);
        unsigned long long page1 = pmm_alloc_page();
        unsigned long long page2 = pmm_alloc_page();
        unsigned long long page3 = pmm_alloc_pages(4);
        
        if (page1 && page2 && page3) {
            print_string("Allocated pages OK", 7, 0);
            print_string("Page1: ", 8, 0);
            print_hex(page1, 8, 7);
            print_string("Page2: ", 9, 0);
            print_hex(page2, 9, 7);
            print_string("Page3 (4 pages): ", 10, 0);
            print_hex(page3, 10, 18);
            
            // Free pages
            pmm_free_page(page1);
            pmm_free_page(page2);
            pmm_free_pages(page3, 4);
            print_string("Freed pages OK", 11, 0);
            
        print_string("Free after free: ", 12, 0);
        print_decimal(pmm_get_free_pages(), 12, 17);
        
        // Display timer ticks
        print_string("Timer ticks: ", 13, 0);
        print_decimal(timer_get_ticks(), 13, 13);
    } else {
        print_string("Allocation failed!", 7, 0);
    }
    } else {
        print_string("PMM initialization failed!", 1, 0);
    }
    
    print_string("Memory Map:", 14, 0);
    
    // Display memory map entries
    if (g_memory_map && g_memory_map->count > 0) {
        int row = 16;
        for (unsigned int i = 0; i < g_memory_map->count && i < 10; i++) {
            memory_map_entry_t* entry = &g_memory_map->entries[i];
            
            // Print entry number
            char entry_num[16];
            entry_num[0] = '0' + (i % 10);
            entry_num[1] = ':';
            entry_num[2] = ' ';
            entry_num[3] = '\0';
            print_string(entry_num, row, 0);
            
            // Print base address
            print_hex(entry->base, row, 4);
            print_string("-", row, 20);
            
            // Print end address (base + length)
            print_hex(entry->base + entry->length, row, 21);
            
            // Print type
            const char* type_str = "Unknown";
            if (entry->type == MEMORY_TYPE_USABLE) type_str = "Usable";
            else if (entry->type == MEMORY_TYPE_RESERVED) type_str = "Reserved";
            else if (entry->type == MEMORY_TYPE_ACPI_RECLAIM) type_str = "ACPI Reclaim";
            else if (entry->type == MEMORY_TYPE_ACPI_NVS) type_str = "ACPI NVS";
            else if (entry->type == MEMORY_TYPE_BAD) type_str = "Bad";
            
            print_string(type_str, row, 38);
            
            row++;
            if (row >= 24) break; // Don't overflow screen
        }
        
        // Print total count
        char count_msg[32];
        count_msg[0] = 'T';
        count_msg[1] = 'o';
        count_msg[2] = 't';
        count_msg[3] = 'a';
        count_msg[4] = 'l';
        count_msg[5] = ':';
        count_msg[6] = ' ';
        count_msg[7] = '0' + (g_memory_map->count % 10);
        count_msg[8] = '\0';
        print_string(count_msg, 24, 0);
    } else {
        print_string("No memory map available", 16, 0);
    }
    
    // Main loop - update timer display
    unsigned long long last_ticks = 0;
    while (1) {
        unsigned long long current_ticks = timer_get_ticks();
        if (current_ticks != last_ticks) {
            print_string("Timer ticks: ", 13, 0);
            print_decimal(current_ticks, 13, 13);
            last_ticks = current_ticks;
        }
        asm volatile ("hlt");  // Halt until interrupt
    }
}