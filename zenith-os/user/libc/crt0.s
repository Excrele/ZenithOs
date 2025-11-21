# C runtime startup code for user programs
# Sets up stack and calls main()
# Kernel sets up stack with: [argc] [argv[0]] [argv[1]] ... [argv[n]] [NULL] [strings...]

.global _start
_start:
    # Stack is already set up by kernel
    # Kernel layout: argc at bottom, then argv array, then strings
    
    # Clear frame pointer
    xor %ebp, %ebp
    
    # Get argc from stack (kernel puts it at the bottom)
    # We need to find it - it's at a fixed offset from current stack
    # For now, assume kernel sets up stack properly and argc is accessible
    # We'll use a simple approach: argc is first, argv follows
    
    # Calculate argc location (kernel puts it at stack bottom)
    # For simplicity, we'll pass argc=0, argv=NULL if not set up
    # The kernel's process_exec should have set this up properly
    
    # Kernel sets up stack: [argc] [argv array] [strings]
    # argc is at the bottom of our stack area
    # We need to find it - kernel puts it just before argv array
    
    # For now, use a simple approach:
    # Kernel should have set up: argc at (esp), argv array follows
    # We'll read argc from a known location
    # Actually, kernel sets stack_ptr to point to argc location
    
    # Read argc from stack (kernel puts it at stack bottom)
    mov (%esp), %eax  # argc (if kernel set it up)
    
    # Calculate argv pointer (4 bytes after argc)
    lea 4(%esp), %ebx  # argv
    
    # Push arguments for main (C calling convention: argc, argv)
    push %ebx  # argv
    push %eax  # argc
    
    # Call main
    call main
    
    # Clean up stack
    add $8, %esp
    
    # Exit with return value
    mov %eax, %ebx  # exit code
    mov $1, %eax    # SYS_EXIT
    int $0x80
    # Should never return
    hlt

