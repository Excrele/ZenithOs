# Zenith OS - Manjaro/Arch Setup Guide

This guide is for Arch-based Linux distributions (Manjaro, Arch Linux, EndeavourOS, etc.).

## 🚀 Quick Start

### Step 1: Install Required Tools

```bash
sudo pacman -Syu
sudo pacman -S base-devel nasm qemu gcc-elf binutils-elf
```

### Step 2: Build the OS

```bash
cd zenith-os
make -f Makefile.mk
```

### Step 3: Run in QEMU

```bash
make -f Makefile.mk run
```

Or:
```bash
qemu-system-x86_64 -fda boot.img -serial stdio
```

## 📋 Detailed Installation

### Required Packages

| Package | Purpose | Install Command |
|---------|---------|----------------|
| **base-devel** | GCC, Make, and build tools | `sudo pacman -S base-devel` |
| **nasm** | Netwide Assembler | `sudo pacman -S nasm` |
| **qemu** | QEMU emulator | `sudo pacman -S qemu` |
| **gcc-elf** | Cross-compiler (GCC) | `sudo pacman -S gcc-elf` |
| **binutils-elf** | Cross-compiler (binutils) | `sudo pacman -S binutils-elf` |

### Complete Installation Command

```bash
sudo pacman -Syu && sudo pacman -S \
    base-devel \
    nasm \
    qemu \
    gcc-elf \
    binutils-elf
```

## 🔧 Verification

After installation, verify all tools are available:

```bash
# Check GCC
gcc --version

# Check NASM
nasm --version

# Check QEMU
qemu-system-x86_64 --version

# Check cross-compiler
x86_64-elf-gcc --version
x86_64-elf-ld --version
```

**Note**: On Arch-based systems, the cross-compiler may be named differently. Check with:
```bash
pacman -Ql gcc-elf | grep bin
```

If the cross-compiler isn't available, you may need to use the AUR or build from source.

## 🏗️ Building

### Build OS Kernel

```bash
cd zenith-os
make -f Makefile.mk
```

This will create:
- `stage1.bin` - Bootloader stage 1
- `stage2.bin` - Bootloader stage 2
- `kernel.bin` - Kernel binary
- `boot.img` - Complete bootable image

### Build User Programs

```bash
cd user
make
```

This will create binaries in `user/build/`:
- `hello.bin`
- `calc.bin`
- `cal.bin`
- `sysinfo.bin`
- `cp.bin`
- `rm.bin`
- `echo.bin`

### Clean Build Artifacts

```bash
# Clean kernel build
make -f Makefile.mk clean

# Clean user programs
cd user
make clean
```

## 🎮 Running & Testing

### Run in QEMU

```bash
make -f Makefile.mk run
```

Or directly:
```bash
qemu-system-x86_64 -fda boot.img -serial stdio
```

### QEMU Options

```bash
# Run with more memory
qemu-system-x86_64 -fda boot.img -m 512M -serial stdio

# Run with debug output
qemu-system-x86_64 -fda boot.img -serial stdio -d int

# Run with GDB support
qemu-system-x86_64 -fda boot.img -serial stdio -s -S
```

Then in another terminal:
```bash
gdb
(gdb) target remote :1234
(gdb) symbol-file kernel.bin
```

### Expected Output

1. **Bootloader**: "Zenith OS Loading..."
2. **Kernel initialization**: Various system components
3. **Shell prompt**: `zenith>`

### Test Commands

```
zenith> help          # Show available commands
zenith> ls            # List directory
zenith> ps            # Show processes
zenith> echo Hello    # Echo text
zenith> clear         # Clear screen
```

## 🐛 Troubleshooting

### "Package not found" errors

**Solution**: Update package database:
```bash
sudo pacman -Sy
```

### Cross-compiler not available

**Solution**: The cross-compiler packages may not be in the main repositories. Options:

**Option 1: Use AUR (Arch User Repository)**

If you have `yay` or `paru`:
```bash
# Using yay
yay -S gcc-x86-64-elf-git binutils-x86-64-elf-git

# Using paru
paru -S gcc-x86-64-elf-git binutils-x86-64-elf-git
```

