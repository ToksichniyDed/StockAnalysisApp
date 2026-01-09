find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(spdlog REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(AppMainWindow)
add_subdirectory(StockExchangeConnectors)

add_executable(${PROJECT_NAME} main.cpp)

add_library(Logger)
target_sources(Logger PUBLIC FILE_SET CXX_MODULES FILES Logger.ixx)
target_link_libraries(Logger PRIVATE spdlog::spdlog)

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt6::Core
        Qt6::Widgets
        spdlog::spdlog
        AppMainWindow)

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
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5105)
endif()