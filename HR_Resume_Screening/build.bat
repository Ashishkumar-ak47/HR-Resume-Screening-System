@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Iinclude src\main.c src\module1.c src\module2.c src\utils.c src\employee.c -o build\resume_screen.exe
if errorlevel 1 exit /b 1
echo Build successful: build\resume_screen.exe
