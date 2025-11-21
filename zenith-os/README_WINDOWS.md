# Zenith OS - Windows Development Guide

## 🚀 Quick Start

1. **Run setup script** (as Administrator):
   ```cmd
   setup_windows.bat
   ```

2. **Install cross-compiler** (choose one):
   - **WSL** (recommended): `wsl --install` then `sudo apt install gcc-x86-64-elf binutils-x86-64-elf`
   - **Pre-built**: Download from https://github.com/lordmilko/i686-elf-tools

3. **Build the OS**:
   ```cmd
   build.bat
   ```
   Or: `make -f Makefile.mk` (if Make is installed)

4. **Run in QEMU**:
   ```cmd
   run.bat
   ```

## 📋 Required Tools

| Tool | Purpose | How to Install |
|------|---------|----------------|
| **NASM** | Assembler | `setup_windows.bat` or `choco install nasm` |
| **QEMU** | Emulator | `setup_windows.bat` or `choco install qemu` |
| **Make** | Build tool | `setup_windows.bat` or `choco install make` |
| **Cross-compiler** | Compiler | WSL or manual install (see below) |

## 🔧 Installation Methods

### Method 1: Automated Setup (Easiest)

Run as Administrator:
```cmd
setup_windows.bat
```

This installs:
- ✅ NASM
- ✅ QEMU  
- ✅ Make
- ⚠️ Cross-compiler (manual step required)

### Method 2: WSL (Recommended)

WSL provides the best compatibility:

```powershell
# Install WSL
wsl --install

# In WSL, install tools:
sudo apt update
sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
```

Then build in WSL:
```bash
cd /mnt/f/Development/Zenith\ Os/ZenithOs/zenith-os
make -f Makefile.mk
```

### Method 3: Chocolatey (Native Windows)

```powershell
# Install Chocolatey (as Admin)
Set-ExecutionPolicy Bypass -Scope Process -Force
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install tools
choco install nasm qemu make -y
```

## 🏗️ Building

### Build OS Kernel

**Option A: Using batch script**
```cmd
build.bat
```

**Option B: Using Make**
```cmd
make -f Makefile.mk
```

**Option C: Using WSL**
```bash
wsl make -f Makefile.mk
```

### Build User Programs

```cmd
build_user.bat
```

Or in WSL:
```bash
cd user
make
```

## 🎮 Running & Testing

### Run in QEMU

```cmd
run.bat
```

Or directly:
```cmd
qemu-system-x86_64 -fda boot.img -serial stdio
```

### Expected Output

1. Bootloader: "Zenith OS Loading..."
2. Kernel initialization messages
3. Shell prompt: `zenith>`

### Test Commands

```
zenith> help          # Show available commands
zenith> ls            # List directory
zenith> ps             # Show processes
zenith> echo Hello     # Echo text
```

## 📁 Project Structure

```
zenith-os/
├── setup_windows.bat    # Setup script
├── build.bat            # Build OS
├── run.bat              # Run in QEMU
├── build_user.bat       # Build user programs
├── clean.bat            # Clean build files
├── WINDOWS_SETUP.md     # Detailed setup guide
├── QUICK_START.md       # Quick start guide
├── boot/                # Bootloader
├── kernel/              # Kernel source
└── user/                # User programs
```

## 🛠️ Troubleshooting

### "Command not found"

**Solution**: Run `setup_windows.bat` as Administrator, or install manually.

### "Cross-compiler not found"

**Solution**: 
- Install WSL and run: `sudo apt install gcc-x86-64-elf binutils-x86-64-elf`
- Or download pre-built binaries

### "Make: command not found"

**Solution**:
- Install Make: `choco install make`
- Or use WSL
- Or use `build.bat` (limited functionality)

### Build fails

**Solution**:
1. Clean: `clean.bat`
2. Check all tools are installed
3. Try WSL for better compatibility
4. See WINDOWS_SETUP.md for details

### QEMU won't start

**Solution**:
- Check `boot.img` exists
- Verify QEMU installation: `qemu-system-x86_64 --version`
- Try: `qemu-system-x86_64 -fda boot.img -serial stdio`

## 📚 Documentation

- **QUICK_START.md** - Get started in 5 minutes
- **WINDOWS_SETUP.md** - Detailed setup instructions
- **FEATURE_GOALS.md** - Feature roadmap
- **user/README.md** - User program guide

## ✅ Verification Checklist

After setup, verify:

- [ ] `nasm --version` works
- [ ] `qemu-system-x86_64 --version` works
- [ ] `x86_64-elf-gcc --version` works (or WSL equivalent)
- [ ] `make -f Makefile.mk` builds successfully
- [ ] `run.bat` starts QEMU
- [ ] Shell prompt appears

## 🎯 Next Steps

1. ✅ Run `setup_windows.bat`
2. ✅ Install cross-compiler
3. ✅ Build: `build.bat`
4. ✅ Run: `run.bat`
5. ✅ Build user programs: `build_user.bat`
6. ✅ Start developing!

## 💡 Tips

- **Use WSL** for best compatibility
- **Keep terminal open** after running setup scripts
- **Restart terminal** after installing tools (for PATH updates)
- **Run as Administrator** when installing tools
- **Check PATH** if tools aren't found after installation

## 🆘 Need Help?

- Check **WINDOWS_SETUP.md** for detailed instructions
- See **QUICK_START.md** for quick reference
- Review error messages carefully
- Try WSL if native Windows tools fail

---

**Happy coding! 🚀**

