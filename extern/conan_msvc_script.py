import platform
import subprocess
import sys
from pathlib import Path


def checkWindows() -> bool:
    if platform.system() != "Windows":
        print("❌ Этот скрипт предназначен только для Windows")
        return False
    return True


def findConan() -> bool:
    try:
        result = subprocess.run(
            ["conan", "--version"],
            capture_output=True,
            text=True,
            check=True
        )
        print(f"✓ {result.stdout.strip()}")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ Conan не найден в PATH")
        print("   Установите: pip install conan>=2.0")
        return False


def checkMSVC() -> bool:
    try:
        result = subprocess.run(
            ["cl"],
            capture_output=True,
            text=True,
            check=False
        )
        # cl без аргументов выводит версию в stderr
        output = result.stderr
        if "Microsoft (R) C/C++ Optimizing Compiler" in output:
            version_line = output.split('\n')[0]
            print(f"✓ {version_line}")

            # Проверка версии для C++23
            if "19.4" in output or "Version 19.4" in output:
                print("  ✓ Поддержка C++23 подтверждена")
            else:
                print("  ⚠️  Для C++23 нужен MSVC 19.40+ (VS 2022 17.10+)")
            return True
    except FileNotFoundError:
        pass

    print("❌ MSVC не найден")
    print("   Запустите из Developer Command Prompt for VS 2022")
    print("   Или установите Visual Studio 2022 с C++ workload")
    return False


def installDependencies(profileName: str) -> bool:
    scriptDir = Path(__file__).parent.resolve()

    print()
    print(f"📦 Установка зависимостей для профиля: {profileName}")
    print(f"   Директория проекта: {scriptDir}")
    print()

    try:
        result = subprocess.run(
            [
                "conan", "install", ".",
                f"--profile:host={profileName}",
                f"--profile:build={profileName}",
                "--build=missing"
            ],
            cwd=scriptDir,
            check=True
        )

        print()
        print("=" * 60)
        print("✅ Зависимости успешно установлены")
        print("=" * 60)
        return True

    except subprocess.CalledProcessError as e:
        print()
        print("=" * 60)
        print(f"❌ Ошибка установки зависимостей (код: {e.returncode})")
        print("=" * 60)
        return False


def main():
    print("=" * 60)
    print("  Установка Conan зависимостей")
    print("  Компилятор: MSVC")
    print("  Конфигурация: Release")
    print("  Стандарт: C++23")
    print("  Платформа: Windows")
    print("=" * 60)
    print()

    if not checkWindows():
        print()
        input("Нажмите Enter для выхода...")
        sys.exit(1)

    if not findConan():
        print()
        input("Нажмите Enter для выхода...")
        sys.exit(1)

    checkMSVC()

    success = installDependencies("windows_msvc_release")

    print()
    input("Нажмите Enter для выхода...")
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()