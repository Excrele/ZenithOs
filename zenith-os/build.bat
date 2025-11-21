@echo off
REM Build script for Zenith OS (Windows)
REM This script builds the OS using available tools

echo ========================================
echo Zenith OS - Build Script
echo ========================================
echo.

REM Check for Make first
where make >nul 2>&1
if %errorLevel% equ 0 (
    echo Using Make...
    make -f Makefile.mk
    if %errorLevel% equ 0 (
        echo.
        echo Build successful!
        echo Output: boot.img
        goto :end
    ) else (
        echo Make build failed. Trying manual build...
        echo.
    )
)

REM Manual build using available tools
echo Building manually...
echo.

REM Check for NASM
where nasm >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: NASM not found!
    echo Please install NASM or run setup_windows.bat
    pause
    exit /b 1
)

REM Check for cross-compiler
where x86_64-elf-gcc >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: Cross-compiler (x86_64-elf-gcc) not found!
    echo.
    echo Please install cross-compiler:
    echo   - Use WSL: sudo apt install gcc-x86-64-elf binutils-x86-64-elf
    echo   - Or download from: https://github.com/lordmilko/i686-elf-tools
    echo.
    echo See WINDOWS_SETUP.md for details.
    pause
    exit /b 1
)

echo Building bootloader stage 1...
nasm -f bin boot\stage1\boot.asm -o stage1.bin
if %errorLevel% neq 0 (
    echo ERROR: Failed to build stage1.bin
    pause
    exit /b 1
)

echo Building bootloader stage 2...
nasm -f bin boot\stage2\stage2.asm -o stage2.bin
if %errorLevel% neq 0 (
    echo ERROR: Failed to build stage2.bin
    pause
    exit /b 1
)

echo.
echo Bootloader stages built successfully!
echo.
echo NOTE: Kernel build requires Make or WSL.
echo.
echo For best results, use one of these options:
echo   1. Use WSL: wsl make -f Makefile.mk
echo   2. Install Make: choco install make
echo      Then run: make -f Makefile.mk
echo   3. Use MSYS2 or Cygwin
echo.
echo Attempting to use Make if available...
where make >nul 2>&1
if %errorLevel% equ 0 (
    echo Using Make to build kernel...
    make -f Makefile.mk
    if %errorLevel% equ 0 (
        echo.
        echo Build successful!
        echo Output: boot.img
        goto :end
    ) else (
        echo Make build failed. Trying WSL...
    )
)

REM Try WSL if available
where wsl >nul 2>&1
if %errorLevel% equ 0 (
    echo Using WSL to build...
    wsl make -f Makefile.mk
    if %errorLevel% equ 0 (
        echo.
        echo Build successful!
        echo Output: boot.img
        goto :end
    )
)

echo.
echo Build incomplete. Please use WSL or install Make.
echo See WINDOWS_SETUP.md for detailed instructions.
echo.
echo Bootloader files (stage1.bin, stage2.bin) are ready.
echo Kernel build requires cross-compiler and Make.
pause
exit /b 1

:end
echo.
pause

