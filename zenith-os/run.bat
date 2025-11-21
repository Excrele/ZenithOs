@echo off
REM Run Zenith OS in QEMU (Windows)

echo ========================================
echo Zenith OS - QEMU Launcher
echo ========================================
echo.

REM Check for QEMU
where qemu-system-x86_64 >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: QEMU not found!
    echo Please install QEMU or run setup_windows.bat
    pause
    exit /b 1
)

REM Check if boot.img exists
if not exist boot.img (
    echo ERROR: boot.img not found!
    echo Please build the OS first using build.bat or make
    pause
    exit /b 1
)

echo Starting QEMU...
echo.
echo Controls:
echo   - Ctrl+Alt+G: Release mouse/keyboard
echo   - Ctrl+Alt+F: Toggle fullscreen
echo   - Ctrl+C: Stop QEMU
echo.

REM Run QEMU
qemu-system-x86_64 -fda boot.img -serial stdio

echo.
echo QEMU exited.
pause

