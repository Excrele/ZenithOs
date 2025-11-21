@echo off
REM Build user programs (Windows)
REM Note: This requires WSL or a Linux-like environment

echo ========================================
echo Zenith OS - User Programs Build Script
echo ========================================
echo.

REM Check for WSL
where wsl >nul 2>&1
if %errorLevel% equ 0 (
    echo Using WSL to build user programs...
    echo.
    wsl bash -c "cd /mnt/f/Development/Zenith\ Os/ZenithOs/zenith-os/user && make"
    if %errorLevel% equ 0 (
        echo.
        echo Build successful!
        echo Output files in: user\build\
        goto :end
    ) else (
        echo Build failed. Make sure cross-compiler is installed in WSL.
        echo Run in WSL: sudo apt install gcc-x86-64-elf binutils-x86-64-elf
        pause
        exit /b 1
    )
) else (
    echo ERROR: WSL not found!
    echo.
    echo User programs require a Linux-like environment to build.
    echo Options:
    echo   1. Install WSL: wsl --install
    echo   2. Use MSYS2 or Cygwin
    echo   3. Build manually in a Linux VM
    echo.
    pause
    exit /b 1
)

:end
echo.
pause

