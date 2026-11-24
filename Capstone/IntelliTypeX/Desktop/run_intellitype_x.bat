@echo off
title IntelliType X - Build and Run
cd /d "%~dp0"
where gcc >nul 2>nul
if errorlevel 1 (
  echo GCC was not found. Install MSYS2 UCRT64 GCC and add its bin folder to PATH.
  pause
  exit /b 1
)
gcc -std=c11 -O2 -Wall -Wextra intellitype_x.c -o IntelliTypeX.exe
if errorlevel 1 (
  echo Build failed.
  pause
  exit /b 1
)
IntelliTypeX.exe
