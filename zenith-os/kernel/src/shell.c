#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "vfs.h"
#include "syscall.h"
#include "process.h"
#include "memory.h"
#include "pmm.h"
#include "paging.h"

#define SHELL_MAX_LINE 256
#define SHELL_MAX_ARGS 16

// Shell prompt
static const char* g_shell_prompt = "zenith> ";

// Command buffer
static char g_command_buffer[SHELL_MAX_LINE];
static int g_command_pos = 0;

// Simple string functions
static int strlen(const char* s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

static void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

static char* strtok(char* str, const char* delim) {
    static char* saved = 0;
    if (str) saved = str;
    if (!saved || !*saved) return 0;
    
    char* start = saved;
    while (*saved && *saved != *delim) saved++;
    if (*saved) {
        *saved = '\0';
        saved++;
    } else {
        saved = 0;
    }
    return start;
}

// Parse command line into arguments
static int parse_command(char* line, char* argv[], int max_args) {
    int argc = 0;
    char* token = strtok(line, " ");
    
    while (token && argc < max_args - 1) {
        argv[argc++] = token;
        token = strtok(0, " ");
    }
    argv[argc] = 0;
    return argc;
}

// Command: help
static int cmd_help(int argc, char* argv[]) {
    vga_print("Available commands:\n");
    vga_print("  help     - Show this help message\n");
    vga_print("  clear    - Clear the screen\n");
    vga_print("  echo     - Print arguments\n");
    vga_print("  ls       - List directory contents\n");
    vga_print("  cat      - Display file contents\n");
    vga_print("  mkdir    - Create directory\n");
    vga_print("  rmdir    - Remove directory\n");
    vga_print("  ps       - List processes\n");
    vga_print("  exit     - Exit shell\n");
    return 0;
}

// Command: clear
static int cmd_clear(int argc, char* argv[]) {
    vga_clear();
    return 0;
}

// Command: echo
static int cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        vga_print(argv[i]);
        if (i < argc - 1) {
            vga_print(" ");
        }
    }
    vga_print("\n");
    return 0;
}

// Command: ls
static int cmd_ls(int argc, char* argv[]) {
    const char* path = (argc > 1) ? argv[1] : "/";
    
    file_descriptor_t fd = vfs_open(path, O_RDONLY);
    if (fd < 0) {
        vga_print("Error: Cannot open directory\n");
        return -1;
    }
    
    int index = 0;
    vfs_node_t* node;
    while ((node = vfs_readdir(fd, index++)) != 0) {
        vga_print(node->name);
        if (node->type == FS_TYPE_DIR) {
            vga_print("/");
        }
        vga_print("  ");
    }
    vga_print("\n");
    
    vfs_close(fd);
    return 0;
}

