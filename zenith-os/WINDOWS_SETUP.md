# Windows Setup Guide for Zenith OS

This guide will help you set up a development environment for Zenith OS on Windows.

## Prerequisites

You'll need the following tools:

1. **Cross-compiler toolchain** (x86_64-elf-gcc, ld, objcopy)
2. **NASM** - Netwide Assembler
3. **QEMU** - For testing/emulation
4. **Make** - Build automation (or use our batch scripts)

## Setup Options

### Option 1: WSL (Windows Subsystem for Linux) - **RECOMMENDED**

WSL is the easiest way to get a Linux-like environment on Windows.

#### Installation Steps:

1. **Install WSL** (if not already installed):
   ```powershell
   wsl --install
   ```
   Or download from Microsoft Store: "Ubuntu" or "Debian"

2. **Install build tools in WSL**:
   ```bash
   sudo apt update
   sudo apt install -y build-essential nasm qemu-system-x86
   ```

3. **Install cross-compiler**:
   ```bash
   sudo apt install -y gcc-x86-64-elf binutils-x86-64-elf
   ```

4. **Test installation**:
   ```bash
   x86_64-elf-gcc --version
   nasm --version
   qemu-system-x86_64 --version
   ```

5. **Build the OS**:
   ```bash
   cd /mnt/f/Development/Zenith\ Os/ZenithOs/zenith-os
   make -f Makefile.mk
   ```

6. **Run in QEMU**:
   ```bash
   make -f Makefile.mk run
   ```

### Option 2: MSYS2 (Native Windows)

MSYS2 provides a Unix-like environment on Windows.

#### Installation Steps:

1. **Download MSYS2**: https://www.msys2.org/
2. **Install MSYS2** and open MSYS2 terminal
3. **Update packages**:
   ```bash
   pacman -Syu
   ```

4. **Install build tools**:
   ```bash
   pacman -S base-devel nasm mingw-w64-x86_64-gcc
   ```

5. **Install QEMU**:
   ```bash
   pacman -S qemu
   ```

6. **Note**: You may need to adjust the Makefile to use `gcc` instead of `x86_64-elf-gcc` or install a cross-compiler.

### Option 3: Chocolatey Package Manager

Use Chocolatey to install tools directly on Windows.

#### Installation Steps:

1. **Install Chocolatey** (as Administrator):
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force
   [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
   iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
   ```

2. **Install tools**:
   ```powershell
   choco install nasm qemu make -y
   ```

3. **For cross-compiler**, you'll need to download manually or use WSL.

## Quick Setup Script

We've created `setup_windows.bat` that will help automate the setup process.

Run it as Administrator:
```cmd
setup_windows.bat
```

## Manual Setup

### 1. Install NASM

- Download from: https://www.nasm.us/pub/nasm/releasebuilds/
- Extract to `C:\nasm`
- Add `C:\nasm` to PATH

### 2. Install QEMU

- Download from: https://www.qemu.org/download/#windows
- Install to default location
- QEMU should add itself to PATH

### 3. Install Cross-Compiler

**Option A: Pre-built binaries**
- Download from: https://github.com/lordmilko/i686-elf-tools (for 32-bit)
- Or use WSL (recommended)

**Option B: Build from source** (advanced)
- Follow: https://wiki.osdev.org/GCC_Cross-Compiler

### 4. Install Make

- Download from: http://gnuwin32.sourceforge.net/packages/make.htm
- Or use Chocolatey: `choco install make`
- Or use WSL

## Testing the OS

### Build the OS

```cmd
cd zenith-os
make -f Makefile.mk
```

Or use the batch script:
```cmd
build.bat
```

### Run in QEMU

```cmd
make -f Makefile.mk run
```

Or use the batch script:
```cmd
run.bat
```

### Expected Output

You should see:
1. Bootloader messages
2. Kernel initialization
3. Shell prompt: `zenith>`

### Testing User Programs

1. **Build user programs** (in WSL or MSYS2):
   ```bash
   cd user
   make
   ```

2. **Copy binaries to file system** (once file system is mounted)

3. **Run from shell**:
   ```
   zenith> exec /bin/hello
   ```

## Troubleshooting

### "make: command not found"
- Install Make or use WSL
- Or use the provided batch scripts

### "x86_64-elf-gcc: command not found"
- Install cross-compiler toolchain
- Or use WSL (easiest option)

### "nasm: command not found"
- Install NASM and add to PATH
- Or use WSL

### "qemu-system-x86_64: command not found"
- Install QEMU and add to PATH
- Or use WSL

### Build Errors
- Make sure you're using the correct toolchain
- Check that all paths are correct
- Try cleaning and rebuilding: `make -f Makefile.mk clean`

## Recommended Setup

**For best experience, use WSL2:**
- Native Linux toolchain
- Easy package management
- Better compatibility with OS development tools
- Can access Windows files via `/mnt/`

## Next Steps

1. Run `setup_windows.bat` (as Administrator)
2. Follow the prompts
3. Build the OS: `build.bat`
4. Run in QEMU: `run.bat`
5. Start developing!

## Additional Resources

- OSDev Wiki: https://wiki.osdev.org/
- QEMU Documentation: https://www.qemu.org/documentation/
- NASM Documentation: https://www.nasm.us/docs.php

