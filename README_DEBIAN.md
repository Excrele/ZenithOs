# Zenith OS - Debian/Ubuntu Setup Guide

This guide is for Debian-based Linux distributions (Ubuntu, Debian, Linux Mint, etc.).

## 🚀 Quick Start

### Step 1: Install Required Tools

```bash
sudo apt update
sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
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
| **build-essential** | GCC, Make, and other build tools | `sudo apt install build-essential` |
| **nasm** | Netwide Assembler | `sudo apt install nasm` |
| **qemu-system-x86** | QEMU emulator | `sudo apt install qemu-system-x86` |
| **gcc-x86-64-elf** | Cross-compiler (GCC) | `sudo apt install gcc-x86-64-elf` |
| **binutils-x86-64-elf** | Cross-compiler (binutils) | `sudo apt install binutils-x86-64-elf` |

### Complete Installation Command

```bash
sudo apt update && sudo apt install -y \
    build-essential \
    nasm \
    qemu-system-x86 \
    gcc-x86-64-elf \
    binutils-x86-64-elf
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

**Solution**: Update package list:
```bash
sudo apt update
```

### "Permission denied" errors

**Solution**: Use `sudo` for installation:
```bash
sudo apt install <package>
```

### Cross-compiler not found

**Solution**: Install cross-compiler packages:
```bash
sudo apt install gcc-x86-64-elf binutils-x86-64-elf
```

### Build fails with "command not found"

**Solution**: Install build-essential:
```bash
sudo apt install build-essential
```

### QEMU won't start

**Solution**: 
- Check QEMU is installed: `qemu-system-x86_64 --version`
- Verify `boot.img` exists
- Check file permissions

### "No space left on device"

**Solution**: Clean up build artifacts:
```bash
make -f Makefile.mk clean
cd user && make clean
```

## 📦 Alternative: Install from Source

If packages aren't available in your repository:

### NASM from Source

```bash
wget https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/nasm-2.16.01.tar.gz
tar -xzf nasm-2.16.01.tar.gz
cd nasm-2.16.01
./configure
make
sudo make install
```

### QEMU from Source

```bash
sudo apt install -y git build-essential python3 ninja-build
git clone https://gitlab.com/qemu-project/qemu.git
cd qemu
./configure
make
sudo make install
```

### Cross-Compiler from Source

See: https://wiki.osdev.org/GCC_Cross-Compiler

## 🔄 Updating Packages

```bash
# Update package list
sudo apt update

# Upgrade installed packages
sudo apt upgrade

# Upgrade specific packages
sudo apt install --upgrade nasm qemu-system-x86
```

## 📚 Additional Tools (Optional)

### Development Tools

```bash
sudo apt install -y \
    gdb \
    hexedit \
    bochs \
    git
```

### Documentation Tools

```bash
sudo apt install -y \
    man-db \
    manpages-dev
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
- [ ] `x86_64-elf-gcc --version` works
- [ ] `make -f Makefile.mk` builds successfully
- [ ] `boot.img` is created
- [ ] QEMU starts and shows shell prompt

## 🆘 Getting Help

- Check error messages carefully
- Verify all packages are installed
- Try cleaning and rebuilding
- Check file permissions
- Review `QUICK_START.md` for general guidance

## 📝 Notes

- **Ubuntu 20.04+** and **Debian 11+** are recommended
- Older versions may need additional repositories
- Some packages may have different names in older distributions
- Use `apt-cache search <package>` to find packages

## 🎉 You're Ready!

Once you see the `zenith>` prompt, you're ready to start developing!

---

**Happy coding! 🚀**

