# Zenith OS

A modern, multi-tasking operating system for x86-32 architecture.

## 📖 Documentation

### Platform-Specific Setup Guides

- **Windows**: [`README_WINDOWS.md`](README_WINDOWS.md) - Complete Windows setup guide
- **Debian/Ubuntu**: [`README_DEBIAN.md`](README_DEBIAN.md) - Debian-based Linux setup
- **Manjaro/Arch**: [`README_MANJARO.md`](README_MANJARO.md) - Arch-based Linux setup
- **General Linux**: [`README_LINUX.md`](README_LINUX.md) - General Linux guide

### Quick References

- **Quick Start**: [`QUICK_START.md`](QUICK_START.md) - Get started in 5 minutes
- **Feature Goals**: [`FEATURE_GOALS.md`](FEATURE_GOALS.md) - Development roadmap
- **Roadmap**: [`ROADMAP.md`](ROADMAP.md) - Complete development roadmap

## 🚀 Quick Start

### Windows

1. Run `setup_windows.bat` as Administrator
2. Install cross-compiler (WSL recommended)
3. Build: `build.bat`
4. Run: `run.bat`

See [`README_WINDOWS.md`](README_WINDOWS.md) for details.

### Linux (Debian/Ubuntu)

```bash
sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
make -f Makefile.mk
make -f Makefile.mk run
```

See [`README_DEBIAN.md`](README_DEBIAN.md) for details.

### Linux (Manjaro/Arch)

```bash
sudo pacman -S base-devel nasm qemu gcc-elf binutils-elf
make -f Makefile.mk
make -f Makefile.mk run
```

See [`README_MANJARO.md`](README_MANJARO.md) for details.

## ✨ Features

### Core Systems
- ✅ Two-stage bootloader
- ✅ Multi-tasking with round-robin scheduler
- ✅ Virtual memory (paging)
- ✅ Process management (fork, exec, wait)
- ✅ System calls (28 implemented)
- ✅ File system (VFS with simple FS)
- ✅ Device drivers (VGA, keyboard, serial, ATA)

### User Space
- ✅ Standard C Library (libc)
- ✅ User program support
- ✅ ELF loader with argc/argv
- ✅ 7 user programs (calc, cal, etc.)

## 📁 Project Structure

```
zenith-os/
├── boot/              # Bootloader (stage 1 & 2)
├── kernel/            # Kernel source code
│   └── src/          # Kernel source files
├── user/             # User space
│   ├── libc/         # Standard C library
│   └── programs/     # User programs
├── Makefile.mk       # Kernel build system
└── Documentation/    # Setup guides and docs
```

## 🛠️ Building

### Build OS

```bash
make -f Makefile.mk
```

### Build User Programs

```bash
cd user
make
```

### Clean

```bash
make -f Makefile.mk clean
cd user && make clean
```

## 🎮 Running

```bash
qemu-system-x86_64 -fda boot.img -serial stdio
```

Or use the provided scripts:
- Windows: `run.bat`
- Linux: `make -f Makefile.mk run`

## 📚 User Programs

Available programs:
- `hello` - Hello World
- `calc` - Calculator
- `cal` - Calendar
- `sysinfo` - System information
- `cp` - Copy files
- `rm` - Remove files
- `echo` - Echo text

See [`user/README.md`](user/README.md) for details.

## 🎯 Current Status

- **Core OS**: ✅ Complete
- **Multi-tasking**: ✅ Complete
- **File System**: ✅ Complete
- **User Programs**: ✅ 7 programs implemented
- **Standard C Library**: ✅ Complete

See [`FEATURE_GOALS.md`](FEATURE_GOALS.md) for future plans.

## 📝 Requirements

- Cross-compiler (x86_64-elf-gcc)
- NASM assembler
- QEMU emulator
- Make

See platform-specific guides for installation instructions.

## 🆘 Getting Help

1. Check platform-specific README
2. Review `QUICK_START.md`
3. Check error messages
4. Verify all tools are installed

## 📄 License

[Add your license here]

## 🙏 Acknowledgments

- OSDev.org community
- QEMU project
- Various OS development resources

---

**Happy coding! 🚀**

