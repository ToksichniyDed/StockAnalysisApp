import shutil
from pathlib import Path

# Пути до исходных профилей (твоих конфигов)
source_profiles = {
    "windows_clang_debug": Path("conan_profiles/windows_clang_debug"),
    "windows_gcc_debug": Path("conan_profiles/windows_gcc_debug"),
    "windows_msvc_debug": Path("conan_profiles/windows_msvc_debug")
}

# Директория профилей Conan 2
conan_profiles_dir = Path.home() / ".conan2" / "profiles"
conan_profiles_dir.mkdir(parents=True, exist_ok=True)

for name, src_path in source_profiles.items():
    dest_path = conan_profiles_dir / name
    shutil.copy(src_path, dest_path)
    print(f"Профиль {name} скопирован в {dest_path}")
