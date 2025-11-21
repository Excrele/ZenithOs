@echo off
REM Zenith OS Windows Setup Script
REM This script helps set up the development environment on Windows

echo ========================================
echo Zenith OS - Windows Setup Script
echo ========================================
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: This script requires Administrator privileges!
    echo Please right-click and select "Run as Administrator"
    pause
    exit /b 1
)

echo Checking for required tools...
echo.

REM Check for Chocolatey
where choco >nul 2>&1
if %errorLevel% neq 0 (
    echo Chocolatey not found. Installing Chocolatey...
    echo.
    powershell -NoProfile -ExecutionPolicy Bypass -Command "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    if %errorLevel% neq 0 (
        echo Failed to install Chocolatey. Please install manually from https://chocolatey.org/
        pause
        exit /b 1
    )
    echo Chocolatey installed successfully!
    echo.
) else (
    echo Chocolatey is already installed.
    echo.
)

REM Refresh PATH
call refreshenv >nul 2>&1

REM Check and install NASM
where nasm >nul 2>&1
if %errorLevel% neq 0 (
    echo NASM not found. Installing NASM...
    choco install nasm -y
    if %errorLevel% neq 0 (
        echo Failed to install NASM. Please install manually from https://www.nasm.us/
        pause
        exit /b 1
    )
    echo NASM installed successfully!
    echo.
) else (
    echo NASM is already installed.
    nasm --version
    echo.
)

REM Check and install QEMU
where qemu-system-x86_64 >nul 2>&1
if %errorLevel% neq 0 (
    echo QEMU not found. Installing QEMU...
    choco install qemu -y
    if %errorLevel% neq 0 (
        echo Failed to install QEMU. Please install manually from https://www.qemu.org/
        pause
        exit /b 1
    )
    echo QEMU installed successfully!
    echo.
) else (
    echo QEMU is already installed.
    qemu-system-x86_64 --version
    echo.
)

REM Check for Make
where make >nul 2>&1
if %errorLevel% neq 0 (
    echo Make not found. Installing Make...
    choco install make -y
    if %errorLevel% neq 0 (
        echo Make installation failed. You can use WSL or MSYS2 instead.
        echo Or use the provided build.bat script.
        echo.
    ) else (
        echo Make installed successfully!
        echo.
    )
) else (
    echo Make is already installed.
    make --version
    echo.
)

REM Check for WSL
where wsl >nul 2>&1
if %errorLevel% equ 0 (
    echo WSL is available. This is the recommended option for cross-compiler.
    echo.
    echo To set up WSL with cross-compiler, run in WSL:
    echo   sudo apt update
    echo   sudo apt install -y build-essential nasm qemu-system-x86 gcc-x86-64-elf binutils-x86-64-elf
    echo.
) else (
    echo WSL is not installed. Consider installing WSL for easier cross-compiler setup.
    echo Install from Microsoft Store or run: wsl --install
    echo.
)

echo ========================================
echo Setup Summary
echo ========================================
echo.

echo Checking installed tools:
echo.

where nasm >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] NASM
    nasm --version | findstr /i "version"
) else (
    echo [MISSING] NASM
)

echo.

where qemu-system-x86_64 >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] QEMU
    qemu-system-x86_64 --version | findstr /i "version"
) else (
    echo [MISSING] QEMU
)

echo.

where make >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Make
    make --version | findstr /i "version"
) else (
    echo [MISSING] Make (optional - can use build.bat instead)
)

echo.

where x86_64-elf-gcc >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Cross-compiler (x86_64-elf-gcc)
    x86_64-elf-gcc --version | findstr /i "gcc"
) else (
    echo [MISSING] Cross-compiler
    echo.
    echo IMPORTANT: Cross-compiler is required to build the OS!
    echo.
    echo Options:
    echo   1. Use WSL (recommended):
    echo      wsl --install
    echo      Then in WSL: sudo apt install gcc-x86-64-elf binutils-x86-64-elf
    echo.
    echo   2. Download pre-built binaries:
    echo      https://github.com/lordmilko/i686-elf-tools
    echo.
    echo   3. Build from source (advanced):
    echo      https://wiki.osdev.org/GCC_Cross-Compiler
    echo.
)

echo.
echo ========================================
echo Next Steps
echo ========================================
echo.
echo 1. If cross-compiler is missing, install it using one of the options above.
echo 2. Build the OS: build.bat
echo 3. Run in QEMU: run.bat
echo.
echo For detailed instructions, see WINDOWS_SETUP.md
echo.
pause

