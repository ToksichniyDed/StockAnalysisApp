@echo off
REM Пытаемся найти conan.exe в PATH
where conan >nul 2>nul
if %errorlevel%==0 (
    set CONAN_EXE=conan
) else (
    echo Conan.exe не найден в PATH.
    pause
    exit /b 1
)

REM Определяем директорию, где лежит батник
set SCRIPT_DIR=%~dp0

REM Переход в директорию проекта
cd /d "%SCRIPT_DIR%"

REM Установка зависимостей
"%CONAN_EXE%" install . --profile:host windows_clang_debug --profile:build windows_clang_debug --build=missing

pause
