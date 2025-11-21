#ifndef SHELL_H
#define SHELL_H

// Initialize shell
void shell_init(void);

// Run shell (main loop)
void shell_run(void);

// Execute a command
int shell_execute_command(const char* command);

#endif // SHELL_H

