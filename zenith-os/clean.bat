@echo off
REM Clean build artifacts (Windows)

echo Cleaning build artifacts...

if exist stage1.bin del stage1.bin
if exist stage2.bin del stage2.bin
if exist kernel.bin del kernel.bin
if exist boot.img del boot.img
if exist kernel-stripped.bin del kernel-stripped.bin

if exist kernel\src\*.o (
    del kernel\src\*.o
    echo Cleaned kernel object files.
)

if exist user\build (
    echo Cleaning user build directory...
    rmdir /s /q user\build
)

echo Clean complete!

