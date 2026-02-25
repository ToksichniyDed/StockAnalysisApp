cmake_minimum_required(VERSION 4.2.0)

# Путь к Conan extern
set(CONAN_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

set(POSSIBLE_TOOLCHAIN_PATHS
        "${CONAN_EXTERN_DIR}/build/Release/generators/conan_toolchain.cmake"
)

set(CMAKE_TOOLCHAIN_FILE_NOT_FOUND TRUE)
foreach(PATH IN LISTS POSSIBLE_TOOLCHAIN_PATHS)
    if(EXISTS "${PATH}")
        set(CMAKE_TOOLCHAIN_FILE "${PATH}" CACHE FILEPATH "Conan toolchain")
        set(CMAKE_TOOLCHAIN_FILE_NOT_FOUND FALSE)
        message(STATUS "Conan toolchain подключён: ${CMAKE_TOOLCHAIN_FILE}")
        break()
    endif()
endforeach()

if(CMAKE_TOOLCHAIN_FILE_NOT_FOUND)
    message(FATAL_ERROR "Conan toolchain не найден. Запусти conan install в extern (Release).")
endif()

project(CoolAppName
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

# Маппинг: Debug-сборка проекта использует Release-зависимости Conan
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release RELEASE "")
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release RELEASE "")
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release RELEASE "")


option(BUILD_TESTS "Build unit tests" ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(spdlog REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5105)
endif()