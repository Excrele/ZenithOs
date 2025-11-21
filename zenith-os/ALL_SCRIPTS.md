# All Windows Scripts Reference

## Setup & Installation

### `setup_windows.bat`
**Purpose**: Automated setup of development tools  
**Run as**: Administrator  
**Installs**:
- NASM (assembler)
- QEMU (emulator)
- Make (build tool)
- Checks for cross-compiler

**Usage**:
```cmd
setup_windows.bat
```

## Building

### `build.bat`
**Purpose**: Build the OS kernel and bootloader  
**Requirements**: NASM, cross-compiler, Make (or WSL)

**Usage**:
```cmd
build.bat
```

**What it does**:
1. Builds bootloader stage 1
2. Builds bootloader stage 2
3. Attempts to build kernel (requires Make or WSL)

### `build_user.bat`
**Purpose**: Build user programs  
**Requirements**: WSL (recommended) or Linux-like environment

**Usage**:
```cmd
build_user.bat
```

**What it does**:
- Builds all user programs in `user/programs/`
- Creates `user/build/*.bin` files

## Running

### `run.bat`
**Purpose**: Launch OS in QEMU emulator  
**Requirements**: QEMU, `boot.img` file

**Usage**:
```cmd
run.bat
```

**Controls**:
- `Ctrl+Alt+G`: Release mouse/keyboard
- `Ctrl+Alt+F`: Toggle fullscreen
- `Ctrl+C`: Stop QEMU

## Maintenance

### `clean.bat`
**Purpose**: Remove all build artifacts  
**Usage**:
```cmd
clean.bat
```

**Removes**:
- `*.bin` files
- `*.img` files
- `kernel/src/*.o` files
- `user/build/` directory

## Helper Scripts

### `build_helper.bat`
**Purpose**: Combine binary files (Windows alternative to `cat`)  
**Usage**:
```cmd
build_helper.bat output.bin file1.bin file2.bin file3.bin
```

## Quick Reference

| Script | Purpose | Run As |
|--------|---------|--------|
| `setup_windows.bat` | Install tools | Admin |
| `build.bat` | Build OS | User |
| `build_user.bat` | Build programs | User |
| `run.bat` | Run in QEMU | User |
| `clean.bat` | Clean build | User |

## Typical Workflow

1. **First time setup**:
   ```cmd
   setup_windows.bat
   ```

2. **Build OS**:
   ```cmd
   build.bat
   ```

3. **Run OS**:
   ```cmd
   run.bat
   ```

4. **Build user programs** (optional):
   ```cmd
   build_user.bat
   ```

5. **Clean** (when needed):
   ```cmd
   clean.bat
   ```

## Notes

- Most scripts check for required tools before running
- Scripts provide helpful error messages if tools are missing
- WSL is recommended for best compatibility
- See individual script files for detailed comments

