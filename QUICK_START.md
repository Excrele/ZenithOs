# Quick Start Guide

## 🚀 Fastest Way to Get Started

**Choose your platform:**
- **Windows**: See `README_WINDOWS.md`
- **Debian/Ubuntu**: See `README_DEBIAN.md`
- **Manjaro/Arch**: See `README_MANJARO.md`
- **Other Linux**: See `README_LINUX.md`

---

## Windows Quick Start

### Step 1: Run Setup Script

Open PowerShell or Command Prompt **as Administrator** and run:

```cmd
cd "F:\Development\Zenith Os\ZenithOs\zenith-os"
setup_windows.bat
```

This will install:
- NASM (assembler)
- QEMU (emulator)
- Make (build tool)
- Check for cross-compiler

### Step 2: Install Cross-Compiler

The cross-compiler is required but not automatically installed. Choose one:

#### Option A: WSL (Recommended - Easiest)

```powershell
wsl --install
```

Then in WSL:
```bash
sudo apt update
sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
```

#### Option B: Pre-built Binaries

Download from: https://github.com/lordmilko/i686-elf-tools

Extract and add to PATH.

### Step 3: Build the OS

#### If using WSL:
```bash
cd /mnt/f/Development/Zenith\ Os/ZenithOs/zenith-os
make -f Makefile.mk
```

#### If using native Windows:
```cmd
build.bat
```

Or if Make is installed:
```cmd
make -f Makefile.mk
```

### Step 4: Run in QEMU

```cmd
run.bat
```

Or:
```cmd
qemu-system-x86_64 -fda boot.img -serial stdio
```

### Step 5: Build User Programs (Optional)

```cmd
build_user.bat
```

Or in WSL:
```bash
cd user
make
```

## 📋 What You'll See

1. **Bootloader messages**: "Zenith OS Loading..."
2. **Kernel initialization**: Various system components initializing
3. **Shell prompt**: `zenith>`

## 🎮 Testing

### In the Shell:

```
zenith> help
zenith> ls
zenith> ps
zenith> echo Hello World
```

### Running User Programs:

Once user programs are built and copied to the file system:
```
zenith> exec /bin/hello
zenith> exec /bin/calc "2 + 3 * 4"
zenith> exec /bin/cal 12 2024
```

## 🛠️ Troubleshooting

### "Command not found" errors
- Run `setup_windows.bat` as Administrator
- Make sure tools are in PATH
- Restart terminal after installation

### Build fails
- Check that cross-compiler is installed
- Try cleaning: `make -f Makefile.mk clean`
- Check WINDOWS_SETUP.md for detailed help

### QEMU won't start
- Make sure `boot.img` exists
- Check QEMU installation
- Try: `qemu-system-x86_64 -fda boot.img -serial stdio`

## 📚 More Information

- **WINDOWS_SETUP.md** - Detailed setup instructions
- **FEATURE_GOALS.md** - Feature roadmap
- **user/README.md** - User program documentation

## ✅ Checklist

- [ ] Ran `setup_windows.bat` as Administrator
- [ ] Installed cross-compiler (WSL or manual)
- [ ] Built the OS (`build.bat` or `make`)
- [ ] Ran in QEMU (`run.bat`)
- [ ] Saw the shell prompt
- [ ] (Optional) Built user programs

## 🎉 You're Ready!

Once you see the `zenith>` prompt, you're ready to start developing!

