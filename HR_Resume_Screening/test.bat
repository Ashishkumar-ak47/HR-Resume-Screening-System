@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Iinclude test\test_module1.c src\module1.c src\module2.c src\utils.c src\employee.c -o build\test_module1.exe
if errorlevel 1 exit /b 1
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Iinclude test\test_module2.c src\module1.c src\module2.c src\utils.c src\employee.c -o build\test_module2.exe
if errorlevel 1 exit /b 1
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Iinclude test\test_employee.c src\employee.c -o build\test_employee.exe
if errorlevel 1 exit /b 1
call build.bat
if errorlevel 1 exit /b 1
build\test_module1.exe
if errorlevel 1 exit /b 1
build\test_module2.exe
if errorlevel 1 exit /b 1
build\test_employee.exe
if errorlevel 1 exit /b 1
if exist build\test_report.txt del /q build\test_report.txt
build\resume_screen.exe --skills sample\skills_software.txt sample\resume.txt --output build\test_report.txt
if errorlevel 1 exit /b 1
findstr /c:"Strong match" build\test_report.txt >nul
if errorlevel 1 exit /b 1
copy /y build\test_report.txt build\test_report_before.txt >nul
build\resume_screen.exe --skills sample\skills_software.txt sample\resume.txt --output build\test_report.txt >nul 2>nul
if errorlevel 2 exit /b 1
if not errorlevel 1 exit /b 1
fc /b build\test_report.txt build\test_report_before.txt >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe --help >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe --version
if errorlevel 1 exit /b 1
build\resume_screen.exe --unknown >nul 2>nul
if not errorlevel 2 exit /b 1
build\resume_screen.exe --skills >nul 2>nul
if not errorlevel 2 exit /b 1
build\resume_screen.exe >nul 2>nul
if not errorlevel 2 exit /b 1
type nul > build\empty_resume.txt
build\resume_screen.exe build\empty_resume.txt >nul 2>nul
if not errorlevel 1 exit /b 1
echo C++>build\invalid_skills.txt
build\resume_screen.exe --skills build\invalid_skills.txt sample\resume.txt >nul 2>nul
if not errorlevel 1 exit /b 1
build\resume_screen.exe employee add T100 "Test Employee" sample\resume.txt >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe employee list | findstr /c:"Test Employee" >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe employee set-resume T100 sample\resume.txt >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe employee screen T100 >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe employee remove T100 >nul
if errorlevel 1 exit /b 1
build\resume_screen.exe sample\resume.txt --output sample\resume.txt >nul 2>nul
if not errorlevel 2 exit /b 1
del /q build\empty_resume.txt build\invalid_skills.txt build\test_report_before.txt
echo All unit and CLI integration checks passed.
