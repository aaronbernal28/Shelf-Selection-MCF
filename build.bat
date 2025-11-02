@echo off
echo Building Shelf Selection Simulation...
echo.

REM Create bin directory if it doesn't exist
if not exist bin mkdir bin

REM Compile with g++
g++ -std=c++17 -Wall -Wextra -I./include src/main.cpp -o bin/simulation.exe

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo Executable: bin\simulation.exe
    echo ========================================
    echo.
    echo Run with: bin\simulation.exe
    echo.
) else (
    echo.
    echo ========================================
    echo Build failed! Check errors above.
    echo ========================================
    echo.
)

pause