**Option 2: Build from source**

See: https://wiki.osdev.org/GCC_Cross-Compiler

**Option 3: Use regular GCC (not recommended)**

You can modify the Makefile to use regular `gcc`, but this may cause issues:
```makefile
CROSS_COMPILE = 
CC = gcc
LD = ld
```

### "Permission denied" errors

**Solution**: Use `sudo` for installation:
```bash
sudo pacman -S <package>
```

### Build fails with "command not found"

**Solution**: Install base-devel:
```bash
sudo pacman -S base-devel
```

### QEMU won't start

**Solution**: 
- Check QEMU is installed: `qemu-system-x86_64 --version`
- Verify `boot.img` exists
- Check file permissions
- Try: `qemu-system-x86_64 -fda boot.img -serial stdio`

### "No space left on device"

**Solution**: Clean up build artifacts:
```bash
make -f Makefile.mk clean
cd user && make clean
```

## 📦 AUR Packages (Optional)

If you use an AUR helper like `yay` or `paru`:

### Install AUR Helper (if needed)

**yay**:
```bash
cd /tmp
git clone https://aur.archlinux.org/yay.git
cd yay
makepkg -si
```

**paru**:
```bash
cd /tmp
git clone https://aur.archlinux.org/paru.git
cd paru
makepkg -si
```

### Install Cross-Compiler from AUR

```bash
# Using yay
yay -S gcc-x86-64-elf-git binutils-x86-64-elf-git

# Using paru
paru -S gcc-x86-64-elf-git binutils-x86-64-elf-git
```

## 🔄 Updating Packages

```bash
# Update package database and upgrade all packages
sudo pacman -Syu

# Update specific packages
sudo pacman -S <package>
```

## 📚 Additional Tools (Optional)

### Development Tools

```bash
sudo pacman -S \
    gdb \
    hexedit \
    bochs \
    git \
    vim \
    emacs
```

### Documentation Tools

```bash
sudo pacman -S \
    man-db \
    man-pages
```

## 🎯 Quick Reference

### Common Commands

```bash
# Build everything
make -f Makefile.mk

# Build and run
make -f Makefile.mk && make -f Makefile.mk run

# Clean and rebuild
make -f Makefile.mk clean && make -f Makefile.mk

# Build user programs
cd user && make

# Run QEMU
qemu-system-x86_64 -fda boot.img -serial stdio
```

### File Locations

- **Bootloader**: `boot/stage1/`, `boot/stage2/`
- **Kernel**: `kernel/src/`
- **User programs**: `user/programs/`
- **Build output**: `boot.img`, `user/build/*.bin`

## ✅ Verification Checklist

After setup, verify:

- [ ] `gcc --version` works
- [ ] `nasm --version` works
- [ ] `qemu-system-x86_64 --version` works
- [ ] `x86_64-elf-gcc --version` works (or alternative)
- [ ] `make -f Makefile.mk` builds successfully
- [ ] `boot.img` is created
- [ ] QEMU starts and shows shell prompt

## 🔍 Finding Packages

### Search for packages

```bash
# Search in official repositories
pacman -Ss <package-name>

# Search in AUR
yay -Ss <package-name>  # if using yay
paru -Ss <package-name>  # if using paru
```

### Check if package is installed

```bash
pacman -Q <package-name>
```

### List installed packages

```bash
pacman -Q | grep <keyword>
```

## 🆘 Getting Help

- Check error messages carefully
- Verify all packages are installed
- Try cleaning and rebuilding
- Check file permissions
- Review `QUICK_START.md` for general guidance
- Check Arch Wiki: https://wiki.archlinux.org/

## 📝 Notes

- **Manjaro** and **Arch Linux** are fully supported
- Cross-compiler packages may be in AUR, not main repos
- Use `yay` or `paru` for easier AUR package management
- Some packages may need to be built from source
- Always run `sudo pacman -Syu` before installing new packages

## 🎉 You're Ready!

Once you see the `zenith>` prompt, you're ready to start developing!

---

**Happy coding! 🚀**