// Command: cat
static int cmd_cat(int argc, char* argv[]) {
    if (argc < 2) {
        vga_print("Usage: cat <file>\n");
        return -1;
    }
    
    file_descriptor_t fd = vfs_open(argv[1], O_RDONLY);
    if (fd < 0) {
        vga_print("Error: Cannot open file\n");
        return -1;
    }
    
    char buffer[512];
    int bytes_read;
    while ((bytes_read = vfs_read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        vga_print(buffer);
    }
    vga_print("\n");
    
    vfs_close(fd);
    return 0;
}

// Command: mkdir
static int cmd_mkdir(int argc, char* argv[]) {
    if (argc < 2) {
        vga_print("Usage: mkdir <directory>\n");
        return -1;
    }
    
    if (vfs_mkdir(argv[1]) < 0) {
        vga_print("Error: Cannot create directory\n");
        return -1;
    }
    
    return 0;
}

// Command: rmdir
static int cmd_rmdir(int argc, char* argv[]) {
    if (argc < 2) {
        vga_print("Usage: rmdir <directory>\n");
        return -1;
    }
    
    if (vfs_rmdir(argv[1]) < 0) {
        vga_print("Error: Cannot remove directory\n");
        return -1;
    }
    
    return 0;
}

// Command: ps
static int cmd_ps(int argc, char* argv[]) {
    vga_print("PID  Name\n");
    vga_print("---  ----\n");
    
    // Iterate through process list
    extern process_t* g_process_list;
    process_t* proc = g_process_list;
    while (proc) {
        // Print PID (simple conversion)
        char pid_str[16];
        unsigned int pid = proc->pid;
        int i = 0;
        if (pid == 0) {
            pid_str[i++] = '0';
        } else {
            char temp[16];
            int j = 0;
            while (pid > 0) {
                temp[j++] = '0' + (pid % 10);
                pid /= 10;
            }
            for (int k = j - 1; k >= 0; k--) {
                pid_str[i++] = temp[k];
            }
        }
        pid_str[i] = '\0';
        vga_print(pid_str);
        vga_print("  ");
        vga_print(proc->name);
        vga_print("\n");
        
        proc = proc->next;
    }
    
    return 0;
}

// Command: exit
static int cmd_exit(int argc, char* argv[]) {
    return 1; // Signal to exit shell
}

// Execute a command
int shell_execute_command(const char* command) {
    if (!command || !*command) {
        return 0;
    }
    
    // Copy command to buffer for parsing
    char cmd_buf[SHELL_MAX_LINE];
    strcpy(cmd_buf, command);
    
    // Parse command
    char* argv[SHELL_MAX_ARGS];
    int argc = parse_command(cmd_buf, argv, SHELL_MAX_ARGS);
    
    if (argc == 0) {
        return 0;
    }
    
    // Execute command
    if (strcmp(argv[0], "help") == 0) {
        return cmd_help(argc, argv);
    } else if (strcmp(argv[0], "clear") == 0) {
        return cmd_clear(argc, argv);
    } else if (strcmp(argv[0], "echo") == 0) {
        return cmd_echo(argc, argv);
    } else if (strcmp(argv[0], "ls") == 0) {
        return cmd_ls(argc, argv);
    } else if (strcmp(argv[0], "cat") == 0) {
        return cmd_cat(argc, argv);
    } else if (strcmp(argv[0], "mkdir") == 0) {
        return cmd_mkdir(argc, argv);
    } else if (strcmp(argv[0], "rmdir") == 0) {
        return cmd_rmdir(argc, argv);
    } else if (strcmp(argv[0], "ps") == 0) {
        return cmd_ps(argc, argv);
    } else if (strcmp(argv[0], "exit") == 0) {
        return cmd_exit(argc, argv);
    } else {
        vga_print("Unknown command: ");
        vga_print(argv[0]);
        vga_print("\nType 'help' for available commands\n");
        return -1;
    }
}

// Initialize shell
void shell_init(void) {
    g_command_pos = 0;
    g_command_buffer[0] = '\0';
}

// Run shell (main loop)
void shell_run(void) {
    vga_print("\nZenith OS Shell\n");
    vga_print("Type 'help' for available commands\n");
    
    while (1) {
        vga_print(g_shell_prompt);
        
        // Read command line
        g_command_pos = 0;
        g_command_buffer[0] = '\0';
        
        while (1) {
            // Wait for keyboard input
            while (!keyboard_has_data()) {
                asm volatile ("hlt"); // Wait for interrupt
            }
            
            unsigned char c = keyboard_getchar();
            
            if (c == '\n' || c == '\r') {
                // Execute command
                vga_print("\n");
                if (g_command_pos > 0) {
                    g_command_buffer[g_command_pos] = '\0';
                    int result = shell_execute_command(g_command_buffer);
                    if (result == 1) {
                        // Exit command
                        return;
                    }
                }
                break;
            } else if (c == '\b' || c == 127) {
                // Backspace
                if (g_command_pos > 0) {
                    g_command_pos--;
                    g_command_buffer[g_command_pos] = '\0';
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
            } else if (c >= 32 && c < 127) {
                // Printable character
                if (g_command_pos < SHELL_MAX_LINE - 1) {
                    g_command_buffer[g_command_pos++] = c;
                    g_command_buffer[g_command_pos] = '\0';
                    vga_putchar(c);
                }
            }
        }
    }
}

