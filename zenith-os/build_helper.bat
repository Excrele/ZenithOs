@echo off
REM Helper script to combine bootloader and kernel files (Windows alternative to 'cat')

if "%1"=="" (
    echo Usage: build_helper.bat output_file input1 input2 input3
    exit /b 1
)

set OUTPUT=%1
shift

REM Create output file (empty)
type nul > "%OUTPUT%"

REM Append each input file
:loop
if "%1"=="" goto :done
type "%1" >> "%OUTPUT%"
shift
goto :loop

:done
echo Combined files into %OUTPUT%

