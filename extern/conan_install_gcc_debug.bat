@echo off
REM Найти conan.exe в PATH
where conan >nul 2>nul
if %errorlevel%==0 (
    set CONAN_EXE=conan
) else (
    echo Conan.exe не найден в PATH.
    pause
    exit /b 1
)

REM Директория батника
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

REM Установка зависимостей
"%CONAN_EXE%" install . --profile:host windows_gcc_debug --profile:build windows_gcc_debug --build=missing

pause
