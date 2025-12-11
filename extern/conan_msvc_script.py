import platform
import subprocess
import sys
from pathlib import Path

sys.stdout.reconfigure(encoding='utf-8')

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
    print("  Установка Conan зависимостей (MSVC, Release, C++23)")
    print("=" * 60)
    print()

    installDependencies("windows_msvc_release")

if __name__ == "__main__":
    main()