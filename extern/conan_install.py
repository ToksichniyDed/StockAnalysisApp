import shutil
from pathlib import Path
import sys

sys.stdout.reconfigure(encoding='utf-8')

#Путь до исходной папки
source_dir = Path("conan_profiles")

#Директория профилей Conan 2
conan_profiles_dir = Path.home() / ".conan2" / "profiles"
conan_profiles_dir.mkdir(parents=True, exist_ok=True)

#Копируем все элементы (файлы и подпапки) из source_dir в conan_profiles_dir (с overwrite)
for item in source_dir.iterdir():
    dest_item = conan_profiles_dir / item.name
    if item.is_dir():
        shutil.copytree(item, dest_item, dirs_exist_ok=True)
    else:
        shutil.copy(item, dest_item)

print(f"Все элементы из {source_dir} скопированы в {conan_profiles_dir}")