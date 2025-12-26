cmake_minimum_required(VERSION 3.31)

# Путь к Conan extern (относительный от корня проекта, для SomeShit/c++)
set(CONAN_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../extern")

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

# Маппинг: Debug-сборка проекта использует Release-зависимости Conan
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release RELEASE "")
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release RELEASE "")
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release RELEASE "")

find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(AppMainWindow)
add_subdirectory(StockExchangeConnectors)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE AppMainWindow)

if(WIN32)
    get_target_property(_qt_qmake Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake}" DIRECTORY)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${_qt_bin_dir}/windeployqt.exe"
            --release
            "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Развёртывание Qt runtime"
            VERBATIM
    )
endif ()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5105 /wd5054)
    target_compile_options(${PROJECT_NAME} PRIVATE /MD$<$<CONFIG:Debug>:d>)
endif()