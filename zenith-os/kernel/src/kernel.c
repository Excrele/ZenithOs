// Freestanding kernel entry
void kernel_main(void) {
    const char* msg = "Zenith OS Kernel Loaded! (Bare Metal Victory)";
    char* video = (char*) 0xb8000;
    for (int i = 0; msg[i] != '\0'; ++i) {
        video[i * 2] = msg[i];      // Char
        video[i * 2 + 1] = 0x0f;    // Attr: White on black
    }
    while (1) {}  // Halt
}