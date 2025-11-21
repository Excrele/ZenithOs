# Zenith OS - Linux Setup Guide (General)

This is a general guide for Linux users. For distribution-specific instructions, see:
- **Debian/Ubuntu**: `README_DEBIAN.md`
- **Manjaro/Arch**: `README_MANJARO.md`

## 🚀 Quick Start

### Step 1: Install Required Tools

The exact command depends on your distribution:

**Debian/Ubuntu**:
```bash
sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
```

**Manjaro/Arch**:
```bash
sudo pacman -S base-devel nasm qemu gcc-elf binutils-elf
```

**Fedora/RHEL**:
```bash
sudo dnf install gcc make nasm qemu-system-x86
# Cross-compiler may need to be built from source
```

**openSUSE**:
```bash
sudo zypper install gcc make nasm qemu
# Cross-compiler may need to be built from source
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

## 📋 Required Tools

| Tool | Purpose | Package Name (varies by distro) |
|------|---------|----------------------------------|
| **GCC** | C compiler | `gcc`, `build-essential` (Debian), `base-devel` (Arch) |
| **Make** | Build automation | `make`, included in build-essential/base-devel |
| **NASM** | Assembler | `nasm` |
| **QEMU** | Emulator | `qemu-system-x86`, `qemu` |
| **Cross-compiler** | x86_64-elf-gcc | `gcc-x86-64-elf` (Debian), AUR (Arch), or build from source |

## 🔧 Verification

After installation, verify tools:

```bash
gcc --version
nasm --version
qemu-system-x86_64 --version
x86_64-elf-gcc --version  # May not be available on all distros
```

## 🏗️ Building

### Build OS Kernel

```bash
cd zenith-os
make -f Makefile.mk
```

### Build User Programs

```bash
cd user
make
```

### Clean Build

```bash
make -f Makefile.mk clean
cd user && make clean
```

## 🎮 Running

```bash
qemu-system-x86_64 -fda boot.img -serial stdio
```

## 🐛 Troubleshooting

### Cross-Compiler Not Available

If `x86_64-elf-gcc` isn't available in your distribution's repositories:

1. **Check AUR** (Arch-based): Use `yay` or `paru`
2. **Build from source**: See https://wiki.osdev.org/GCC_Cross-Compiler
3. **Use pre-built binaries**: Download from OSDev community

### Package Manager Commands

| Distribution | Update | Install | Search |
|--------------|--------|---------|--------|
| Debian/Ubuntu | `apt update` | `apt install` | `apt search` |
| Arch/Manjaro | `pacman -Sy` | `pacman -S` | `pacman -Ss` |
| Fedora | `dnf check-update` | `dnf install` | `dnf search` |
| openSUSE | `zypper refresh` | `zypper install` | `zypper search` |

## 📚 Distribution-Specific Guides

- **Debian/Ubuntu**: See `README_DEBIAN.md`
- **Manjaro/Arch**: See `README_MANJARO.md`
- **Windows**: See `README_WINDOWS.md`

## 🎯 Quick Reference

```bash
# Build
make -f Makefile.mk

# Run
qemu-system-x86_64 -fda boot.img -serial stdio

# Clean
make -f Makefile.mk clean
```

## 🆘 Need Help?

- Check distribution-specific README
- Review error messages
- Verify all tools are installed
- Try cleaning and rebuilding

---

**Happy coding! 🚀**

